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
 * $Id: java_file.c 523 2004-10-20 15:15:42Z anarxia $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <jclass/class.h>

static uint16_t fread_uint16(FILE*);
static uint32_t fread_uint32(FILE*);
static ConstantPool* fread_constant_pool(FILE*);
static void get_next_entry(FILE*, ConstantPoolEntry*);
static uint16_t* fread_interfaces(FILE*, uint16_t);
static Field* fread_fields(FILE*, uint16_t);
static AttributeContainer* fread_attributes(FILE*, uint16_t);

/* Reads 2 bytes from a file and converts them to native byte order.
*/
static uint16_t fread_uint16(FILE* classfile)
{
	uint16_t bytes;
	fread(&(bytes), sizeof(uint16_t), 1, classfile);
	
	bytes = UINT16_NATIVE(bytes);
	
	return bytes;
}

/* Reads 4 bytes from a file and converts them to native byte order.
*/
static uint32_t fread_uint32(FILE* classfile)
{
	uint32_t bytes;
	fread(&(bytes), sizeof(uint32_t), 1, classfile);
	
	bytes = UINT32_NATIVE(bytes);
	
	return bytes;
}

/**
* jclass_class_new_from_file
* @classfile: The file containing the class.
*
* Creates a JavaClass struct from the given class file.
* The file must be opened with "rb" permissions.
* The file will always be closed when the function returns.
*
* Returns: A JavaClass struct allocated with malloc.
*/
JavaClass* jclass_class_new_from_file(FILE* classfile)
{
	JavaClass* class_struct;

	if (classfile == NULL)
		return NULL;
	
	if (fread_uint32(classfile) != JAVA_CLASS_MAGIC)
	{
		fclose(classfile);
		return NULL;
	}
	
	class_struct = (JavaClass*) malloc(sizeof(JavaClass));
	
	class_struct->minor_version = fread_uint16(classfile);
	class_struct->major_version = fread_uint16(classfile);
	
	class_struct->constant_pool = fread_constant_pool(classfile);
	
	class_struct->access_flags = fread_uint16(classfile);
	class_struct->constant_pool->this_class = fread_uint16(classfile);
	class_struct->constant_pool->super_class = fread_uint16(classfile);
	class_struct->interfaces_count = fread_uint16(classfile);
	
	class_struct->interfaces = fread_interfaces(classfile, class_struct->interfaces_count);
	class_struct->fields_count = fread_uint16(classfile);
	class_struct->fields = fread_fields(classfile, class_struct->fields_count);
	
	class_struct->methods_count = fread_uint16(classfile);
	class_struct->methods = fread_fields(classfile, class_struct->methods_count);
	class_struct->attributes_count = fread_uint16(classfile);
	class_struct->attributes = fread_attributes(classfile, class_struct->attributes_count);
	fclose(classfile);
	return class_struct;
}

/**
* jclass_cp_new_from_file
* @classfile: The file containg the class.
*
* Reads the constant pool of the class from the given class file.
* The file must be opened with "rb" permissions.
* The file will always be closed when the function returns.
*
* Returns:  A ConstantPool struct.
*/
ConstantPool* jclass_cp_new_from_file(FILE* classfile)
{
	ConstantPool* constant_pool;
	int bytes;
	
	if (classfile == NULL)
		return NULL;
	
	if (fread_uint32(classfile) != JAVA_CLASS_MAGIC)
	{
		fclose(classfile);
		return NULL;
	}
	
	fread(&bytes, 4, 1, classfile);
	constant_pool = fread_constant_pool(classfile);
	
	fread(&bytes, 2, 1, classfile);
	
	constant_pool->this_class = fread_uint16(classfile);
	constant_pool->super_class = fread_uint16(classfile);
	fclose(classfile);
	return constant_pool;
}

static ConstantPool* fread_constant_pool(FILE* classfile)
{
	ConstantPool* constant_pool;
	uint16_t count = 0;

	constant_pool = (ConstantPool*) malloc(sizeof(ConstantPool));
	constant_pool->count = fread_uint16(classfile);
	constant_pool->entries = (ConstantPoolEntry*) malloc(sizeof(ConstantPoolEntry) * constant_pool->count);
	
	constant_pool->entries[0].tag = CONSTANT_Empty;
	
	do
	{
		count++;
	 	get_next_entry(classfile, &constant_pool->entries[count]);
		
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

/**
* get_next_entry
* @classfile: The file to read from.
* @info: A pointer to the ConstantPoolEntry struct to store the entry. 
*
* Reads the next constant pool entry from the class file.
*/
static void get_next_entry(FILE* classfile, ConstantPoolEntry* info)
{
	int tags_read;

	tags_read = fread(&(info->tag), sizeof(uint8_t), 1, classfile);

	if (tags_read < 1)
	{
		info->tag = 0;
		return;
	}

	switch(info->tag)
	{
		case CONSTANT_Class:
			info->info.classinfo.name_index = fread_uint16(classfile);
			break;

		case CONSTANT_Fieldref:
		case CONSTANT_Methodref:
		case CONSTANT_InterfaceMethodref:
			info->info.ref.class_index = fread_uint16(classfile);
			info->info.ref.name_and_type_index = fread_uint16(classfile);
			break;

		case CONSTANT_String:
			info->info.stringinfo.string_index = fread_uint16(classfile);
			break;

		case CONSTANT_Integer:
		case CONSTANT_Float:
			info->info.integer.bytes = fread_uint32(classfile);
			break;

		case CONSTANT_Long:
		case CONSTANT_Double:
			info->info.longinfo = (LongEntry*) malloc(sizeof(LongEntry));
			info->info.longinfo->long_bytes = ((uint64_t) fread_uint32(classfile)) << 32;
			info->info.longinfo->long_bytes += fread_uint32(classfile);
			break;

		case CONSTANT_NameAndType:
			info->info.nameandtype.name_index = fread_uint16(classfile);
			info->info.nameandtype.descriptor_index = fread_uint16(classfile);
			break;

		case CONSTANT_Utf8:
			info->info.utf8 = (UTF8Entry*) malloc(sizeof(UTF8Entry));
			info->info.utf8->length = fread_uint16(classfile);
			if(info->info.utf8->length)
			{
				info->info.utf8->contents = (uint8_t*) malloc(sizeof(uint8_t) * info->info.utf8->length);
				fread(info->info.utf8->contents, info->info.utf8->length, 1, classfile);
			}
			else
				info->info.utf8->contents = NULL;
				
			break;

		default:
			fprintf(stderr, "Unknown tag number: %d\n", info->tag);
			info->tag = CONSTANT_Empty;
	}
}

static uint16_t* fread_interfaces(FILE* classfile, uint16_t count)
{
	uint16_t* interfaces;
	uint16_t i;
	
	if(count)
		interfaces = (uint16_t*) malloc(sizeof(uint16_t) * count);
	else
		interfaces = NULL;
	
	for(i = 0; i < count; i++)
		interfaces[i] = fread_uint16(classfile);
		
	return interfaces;
}

/**
* fread_fields
* @classfile: The file to read the fields from.
* @count: The number of fields to read.
*
* Reads count number of fields from a class file.
*	
* Returns: An array of Fields read from a file. 
*/
static Field* fread_fields(FILE* classfile, uint16_t count)
{
	Field* field_array;
	uint16_t i;
	
	if(count)
	{
		field_array = (Field*) malloc(sizeof(Field) * count);
		
		for(i=0; i < count; i++)
		{
			field_array[i].access_flags = fread_uint16(classfile);
			field_array[i].name_index = fread_uint16(classfile);
			field_array[i].descriptor_index = fread_uint16(classfile);
			field_array[i].attributes_count = fread_uint16(classfile);
			
			field_array[i].attributes = fread_attributes(classfile, field_array[i].attributes_count);
		}
	}
	else
		field_array = NULL;	
		
	return field_array;
}

/**
* fread_attributes
* @classfile: The file to read the attributes from.
* @count: Number of attributes to read.
* 
* Helper function to read attributes from the file.
* The file pointer must be where the attributes start.
*
* Returns: An array of AttributeContainer structs with the attributes.
*/
static AttributeContainer* fread_attributes(FILE* classfile, uint16_t count)
{
	AttributeContainer* attributes;
	int j;
	
	if(!count)
		return NULL;
	
	attributes = (AttributeContainer*) malloc(sizeof(AttributeContainer) * count);
						
	for(j=0; j < count; j++)
	{
		attributes[j].name_index = fread_uint16(classfile);
		attributes[j].length = fread_uint32(classfile);
		
		if(attributes[j].length)
		{
			attributes[j].contents = (uint8_t*) malloc(sizeof(uint8_t) * attributes[j].length);
			fread(attributes[j].contents, attributes[j].length, 1, classfile);
		}
		else
			attributes[j].contents = NULL;
	}
	
	return attributes;
}
