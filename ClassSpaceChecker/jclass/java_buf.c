/* libjclass - Library for reading java class files
 * Copyright (C) 2003-2004 Nicos Panayides
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 *
 * $Id: java_buf.c 523 2004-10-20 15:15:42Z anarxia $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <jclass/class.h>

static uint16_t read_uint16(const char**);
static uint32_t read_uint32(const char**);
static ConstantPool* read_constant_pool(const char**);
static void get_next_entry(ConstantPoolEntry*, const char**);
static uint16_t* read_interfaces(const char**, uint16_t);
static Field* read_fields(const char**, uint16_t);
static AttributeContainer* read_attributes(const char**, uint16_t);

static uint16_t read_uint16(const char** buf)
{
	uint16_t bytes;
	
	memcpy(&bytes, *buf, 2);
	bytes = UINT16_NATIVE(bytes);
	*buf += 2;
	return bytes;
}

static uint32_t read_uint32(const char** buf)
{
	uint32_t bytes;
	
	memcpy(&bytes, *buf, 4);
	bytes = UINT32_NATIVE(bytes);
	*buf += 4;
	return bytes;
}

/**
* jclass_class_new_from_buffer
* @data: The buffer containing the class.
*
* Creates a JavaClass struct from the given buffer.
* The buffer should be in the same format as a class file.
*
* Returns: A JavaClass struct allocated with malloc.
*/
JavaClass* jclass_class_new_from_buffer(const char* data)
{
	JavaClass* class_struct;
	const char* buf = data;
	
	if(data == NULL)
		return NULL;
	
	if (read_uint32(&buf) != JAVA_CLASS_MAGIC)
		return NULL;
	
	class_struct = (JavaClass*) malloc(sizeof(JavaClass));
	
	class_struct->minor_version = read_uint16(&buf);
	class_struct->major_version = read_uint16(&buf);
	
	class_struct->constant_pool = read_constant_pool(&buf);
	
	class_struct->access_flags = read_uint16(&buf);
	class_struct->constant_pool->this_class = read_uint16(&buf);
	class_struct->constant_pool->super_class = read_uint16(&buf);
	
	class_struct->interfaces_count = read_uint16(&buf);
	class_struct->interfaces = read_interfaces(&buf, class_struct->interfaces_count);
	
	class_struct->fields_count = read_uint16(&buf);
	class_struct->fields = read_fields(&buf, class_struct->fields_count);
	
	class_struct->methods_count = read_uint16(&buf);
	class_struct->methods = read_fields(&buf, class_struct->methods_count);
	
	class_struct->attributes_count = read_uint16(&buf);
	class_struct->attributes = read_attributes(&buf, class_struct->attributes_count);

	return class_struct;
}

/**
* jclass_cp_new_from_buffer
* @data: A memory buffer containing a class file.
*
* Reads the constant pool of the class in the given buffer.
*
* Returns: A ConstantPool struct.
*/
ConstantPool* jclass_cp_new_from_buffer(const char* data)
{
	ConstantPool* cp;
	const char* buf = data;
	
	if(data == NULL)
		return NULL;
	
	if (read_uint32(&buf) != JAVA_CLASS_MAGIC)
		return NULL;
	
	read_uint16(&buf);
	read_uint16(&buf);
	
	cp = read_constant_pool(&buf);
	
	read_uint16(&buf);
	
	cp->this_class = read_uint16(&buf);
	cp->super_class = read_uint16(&buf);
	
	return cp;
}

static ConstantPool* read_constant_pool(const char** buf)
{
	ConstantPool* constant_pool;
	uint16_t count = 0;
				
	constant_pool = (ConstantPool*) malloc(sizeof(ConstantPool));
	constant_pool->count = read_uint16(buf);
	constant_pool->entries = (ConstantPoolEntry*) malloc(sizeof(ConstantPoolEntry) * constant_pool->count);
	
	constant_pool->entries[0].tag = CONSTANT_Empty;
	
	do
	{
		count++;
	 	get_next_entry(&(constant_pool->entries[count]), buf);
		
		if (constant_pool->entries[count].tag != CONSTANT_Empty)
		{
			/* For every double or long the next entry is reserved for the VM */
			if (constant_pool->entries[count].tag == CONSTANT_Long || constant_pool->entries[count].tag == CONSTANT_Double)
			{
				count++;
				constant_pool->entries[count].tag = CONSTANT_Empty;
			}
		}
		else
		{
			fputs("Unrecognised entry in the constant pool\n", stderr);
		}
	}	
	while(count < constant_pool->count - 1);
		
	return constant_pool;
}

static void get_next_entry(ConstantPoolEntry* info, const char** buf)
{
	memcpy(&info->tag, *buf, 1);
	(*buf)++;
	
	switch(info->tag)
	{
		case CONSTANT_Class:
			info->info.classinfo.name_index = read_uint16(buf);
			break;

		case CONSTANT_Fieldref:
		case CONSTANT_Methodref:
		case CONSTANT_InterfaceMethodref:
			info->info.ref.class_index = read_uint16(buf);
			info->info.ref.name_and_type_index = read_uint16(buf);
			break;

		case CONSTANT_String:
			info->info.stringinfo.string_index = read_uint16(buf);
			break;

		case CONSTANT_Integer:
		case CONSTANT_Float:
			info->info.integer.bytes = read_uint32(buf);
			break;

		case CONSTANT_Long:
		case CONSTANT_Double:
			info->info.longinfo = (LongEntry*) malloc(sizeof(LongEntry));
			info->info.longinfo->long_bytes = ((uint64_t) read_uint32(buf)) << 32;
			info->info.longinfo->long_bytes += read_uint32(buf);
			break;

		case CONSTANT_NameAndType:
			info->info.nameandtype.name_index = read_uint16(buf);
			info->info.nameandtype.descriptor_index = read_uint16(buf);
			break;

		case CONSTANT_Utf8:
			info->info.utf8 = (UTF8Entry*) malloc(sizeof(UTF8Entry));
			info->info.utf8->length = read_uint16(buf);
			if(info->info.utf8->length)
			{
				info->info.utf8->contents = (uint8_t*) malloc(sizeof(uint8_t) * info->info.utf8->length);
				memcpy(info->info.utf8->contents, *buf, info->info.utf8->length);
				(*buf) += info->info.utf8->length;
			}
			else
				info->info.utf8->contents = NULL;
					
			break;

		default:
			fprintf(stderr, "Unknown tag number: %d\n", info->tag);
			info->tag = CONSTANT_Empty;
		}
}

static uint16_t* read_interfaces(const char** buf, uint16_t count)
{
	uint16_t* interfaces;
	uint16_t i;
	
	if (buf == NULL)
		return NULL;
	
	if(count)
		interfaces = (uint16_t*) malloc(sizeof(uint16_t) * count);
	else
		interfaces = NULL;
	
	for(i = 0; i < count; i++)
		interfaces[i] = read_uint16(buf);
		
	return interfaces;
}

static Field* read_fields(const char** buf, uint16_t count)
{
	Field* field_array;
	uint16_t i;
	
	if(count)
	{
		field_array = (Field*) malloc(sizeof(Field) * count);
		
		for(i=0; i < count; i++)
		{
			field_array[i].access_flags = read_uint16(buf);
			field_array[i].name_index = read_uint16(buf);
			field_array[i].descriptor_index = read_uint16(buf);
			field_array[i].attributes_count = read_uint16(buf);
			
			field_array[i].attributes = read_attributes(buf, field_array[i].attributes_count);
		}
	}
	else
		field_array = NULL;	
		
	return field_array;
}

static AttributeContainer* read_attributes(const char** buf, uint16_t count)
{
	AttributeContainer* attributes;
	int j;
	
	if(count == 0)
		return NULL;
	
	attributes = (AttributeContainer*) malloc(sizeof(AttributeContainer) * count);
						
	for(j=0; j < count; j++)
	{
		attributes[j].name_index = read_uint16(buf);
		attributes[j].length = read_uint32(buf);
		
		if(attributes[j].length)
		{
			attributes[j].contents = (uint8_t*) malloc(sizeof(uint8_t) * attributes[j].length);
			memcpy(attributes[j].contents, *buf, attributes[j].length);
			(*buf) += attributes[j].length;
		}
		else
			attributes[j].contents = NULL;
	}
	
	return attributes;
}
