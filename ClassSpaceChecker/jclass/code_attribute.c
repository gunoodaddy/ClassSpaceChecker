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
 * $Id: code_attribute.c 523 2004-10-20 15:15:42Z anarxia $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <jclass/attributes.h>
#include <jclass/bytecode.h>
#include <jclass/jstring.h>

/**
* jclass_code_attribute_new
* @container: The attribute container.
*
* Extracts a code attribute from its attribute container.
* This is a copy and it should be freed when it is
* no longer needed.
*
* Returns: A newly constructed CodeAttribute.
*/
CodeAttribute* jclass_code_attribute_new(const AttributeContainer* container)
{
	CodeAttribute* code;
	uint32_t attribute_offset;
	uint16_t attribute_counter;
	uint32_t exception_counter;
	uint16_t temp;
	
	code = (CodeAttribute*) malloc(sizeof(CodeAttribute));
	
	memcpy(&code->max_stack, &(container->contents[0]), 2);
	code->max_stack = UINT16_NATIVE(code->max_stack);
	memcpy(&code->max_locals, &(container->contents[2]), 2);
	code->max_locals = UINT16_NATIVE(code->max_locals);
	memcpy(&code->code_length, &(container->contents[4]), 4);
	code->code_length = UINT32_NATIVE(code->code_length);
	
	if(code->code_length > 0)
	{
		code->code = (uint8_t*) malloc(code->code_length);
		memcpy(code->code, &(container->contents[8]), code->code_length);
	}
	else
		code->code = NULL;
	
	attribute_offset = 8 + code->code_length;
	memcpy(&code->exception_table_length, &(container->contents[attribute_offset]), 2);
	code->exception_table_length = UINT16_NATIVE(code->exception_table_length);
	attribute_offset += 2;
	
	if(code->exception_table_length > 0)
	{
		code->exception_table = (ExceptionTableEntry*)
			malloc(sizeof(ExceptionTableEntry) * code->exception_table_length);
		
		for(exception_counter = 0; exception_counter < code->exception_table_length; exception_counter++)
		{
			memcpy(&temp, &(container->contents[attribute_offset]), 2);
			code->exception_table[exception_counter].start_pc = UINT16_NATIVE(temp);
			attribute_offset += 2;
			
			memcpy(&temp, &(container->contents[attribute_offset]), 2);
			code->exception_table[exception_counter].end_pc = UINT16_NATIVE(temp);
			attribute_offset += 2;
			
			memcpy(&temp, &(container->contents[attribute_offset]), 2);
			code->exception_table[exception_counter].handler_pc = UINT16_NATIVE(temp);
			attribute_offset += 2;
			
			memcpy(&temp, &(container->contents[attribute_offset]), 2);
			code->exception_table[exception_counter].catch_type = UINT16_NATIVE(temp);
			attribute_offset += 2;
		}
	}
	else
		code->exception_table = NULL;
	
	memcpy(&code->attributes_count, &(container->contents[attribute_offset]), 2);
	code->attributes_count = UINT16_NATIVE(code->attributes_count);
	attribute_offset += 2;
	
	if(code->attributes_count > 0)
	{
		code->attributes = (AttributeContainer*) malloc(sizeof(AttributeContainer) * code->attributes_count);
		
		for(attribute_counter = 0; attribute_counter < code->attributes_count; attribute_counter++)
		{
			memcpy(&code->attributes[attribute_counter].name_index, &(container->contents[attribute_offset]), 2);
			code->attributes[attribute_counter].name_index = UINT16_NATIVE(code->attributes[attribute_counter].name_index);
			attribute_offset += 2;
			
			memcpy(&code->attributes[attribute_counter].length, &(container->contents[attribute_offset]), 4);
			code->attributes[attribute_counter].length = UINT32_NATIVE(code->attributes[attribute_counter].length);
			attribute_offset += 4;
			
			if(code->attributes[attribute_counter].length > 0)
			{
				code->attributes[attribute_counter].contents = (uint8_t*) 
					malloc(code->attributes[attribute_counter].length);
				
				memcpy(code->attributes[attribute_counter].contents, 
					&(container->contents[attribute_offset]), 
					code->attributes[attribute_counter].length);
			}
			else
				code->attributes[attribute_counter].contents = NULL;				
				
			attribute_offset += code->attributes[attribute_counter].length;
		}
	}
	else
		code->attributes = NULL;
	
	return code;
}

/**
* jclass_code_attribute_free
* @attribute: The Code attribute to free.
*
* Frees a code attribute.
*/
void jclass_code_attribute_free(CodeAttribute* attribute)
{
	uint32_t i;
	
	if(attribute->code != NULL)
		free(attribute->code);
	
	if(attribute->exception_table != NULL)
		free(attribute->exception_table);
	
	if(attribute->attributes != NULL)
	{
		for(i = 0; i < attribute->attributes_count; i++)
		{
			if(attribute->attributes[i].contents != NULL)
				free(attribute->attributes[i].contents);
		}			
		
		free(attribute->attributes);
	}
	
	free(attribute);
}


/**
* jclass_localvariable_attribute_new
* @container: The attribute container containing the localvariable attribute.
*
* Extracts a localvariable attribute from its attribute container.
* Local variable attributes are contained only within code attributes.
*
* Returns: A newly constructed LocalVariableAttribute.
*/
LocalVariableAttribute* jclass_localvariable_attribute_new(const AttributeContainer* container)
{
	LocalVariableAttribute* localvariabletable;
	uint32_t offset;	
	uint16_t counter;
	
	
	localvariabletable = (LocalVariableAttribute*) 
		malloc(sizeof(LocalVariableAttribute));
	localvariabletable->length = UINT16_NATIVE(*((uint16_t*) &container->contents[0]));
	offset = 2;
	
	if(localvariabletable->length)
		localvariabletable->localvariable = (LocalVariableInfo*) 
			malloc(localvariabletable->length * sizeof(LocalVariableInfo));
	else
		localvariabletable->localvariable = NULL;
	
	for(counter = 0; counter < localvariabletable->length; counter++)
	{
		localvariabletable->localvariable[counter].start_pc = UINT16_NATIVE(*((uint16_t*) &container->contents[offset]));
		offset += 2;
		
		localvariabletable->localvariable[counter].length = UINT16_NATIVE(*((uint16_t*) &container->contents[offset]));
		offset += 2;
		
		localvariabletable->localvariable[counter].name_index = UINT16_NATIVE(*((uint16_t*) &container->contents[offset]));
		offset += 2;
		
		localvariabletable->localvariable[counter].descriptor_index = UINT16_NATIVE(*((uint16_t*) &container->contents[offset]));
		offset += 2;
		
		localvariabletable->localvariable[counter].index = UINT16_NATIVE(*((uint16_t*) &container->contents[offset]));
		offset += 2;
	}
	
	return localvariabletable;
}

/**
* jclass_localvariable_attribute_free
* @attribute: The attribute to free.
*
* Frees a local variable attribute.
*/
void jclass_localvariable_attribute_free(LocalVariableAttribute* attrib)
{
	if(attrib->localvariable != NULL)
		free(attrib->localvariable);
	free(attrib);
}

/**
* jclass_linenumber_attribute_new
* @container: The attribute container holding the attribute.
*
* Extracts a line number attribute from its container.
*
* Returns: A newly constructed LineNumberAttribute.
*/
LineNumberAttribute* jclass_linenumber_attribute_new(const AttributeContainer* container)
{
	LineNumberAttribute* linenumbertable;
	uint16_t counter;
	uint32_t offset;
	
	linenumbertable = (LineNumberAttribute*) malloc(sizeof(LineNumberAttribute));
	linenumbertable->length = UINT16_NATIVE(*((uint16_t*) &container->contents[0]));
	offset = 2;
	
	if(linenumbertable->length)
		linenumbertable->line_number = (LineNumberInfo*) malloc(linenumbertable->length * sizeof(LineNumberInfo));
	else
		linenumbertable->line_number = NULL;
	
	for(counter = 0; counter < linenumbertable->length; counter++)
	{
		linenumbertable->line_number[counter].start_pc = UINT16_NATIVE(*((uint16_t*) &container->contents[offset]));
		offset += 2;
		linenumbertable->line_number[counter].line_number = UINT16_NATIVE(*((uint16_t*) &container->contents[offset]));
		offset += 2;
	}
	
	return linenumbertable;
}

/**
* jclass_linenumber_attribute_free
* @attribute: The line number attribute to free.
*
* Frees a line number attribute.
*/
void jclass_linenumber_attribute_free(LineNumberAttribute* attribute)
{
	if(attribute->line_number != NULL)
		free(attribute->line_number);
	free(attribute);
}
