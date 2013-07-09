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
 * $Id: class.h 523 2004-10-20 15:15:42Z anarxia $
 */

#ifndef __JCLASS_CLASS_H__
#define __JCLASS_CLASS_H__

#ifdef _cplusplus
  extern "C" {
#endif 

#include <jclass/field.h>
#include <jclass/class_loader.h>

#define JAVA_CLASS_MAGIC 0xCAFEBABE

/* Masks for access flags */
#define ACC_PUBLIC 0x0001 
#define ACC_PRIVATE	0x0002
#define ACC_PROTECTED 0x0004
#define ACC_STATIC 0x0008
#define ACC_FINAL 0x0010
#define ACC_SYNCHRONIZED 0x0020
#define ACC_VOLATILE 0x0040
#define ACC_TRANSIENT 0x0080
#define ACC_NATIVE 0x0100
#define ACC_INTERFACE 0x0200
#define ACC_ABSTRACT 0x0400
#define ACC_STRICTFP 0x0800

typedef struct {
	uint16_t minor_version;
	uint16_t major_version;
	ConstantPool *constant_pool;
	uint16_t access_flags;
	uint16_t interfaces_count;
	uint16_t *interfaces;
	uint16_t fields_count;
	Field *fields;
	uint16_t methods_count;
	Field *methods;
	uint16_t attributes_count;
	AttributeContainer *attributes;
} JavaClass;

JavaClass* jclass_class_new(const char *filename, const ClassPath* classpath);

JavaClass* jclass_class_new_from_buffer(const char *data);
JavaClass* jclass_class_new_from_file(FILE *classfile);
void jclass_class_free(JavaClass *javaclass);

const char* jclass_class_get_vm_spec(const JavaClass *javaclass);
char* jclass_class_get_class_name(const JavaClass *javaclass);
char* jclass_class_get_super_class_name(const JavaClass *javaclass);
char* jclass_class_get_package_name(const JavaClass *javaclass);
char* jclass_class_get_sourcefile_name(const JavaClass *javaclass);
char **jclass_class_get_interfaces(const JavaClass *class_struct);

#ifdef _cplusplus
 }
#endif 

#endif
