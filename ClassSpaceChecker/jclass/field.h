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
 * $Id: field.h 523 2004-10-20 15:15:42Z anarxia $
 */

#ifndef __JCLASS_FIELD_H__
#define __JCLASS_FIELD_H__

#ifdef _cplusplus
  extern "C" {
#endif 

#include <jclass/attributes.h>	  

typedef enum {
	V_PUBLIC,
	V_PACKAGE,
	V_PROTECTED,
	V_PRIVATE,
	V_SYNTHETIC
} JCVisibility;

typedef struct {
	/* A 16-bit integer with the access flags for this field/method */
	uint16_t access_flags;
	/* The index in the constant pool for the UTF8Entry 
	with the name of this field */
	uint16_t name_index;
	/* The index in the constant pool for the UTF8Entry 
	with the descriptor for this field */
	uint16_t descriptor_index;
	/* The number of attributes for this field */
	uint16_t attributes_count;
	/* The attributes for this field */
	AttributeContainer* attributes;
} Field;

int jclass_field_is_visible(const Field* field, const ConstantPool* cpool, JCVisibility visibility);
char* jclass_field_get_name(const Field* field, const ConstantPool* cpool);
char* jclass_field_get_descriptor(const Field* field, const ConstantPool* cpool);
CodeAttribute* jclass_field_get_code_attribute(const Field* field, const ConstantPool* cpool);
	  
#ifdef _cplusplus
  }
#endif   
#endif /* __JCLASS_FIELD_H__ */
