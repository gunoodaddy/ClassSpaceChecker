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
 * $Id: constant_pool.c 523 2004-10-20 15:15:42Z anarxia $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <jclass/class.h>
#include <jclass/jstring.h>
#include <jclass/class_loader.h>

static void destroy_cp_info(ConstantPoolEntry*);

/**
 * jclass_cp_new
 * @filename: The name of the class or filename of the class.
 * @classpath: The classpath to use to locate the class.
 *
 * Constructs a ConstantPool struct with the constant pool of the given class.
 * If parsing fails it returns NULL.
 *
 * Returns: A newly constructed ConstantPool struct.
*/
ConstantPool* jclass_cp_new(const char* filename, const ClassPath *classpath)
{
	ConstantPool* new_cp = NULL;
	FILE* classfile;
	ClassFile* class_file_info;
	int is_filename;	
	
	if(filename == NULL)
		return NULL;
	
	is_filename = (strlen(filename) > 6) && !strcmp(".class", &filename[strlen(filename) - 6]);
	
	if(!is_filename)
	{		
		class_file_info = jclass_classloader_get_class_file(filename, classpath);
		
		if(class_file_info->data != NULL)
		{
			new_cp = jclass_cp_new_from_buffer(class_file_info->data);
			free(class_file_info->data);
		}
		else if(class_file_info->file_ptr != NULL)
		{
			new_cp = jclass_cp_new_from_file(class_file_info->file_ptr);
		}
		else
			new_cp = NULL;
		
		free(class_file_info);
	}
	else
	{
		classfile = fopen(filename, "rb");	
		new_cp = jclass_cp_new_from_file(classfile);
	}
	
	return new_cp;
}

/**
 * jclass_cp_free
 * @cpool: The constant pool to free.
 *
 * Frees a constant pool struct.
*/
void jclass_cp_free(ConstantPool* cpool)
{
	uint16_t i;
	
	for(i=1; i< cpool->count; i++)
		destroy_cp_info(&cpool->entries[i]);
	
	free(cpool->entries);	
	free(cpool);	
}

/**
* destroy_cp_info
* @info: The constant pool entry to free its contents.
* 
* Frees the contents of the given ConstantPoolEntry.
* The ConstantPoolEntry is not freed. Only its contents.
*/
void destroy_cp_info(ConstantPoolEntry* info)
{
	if((info->tag == CONSTANT_Long) ||
	  (info->tag == CONSTANT_Double))
	{
		free(info->info.longinfo);
	}
	else if(info->tag == CONSTANT_Utf8)
	{
		if(info->info.utf8->contents != NULL)
			free(info->info.utf8->contents);
				
		free(info->info.utf8);
	}
	
	info->tag = 0;
}

/**
* jclass_cp_get_this_class_name
* @cpool: The constant pool for the class.
*
* Gets the name of the class with the given constant pool.
*
* Returns: A string allocated with malloc.
*/
char* jclass_cp_get_this_class_name(const ConstantPool* cpool)
{
	return jclass_cp_get_class_name(cpool, cpool->this_class, 1);
}

/**
* jclass_cp_get_super_class_name
* @cpool: The constant pool for the class.
*
* Gets the name of the super class for the class with the given constant pool.
*
* Returns: A string allocated with malloc.
*/
char* jclass_cp_get_super_class_name(const ConstantPool* cpool)
{
	return jclass_cp_get_class_name(cpool, cpool->super_class, 1);
}

/**
* jclass_cp_get_class_name 
* @cpool: The constant pool.
* @index: The index of the class in the constant pool.
* @ignore_arrays:  Set to 1 to report class arrays as classes, 0 otherwise.
*
* Gets the name of the class with the given constant pool index.
* The entry must be of type %CONSTANT_Class.
* The class name is converted from the internal format to the
* fully qualifilied external (what it looks like in source code with
* no imports).
*
* Returns: The fully qualified name of the class.
*/
char* jclass_cp_get_class_name(const ConstantPool* constant_pool,
	uint16_t index, int ignore_arrays)
{
	uint16_t name_index;
	int i;
	int j;
	char* class_string;
	char* class_name;
	ConstantPoolEntry* cpool;
	int array_dimensions;
	
	if(index > constant_pool->count)
		return NULL;
	
	cpool = constant_pool->entries;
	
	if(cpool[index].tag == CONSTANT_Empty)
		return NULL;
	
	class_name = NULL;
	
	if (cpool[index].tag == CONSTANT_Class)
	{
		name_index = cpool[index].info.classinfo.name_index;
		/* if index = 0 then the class is anonymous */
		if(name_index)
		{
			class_name = jclass_utf8_to_string(
							cpool[name_index].info.utf8->contents,
							cpool[name_index].info.utf8->length);

			for(i = 0; i < strlen(class_name); i++)
			{
				if(class_name[i] == '/')
					class_name[i] = '.';
			}
			
			array_dimensions = 0;
			while(class_name[array_dimensions] == '[')
				array_dimensions++;

			if(array_dimensions)
			{
				if((strlen(class_name) - array_dimensions) == 1)
				{
					switch(class_name[array_dimensions])
					{
						case 'B':
							class_string = strdup("byte");
							break;
						case 'C':
							class_string = strdup("char");
							break;
						case 'D':
							class_string = strdup("double");
							break;
						case 'F':
							class_string = strdup("float");
							break;
						case 'I':
							class_string = strdup("int");
							break;
						case 'J':
							class_string = strdup("long");
							break;
						case 'S':
							class_string = strdup("short");
							break;
						case 'V':
							class_string = strdup("void");
							break;
						case 'Z':
							class_string = strdup("boolean");
							break;
						default:
							class_string = (char*) malloc(2);
							class_string[0] = class_name[array_dimensions];
							class_string[1] = '\0';
					}
				}
				else
				{
					class_string = (char*) malloc(strlen(class_name) + 1 - array_dimensions);
					strcpy(class_string, &class_name[array_dimensions + 1]);
					/* remove trailing ; */
					class_string[strlen(class_string) - 1] = '\0';
				}
				free(class_name);
				class_name = (char*) malloc(strlen(class_string) + (array_dimensions*2) + 1);
				strcpy(class_name, class_string);
				free(class_string);				

				if(!ignore_arrays)
				{
					j = strlen(class_name);
					for(i = 0; i < array_dimensions; i++)
					{
						class_name[j + (i*2)] = '[';
						class_name[j + (i*2) + 1] = ']';
					}
					class_name[j + (array_dimensions*2)] = '\0';
				}
			}

		}
	}
	return class_name;
}

/**
* jclass_cp_get_entry_type
* @cpool: The constant pool.
* @index: The index of the entry.
*
* Gets the type of the given constant pool entry.
*
* @Since: 0.3
*
* Returns: The entry type.
*/
ConstantTag jclass_cp_get_entry_type(const ConstantPool* cpool, uint16_t index)
{
	if(index > cpool->count)
		return CONSTANT_Empty;
	else
		return (ConstantTag)cpool->entries[index].tag;
}

/**
* jclass_cp_get_method_signature
* @cpool: The constant pool.
* @index: The index of the method in the contant pool.
* @return_type: Set to 0 to prevent the return type from being included.
*
* Gets the signature of a method as a string.
*
* Returns: A newly created string allocated with malloc.
*/
char* jclass_cp_get_method_signature(const ConstantPool* constant_pool,
	uint16_t index, int return_type)
{
	int name_index;
	int type_index;
	int name_and_type_index;
	int class_name_index;
	int string_size;
	char* full_method_name;
	char* class_name;
	char* method_name;
	char* descriptor;
	char* parameters;
	char* type;
	ConstantPoolEntry* cpool;
	
	if(index > constant_pool->count)
		return NULL;
	
	cpool = constant_pool->entries;
	full_method_name = NULL;
	
	if(cpool[index].tag == CONSTANT_Empty)
		return NULL;
		
	if ((cpool[index].tag == CONSTANT_Methodref) ||
		(cpool[index].tag == CONSTANT_InterfaceMethodref) ||
		(cpool[index].tag == CONSTANT_Fieldref)
		)
	{
		class_name_index = cpool[index].info.ref.class_index;
		
		class_name = jclass_cp_get_class_name(constant_pool, class_name_index, 0);
		
		name_and_type_index = cpool[index].info.ref.name_and_type_index;
		name_index = cpool[name_and_type_index].info.nameandtype.name_index;
		type_index = cpool[name_and_type_index].info.nameandtype.descriptor_index;
		
		method_name = jclass_utf8_to_string(
					cpool[name_index].info.utf8->contents,
					cpool[name_index].info.utf8->length);
		
		if(!strcmp(method_name,"<init>"))
		{
			free(method_name);
			method_name = NULL;
		}
		
		descriptor = jclass_utf8_to_string(
						cpool[type_index].info.utf8->contents,
						cpool[type_index].info.utf8->length);
		
		parameters = jclass_descriptor_get_parameters(descriptor);
		type = jclass_descriptor_get_type(descriptor);
		free(descriptor);
		
		if(method_name == NULL)
			string_size = 0;
		else
			string_size = strlen(type) + strlen(method_name) + 2;
			
		string_size += strlen(class_name) + strlen(parameters) + 1;
		full_method_name = (char*) malloc(sizeof(char) * string_size);
		
		if(method_name != NULL)
		{
			if(return_type) 
			{
				strcpy(full_method_name, type);
				strcat(full_method_name, " ");
			}
			else
				full_method_name[0] = '\0';
		}
		else
			full_method_name[0] = '\0';
			
		strcat(full_method_name, class_name);
		
		if(method_name != NULL)
		{
			strcat(full_method_name, ".");
			strcat(full_method_name, method_name);
			free(method_name);
		}
		strcat(full_method_name, parameters);
		
		free(type);
		free(class_name);
		free(parameters);
	}
	
	return full_method_name;
}

/**
* jclass_cp_get_constant_value
* @cpool: The constant pool.
* @index: The index of the entry on the constant pool.
* @int_type: The type of the entry for integer constants.
* If the entry is not of type %CONSTANT_Integer this value is ignored.
*
* Creates a string representation of a constant pool constant.
* This is for entries of type %CONSTANT_Float, %CONSTANT_Double, %CONSTANT_Long
* %CONSTANT_Integer, %CONSTANT_String and %CONSTANT_Utf8.
*
* Returns: A string allocated with malloc.
*/
char* jclass_cp_get_constant_value(const ConstantPool* constant_pool, uint16_t cp_index, IntType int_type)
{
	char* value;
	char* temp_string;
	char* original_string;
	int32_t integer;
	ConstantPoolEntry* cpool;
	
	if(cp_index > constant_pool->count)
		return NULL;
	
	cpool = constant_pool->entries;
	switch(cpool[cp_index].tag)
	{
			case CONSTANT_Float:
				value = jclass_float_to_string(cpool[cp_index].info.integer.bytes);
				break;

			case CONSTANT_Double:
				value = jclass_double_to_string(cpool[cp_index].info.longinfo->long_bytes);
				break;
				
			case CONSTANT_Long:
				value = (char*) malloc(50);
#if	SIZEOF_LONG == 8
				snprintf(value, 50, "%ld", cpool[cp_index].info.longinfo->long_bytes);
#else
				_snprintf(value, 50, "%Ld", cpool[cp_index].info.longinfo->long_bytes);
#endif			
				break;
			case CONSTANT_Integer:
				integer = *((int32_t*) &cpool[cp_index].info.integer.bytes);
				switch(int_type)
				{
					case INT_IS_BOOLEAN:
						value = (integer ? strdup("true") : strdup("false"));
						break;
						
					case INT_IS_CHAR:
						temp_string = (char*) malloc(4);
						temp_string[0] = '\'';
						temp_string[1] = (integer & 0x000000ff);
						temp_string[2] = '\'';
						temp_string[3] = '\0';
						value = jclass_get_printable_string(temp_string);
						free(temp_string);
						break;
					
					case INT_IS_SHORT:
						value = (char*) malloc(7);
						_snprintf(value, 7, "%d", integer);
						break;
					
					case INT_IS_BYTE:
						value = (char*) malloc(5);
						_snprintf(value, 5, "%d", integer);
						break;
					
					default:
						value = (char*) malloc(13);
						_snprintf(value, 13, "%d", integer);
				}
				break;
				
			case CONSTANT_String:
				cp_index = cpool[cp_index].info.stringinfo.string_index;
				original_string = jclass_utf8_to_string(
									cpool[cp_index].info.utf8->contents,
	 								cpool[cp_index].info.utf8->length);
			
				value = jclass_get_printable_string(original_string);
				free(original_string);
				break;
			case CONSTANT_Utf8:
				original_string = jclass_utf8_to_string(
								cpool[cp_index].info.utf8->contents,
	 							cpool[cp_index].info.utf8->length);
			
				value = jclass_get_printable_string(original_string);
				free(original_string);
				break;
			default:
				value = NULL;
		}
		
	return value;
}
