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
 * $Id: class.c 536 2011-12-23 02:30:27Z anarxia $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <jclass/class.h>
#include <jclass/jstring.h>
#include <jclass/class_loader.h>

/*
*
* libjclass API reference
* The goal of the project is to provide an easy way to extract 
* information from java class files.
*
* Credits
* Information that made this project possible was obtained from:\n
*
* File Format
* "The JavaTM Virtual Machine Specification", Second Edition\n
* by Tim Lindholm and Frank Yellin\n
* Copyright(c) 1999 Sun Microsystems Inc.\n
* Available at:
* http://java.sun.com/docs/books/vmspec/2nd-edition/html/ClassFile.doc.html
*
* Instructions
* "Java Virtual Machine Online Instruction Reference"\n
* by Jon Meyer and Troy Downing, O'Reilly Associates\n
* Available at:
* http://mrl.nyu.edu/~meyer/jvmref/ref-Java.html
*/


/**
* jclass_class_new
* @filename: The filename or classname for the class.
* @classpath: The classpath to use to locate the class.
*
* Initializes a new JavaClass struct with info from the given file/class.
* If parsing fails it returns NULL. 
* Use jclass_class_free() to free the class.
*
* Returns: A JavaClass struct allocated with malloc.
*/
JavaClass* 
jclass_class_new(const char* filename, const ClassPath *classpath)
{
	JavaClass* new_class = NULL;
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
			new_class = jclass_class_new_from_buffer(class_file_info->data);
			free(class_file_info->data);
		}
		else if(class_file_info->file_ptr != NULL)
		{
			new_class = jclass_class_new_from_file(class_file_info->file_ptr);
		}
		else
			new_class = NULL;
		
		free(class_file_info);
	}
	else
	{
		classfile = fopen(filename, "rb");	
		new_class = jclass_class_new_from_file(classfile);
	}
	
	return new_class;
}

/**
* jclass_class_free
* @javaclass: The JavaClass struct to free.
*
* Frees a JavaClass struct.
*/
void jclass_class_free(JavaClass* class_struct)
{
	int i;
	int j;
	
	if(class_struct->constant_pool != NULL)
		jclass_cp_free(class_struct->constant_pool);
	
	if(class_struct->interfaces != NULL)
		free(class_struct->interfaces);
		
	if(class_struct->methods != NULL)
	{
		for(i=0;i< class_struct->methods_count;i++)
		{
			for(j=0;j< class_struct->methods[i].attributes_count;j++)
			{
				if(class_struct->methods[i].attributes[j].contents != NULL)
					free(class_struct->methods[i].attributes[j].contents);
			
			}
			if(class_struct->methods[i].attributes != NULL)
				free(class_struct->methods[i].attributes);
		}
		free(class_struct->methods);
	}
	
	if(class_struct->fields != NULL)
	{
		for(i=0;i< class_struct->fields_count;i++)
		{
			for(j=0;j<class_struct->fields[i].attributes_count;j++)
			{
				if(class_struct->fields[i].attributes[j].contents != NULL)
					free(class_struct->fields[i].attributes[j].contents);
			}
			if(class_struct->fields[i].attributes != NULL)
				free(class_struct->fields[i].attributes);
		}	
		
		free(class_struct->fields);
	}
	if(class_struct->attributes != NULL)
	{
		for(i=0;i< class_struct->attributes_count;i++)
		{
			if(class_struct->attributes[i].contents != NULL)
				free(class_struct->attributes[i].contents);
		}
		
		free(class_struct->attributes);
		
	}
	
	free(class_struct);
}

/**
* jclass_class_get_vm_spec
* @javaclass: The class to get the VM spec for.
*
* Gives the minimum VM spec needed to run this class.
* The function returns a pointer to a constant string.
* Do not free it!
*
* Returns: A statically allocated string.
*/
const char* jclass_class_get_vm_spec(const JavaClass* class_struct)
{
	char* vm_spec;
	static char* spec_string[] = { "1.1", "1.2", "1.3", "1.4", "1.5", "6.0", "7.0"};
	
	if(class_struct == NULL)
		return NULL;

	if (class_struct->major_version < 45) {
		return "ancient";
	}

	if (class_struct->major_version - 45 >= sizeof(spec_string)) {
		return "unknown";
	}


	return spec_string[class_struct->major_version - 45];
}

/**
* jclass_class_get_class_name
* @javaclass: The JavaClass that we want the class name for.
*
* Gives the fully qualified name of the class.
*
* Returns: A string allocated with malloc.
*/
char* jclass_class_get_class_name(const JavaClass* class_struct)
{
	char* class_name;
	
	if (class_struct == NULL)
		return NULL;
	
	if(class_struct->constant_pool == NULL)
		return NULL;
		
	class_name = jclass_cp_get_class_name(class_struct->constant_pool, class_struct->constant_pool->this_class, 0);
	
	return class_name;
}

/**
* jclass_class_get_super_class_name
* @javaclass: The class that we want the super class name for.
*
* Gives the fully qualified name of the super class
* for the given class.
*
* Returns: A string allocated with malloc.
*/
char* jclass_class_get_super_class_name(const JavaClass* class_struct)
{
	char* class_name;
	
	if (class_struct == NULL)
		return NULL;
	
	if(class_struct->constant_pool == NULL)
		return NULL;
		
	class_name = jclass_cp_get_class_name(class_struct->constant_pool, class_struct->constant_pool->super_class, 0);
	
	return class_name;
}

/**
* jclass_class_get_sourcefile_name
* @javaclass: The class.
*
* Gives the name of the source file used to compile this class.
* If the class does not have a SourceFile attribute it returns NULL.
*
* Returns: A string allocated with malloc.
*/
char* jclass_class_get_sourcefile_name(const JavaClass* class_struct)
{
	AttributeContainer* attribute;
	ConstantPool* cpool;
	char* filename;
	uint16_t i;
	
	if (class_struct == NULL)
		return NULL;
	
	filename = NULL;
	
	cpool = class_struct->constant_pool;
	attribute = class_struct->attributes;
	for(i = 0; i < class_struct->attributes_count; i++)
	{
		if(jclass_attribute_container_has_attribute(&(attribute[i]), "SourceFile", cpool))
		{
			SourceFileAttribute* sourcefile = jclass_sourcefile_attribute_new(&attribute[i]);
			filename = jclass_cp_get_constant_value(cpool, sourcefile->filename_index, INT_IS_INT);
			jclass_sourcefile_attribute_free(sourcefile);
			break;
		}
	}
	
	return filename;
}

/**
* jclass_class_get_interfaces
* @class_struct: The class to get its interfaces.
*
* Gives a null terminated array with the names of all interfaces implemented
* by the given class. If the class does not implement anything it returns
* NULL.
*
* @Since: 0.4
*
* Returns: A string allocated with malloc.
*/
char **jclass_class_get_interfaces(const JavaClass* class_struct)
{
	char **interface_name;
	uint16_t count, no_interfaces;
	
	no_interfaces = class_struct->interfaces_count;
	if (no_interfaces == 0)
		return NULL;

	interface_name = (char**) malloc(sizeof(char*) * (no_interfaces+1));
	
	for(count = 0; count < no_interfaces; count++)
	{
		interface_name[count] = jclass_cp_get_class_name(class_struct->constant_pool,
											class_struct->interfaces[count], 0);
	}
	interface_name[count] = NULL;
	return interface_name;
}

/**
* jclass_class_get_package_name
* @javaclass: The class to get its package name.
*
* Gives the name of the package this class is part of.
* If the class is not in a package it returns NULL.
*
* Returns: A string allocated with malloc.
*/
char* jclass_class_get_package_name(const JavaClass* javaclass)
{
	char* class_name;
	char* package_name;
	
	if (javaclass == NULL)
		return NULL;
	
	class_name = jclass_class_get_class_name(javaclass);
	package_name = jclass_get_package_from_class_name(class_name);
	free(class_name);
	
	return package_name;
}
