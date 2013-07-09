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
 * $Id: attributes.h 523 2004-10-20 15:15:42Z anarxia $
 */

#ifndef __JCLASS_ATTRIBUTES_H__
#define __JCLASS_ATTRIBUTES_H__

#ifdef _cplusplus
 extern "C" {
#endif 

#include <jclass/constant_pool.h>

typedef struct {
	/* The index of the utf8_info entry in the constant pool
	* with the name of the attribute.
	*/
	uint16_t name_index;
	/* The length in bytes of the contents of the attribute */
	uint32_t length;
	/* The attribute. */
	uint8_t* contents;
} AttributeContainer;

/* Class Attributes */

typedef struct {
	uint16_t type_index;	     
    uint16_t outer_class_type_index;	     
    uint16_t name_index;	     
    uint16_t access_flags;	
} InnerClassInfo;

typedef struct {
	uint16_t no_innerclasses;
    InnerClassInfo* classes;
} InnerClassesAttribute;

typedef struct {
	uint16_t no_exceptions;
    uint16_t* exception_index;
} ExceptionsAttribute;

typedef struct {
	/* Index to the constant pool for a utf8 entry
	with the name of the source file.
	*/
	uint16_t filename_index;
} SourceFileAttribute;

typedef struct {
	/* Index to the constant pool for an entry
	with the constant value.
	*/
	uint16_t cp_index;
} ConstantValueAttribute;

typedef struct {
	/* The start PC of the exception handler scope. */
	uint16_t start_pc;
	/* The end PC of the exception handler scope. */
    uint16_t end_pc;
	/* The PC where the exception handler is found. */
    uint16_t handler_pc;
	/* The index in the constant pool for the exception class.
	* An index of 0 indicates all exceptions will be handled (finally).
	*/
    uint16_t catch_type;
} ExceptionTableEntry;

typedef struct {
	uint16_t max_stack;
    uint16_t max_locals;
	/* Length of the code in bytes */
    uint32_t code_length;
	/* The code. It is stored in big-endian order */
    uint8_t* code;
	/* The length of the exception table */
    uint16_t exception_table_length;
	/* Holds information about the exceptions and handlers
	* in the code.
	*/
    ExceptionTableEntry* exception_table;
	/* Number of attributes */
    uint16_t attributes_count;
	/* Attributes for the code */
    AttributeContainer* attributes;
} CodeAttribute;

typedef struct {
	/* The index in the code array that the line begins */
	uint16_t start_pc;
	/* The line number in the source code */	
   	uint16_t line_number;	  
} LineNumberInfo;

typedef struct {
	/* The number of line numbers (not length in bytes) */
	uint16_t length;
    LineNumberInfo* line_number;     
} LineNumberAttribute;

typedef struct {
	/* The PC where the scope of the variable begins */
	uint16_t start_pc;
	/* The length of the scope of the variable*/
	uint16_t length;
	/* The index in the constant pool of the UTF8Entry 
	* with the name of the local variable.
	*/
	uint16_t name_index;
	/* The index in the constant pool of the UTF8Entry 
	* with the descriptor of the local variable.
	*/
	uint16_t descriptor_index;
	/* The index of the variable */
	uint16_t index;
} LocalVariableInfo;

typedef struct {
	uint16_t length;
	LocalVariableInfo* localvariable;
} LocalVariableAttribute;

int jclass_attribute_container_has_attribute(const AttributeContainer* container, 
											const char* attribute_name,
											const ConstantPool* cpool);

SourceFileAttribute* jclass_sourcefile_attribute_new(const AttributeContainer* container);
void jclass_sourcefile_attribute_free(SourceFileAttribute* attribute);

ConstantValueAttribute* jclass_constantvalue_attribute_new(const AttributeContainer* container);
void jclass_constantvalue_attribute_free(ConstantValueAttribute* attribute);

ExceptionsAttribute* jclass_exceptions_attribute_new(const AttributeContainer* container);
void jclass_exceptions_attribute_free(ExceptionsAttribute* attribute);

InnerClassesAttribute* jclass_innerclasses_attribute_new(const AttributeContainer* container);
void jclass_innerclasses_attribute_free(InnerClassesAttribute* attribute);

CodeAttribute* jclass_code_attribute_new(const AttributeContainer* container);
void jclass_code_attribute_free(CodeAttribute* attribute);

LineNumberAttribute* jclass_linenumber_attribute_new(const AttributeContainer* container);
void jclass_linenumber_attribute_free(LineNumberAttribute* attribute);

LocalVariableAttribute* jclass_localvariable_attribute_new(const AttributeContainer* container);
void jclass_localvariable_attribute_free(LocalVariableAttribute* attribute);

#ifdef _cplusplus
 }
#endif 

#endif
