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
 * $Id: constant_pool.h 523 2004-10-20 15:15:42Z anarxia $
 */

#ifndef __JCLASS_CONSTANT_POOL_H__
#define __JCLASS_CONSTANT_POOL_H__

#ifdef _cplusplus
 extern "C" {
#endif 

#include <stdio.h>
#include <jclass/types.h>
#include <jclass/class_loader.h>

typedef enum {
	CONSTANT_Empty = 0,
	CONSTANT_Utf8 =	1, 
	CONSTANT_Integer = 3,
	CONSTANT_Float = 4,
	CONSTANT_Long = 5,
	CONSTANT_Double	= 6,
	CONSTANT_Class = 7,
	CONSTANT_String = 8,
	CONSTANT_Fieldref = 9,
	CONSTANT_Methodref = 10,
	CONSTANT_InterfaceMethodref = 11,
	CONSTANT_NameAndType = 12
} ConstantTag;

typedef enum {
	INT_IS_INT,
	INT_IS_BOOLEAN,
	INT_IS_CHAR,
	INT_IS_SHORT,
	INT_IS_BYTE
} IntType;

/* Constant pool entries */

typedef struct {
		/* The index in the constant pool of the utf8_info entry 
		* with the name of the class. */
    	uint16_t name_index;
} ClassEntry;

typedef struct {
		/* The index in the constant pool of the ClassEntry 
		* of the class the field/method is a member of.
		*/
    	uint16_t class_index;
		/* The index in the constant pool of the NameAndTypeEntry 
		* with the description of the field/method.
		*/
    	uint16_t name_and_type_index;
} ReferenceEntry;

typedef struct {
		/* The index in the constant pool of the UTF8Entry
		* represending the initial value of the string.
		*/
    	uint16_t string_index;
} StringEntry;

typedef struct {
		/* The initial value of the int/float */
    	uint32_t bytes;
} IntegerEntry;

typedef struct {
    	uint64_t long_bytes;
} LongEntry;

typedef struct {
		/* The index in the constant pool of the utf8_info entry 
		* with the name of the field/method.*/
    	uint16_t name_index;
		/* The index in the constant pool of the utf8_info entry 
		* with the coded field/method descriptor.*/
    	uint16_t descriptor_index;
} NameAndTypeEntry;

typedef struct {
		/* The length of the data in bytes */
    	uint16_t length;
		/* The UTF-8 data */
		uint8_t* contents;
} UTF8Entry;

typedef struct {
		/* It's value determines the type of the entry. */
		uint8_t tag;
		union {
			UTF8Entry* utf8;
			LongEntry* longinfo;
			/* structs with size <= size of pointer */
			NameAndTypeEntry nameandtype;
			IntegerEntry integer;
			ReferenceEntry ref;
			StringEntry stringinfo;
			ClassEntry classinfo;
		}info;
} ConstantPoolEntry;

typedef struct {
	/* Number of entries */
	uint16_t count;
	/* The index of this class in the constant pool */
	uint16_t this_class;
	/* The index of the super class in the constant pool */
	uint16_t super_class;
	/* An array with the entries */
	ConstantPoolEntry* entries;
} ConstantPool;


ConstantPool* jclass_cp_new(const char* filename, const ClassPath *classpath);
ConstantPool* jclass_cp_new_from_buffer(const char* data);
ConstantPool* jclass_cp_new_from_file(FILE* classfile);

void jclass_cp_free(ConstantPool* cpool);

char* jclass_cp_get_this_class_name(const ConstantPool* cpool);
char* jclass_cp_get_super_class_name(const ConstantPool* cpool);
char* jclass_cp_get_class_name(const ConstantPool* cpool, uint16_t index, int ignore_arrays);
char* jclass_cp_get_method_signature(const ConstantPool* cpool, uint16_t index, int return_type);
char* jclass_cp_get_constant_value(const ConstantPool* cpool, uint16_t index, IntType int_type);
ConstantTag jclass_cp_get_entry_type(const ConstantPool* cpool, uint16_t index);

#ifdef _cplusplus
 }
#endif 

#endif
