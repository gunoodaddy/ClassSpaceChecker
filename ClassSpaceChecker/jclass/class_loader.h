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
 * $Id: class_loader.h 527 2005-10-01 00:42:22Z anarxia $
 */

#ifndef __JCLASS_CLASS_LOADER_H__
#define __JCLASS_CLASS_LOADER_H__

#include <stdio.h>

#ifdef _cplusplus
  extern "C" {
#endif 

typedef struct {
	FILE* file_ptr;
	char* data;
} ClassFile;

typedef struct ClassPath {
	char *path;
	struct ClassPath *next;
	struct ClassPath *tail;
} ClassPath;

#define foreach_in_classpath(node, path) for(node = path; node; node = node->next)

typedef struct  {
		char* (*get_class_filename) (const char*, const ClassPath*);
		ClassFile* (*get_class_file) (const char*, const ClassPath*);
		ClassPath* (*get_classpath) (const char*, const char*);
} ClassLoader;

ClassLoader* jclass_classloader_get_current(void);
ClassLoader* jclass_classloader_get_default(void);
void jclass_classloader_set(ClassLoader* classloader);

char* jclass_classloader_get_class_filename(const char* class_name, const ClassPath *classpath);

ClassFile* jclass_classloader_get_class_file(const char* class_name, const ClassPath *classpath);

ClassPath* jclass_classloader_get_classpath(const char* classpath_string, const char* bootclasspath_string);
void jclass_classloader_classpath_free(ClassPath *path);

#ifdef _cplusplus
  }
#endif   
#endif /* CLASS_LOADER_H */
