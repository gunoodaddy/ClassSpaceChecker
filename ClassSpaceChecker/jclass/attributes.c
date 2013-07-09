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
 * $Id: attributes.c 523 2004-10-20 15:15:42Z anarxia $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <jclass/attributes.h>
#include <jclass/jstring.h>

/**
* jclass_attribute_container_has_attribute
* @container: The container to check.
* @attribute_name: The name of the attribute to look for.
* @cpool: The constant pool for the class.
*
* Checks if the given attribute container contains a specific attribute.
*
* Returns:  1 if the container has the given attribute, 0 otherwise.
*/
int jclass_attribute_container_has_attribute(const AttributeContainer* container, 
	const char* attribute_name, const ConstantPool* constant_pool)
{
	int is_synthetic;
	char* attr_name;
	UTF8Entry* attr_info;
	
	if(container == NULL)
		return 0;

	attr_info = constant_pool->entries[container->name_index].info.utf8;
	attr_name = jclass_utf8_to_string(attr_info->contents, attr_info->length);

	is_synthetic = !strcmp(attribute_name, attr_name);
	free(attr_name);

	return is_synthetic;
}

/**
* jclass_constantvalue_attribute_new
* @container: The attribute container.
*
* Extracts an constantvalue attribute from its attribute container.
*
* Returns: A newly constructed ConstantValue attribute.
*/
ConstantValueAttribute* jclass_constantvalue_attribute_new(const AttributeContainer* container)
{
	ConstantValueAttribute* attribute;
	attribute = (ConstantValueAttribute*) malloc(sizeof(ConstantValueAttribute));
	
	memcpy(&attribute->cp_index, &(container->contents[0]), 2);
	attribute->cp_index = UINT16_NATIVE(attribute->cp_index);
	return attribute;
}

/**
* jclass_constantvalue_attribute_free
* @attribute: The constant value attribute to free.
*
* Frees a constant value attribute.
*/
void jclass_constantvalue_attribute_free(ConstantValueAttribute* attribute)
{
	free(attribute);
}

/**
* jclass_sourcefile_attribute_new
* @container: The attribute container.
*
* Extracts a sourcefile attribute from its attribute container.
*
* Returns: A newly constructed SourceFileAttribute.
*/
SourceFileAttribute* jclass_sourcefile_attribute_new(const AttributeContainer* container)
{
	SourceFileAttribute* attribute;
	attribute = (SourceFileAttribute*) malloc(sizeof(SourceFileAttribute));
	
	memcpy(&attribute->filename_index, &(container->contents[0]), 2);
	attribute->filename_index = UINT16_NATIVE(attribute->filename_index);
	
	return attribute;
}

/**
* jclass_sourcefile_attribute_free
* @attribute: The source file attribute to free.
*
* Frees a source file attribute. 
*/
void jclass_sourcefile_attribute_free(SourceFileAttribute* attribute)
{
	free(attribute);
}

/**
* jclass_exceptions_attribute_new
* @container: The attribute container.
*
* Extracts an exceptions attribute from its attribute container.
*
* Returns: A newly constructed ExceptionsAttribute.
*/
ExceptionsAttribute* jclass_exceptions_attribute_new(const AttributeContainer* container)
{
	ExceptionsAttribute* attribute;
	uint16_t j;
	
	attribute = (ExceptionsAttribute*) malloc(sizeof(ExceptionsAttribute));
	
	memcpy(&attribute->no_exceptions, &(container->contents[0]), 2);
	attribute->no_exceptions = UINT16_NATIVE(attribute->no_exceptions);
	
	if(attribute->no_exceptions)
		attribute->exception_index = (uint16_t*) malloc(sizeof(uint16_t) * attribute->no_exceptions);
	else
		attribute->exception_index = NULL;

	for(j = 0; j < attribute->no_exceptions; j++)
	{
		memcpy(&attribute->exception_index[j], &(container->contents[(j*2) + 2]), 2);
		attribute->exception_index[j] = UINT16_NATIVE(attribute->exception_index[j]);
	}
	
	return attribute;
}

/**
* jclass_exceptions_attribute_free
* @attribute: The exception attribute to free.
*
* Frees an exceptions attribute.
*/
void jclass_exceptions_attribute_free(ExceptionsAttribute* attribute)
{
	if(attribute->exception_index != NULL)
		free(attribute->exception_index);
	
	free(attribute);
}

/**
* jclass_innerclasses_attribute_new
* @container: The attribute container.
*
* Extracts an innerclass attribute from its attribute container.
*
* Returns: A newly constructed InnerClassesAttribute.
*/
InnerClassesAttribute* jclass_innerclasses_attribute_new(const AttributeContainer* container)
{
	InnerClassesAttribute* attrib;
	uint16_t j;
	uint16_t temp;
	
	attrib = (InnerClassesAttribute*) malloc(sizeof(InnerClassesAttribute));
	
	memcpy(&temp, &(container->contents[0]), 2);
	attrib->no_innerclasses = UINT16_NATIVE(temp);
	
	if(attrib->no_innerclasses)
		attrib->classes = (InnerClassInfo*) malloc(attrib->no_innerclasses * sizeof(InnerClassInfo));
	else
		attrib->classes = NULL;
		
	for(j = 0; j < attrib->no_innerclasses; j++)
	{
		memcpy(&temp, &(container->contents[(j*8) + 2]), 2);
		attrib->classes[j].type_index = UINT16_NATIVE(temp);
		memcpy(&temp, &(container->contents[(j*8) + 4]), 2);
		attrib->classes[j].outer_class_type_index = UINT16_NATIVE(temp);
		memcpy(&temp, &(container->contents[(j*8) + 6]), 2);
		attrib->classes[j].name_index = UINT16_NATIVE(temp);
		memcpy(&temp, &(container->contents[(j*8) + 8]), 2);
		attrib->classes[j].access_flags = UINT16_NATIVE(temp);
	}

	return attrib;
}

/**
* jclass_innerclasses_attribute_free
* @attribute: The innerclasses attribute to free.
*
* Frees an innerclasses attribute.
*/
void jclass_innerclasses_attribute_free(InnerClassesAttribute* attribute)
{
	if(attribute->classes != NULL)
		free(attribute->classes);
	free(attribute);
}
