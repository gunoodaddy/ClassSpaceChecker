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
 * $Id: class_loader.c 523 2004-10-20 15:15:42Z anarxia $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* For access */
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#else
#  ifdef HAVE_WINDOWS_H
#    include <windows.h>
#  else
#    error You need unistd.h or windows.h
#  endif
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <jclass/class_loader.h>
#include <jclass/jstring.h>

#include "strtok_r.h"

#ifdef __WIN32__
#define SYSTEM_PATH_SLASH '\\'
#define BOOTSTRAP_LIB "\\jre\\lib\\rt.jar"
#else
#define SYSTEM_PATH_SLASH '/'
#define BOOTSTRAP_LIB "/jre/lib/rt.jar"
#endif

//static int isin_jar(const char*, const char*);
static ClassFile* _get_class_file(const char*, const ClassPath*);
static char* _get_class_filename(const char*, const ClassPath*);
static ClassPath * _get_classpath(const char*, const char*);

static ClassLoader _jclass_classloader_default = { _get_class_filename, _get_class_file, _get_classpath };
static ClassLoader _jclass_classloader_current = { _get_class_filename, _get_class_file, _get_classpath };

/**
* jclass_classloader_get_current
*
* Gives the current class loader.
*
* @Since: 0.3
*
* Returns: A pointer to the current class loader.
*/
ClassLoader* jclass_classloader_get_current()
{
	return &_jclass_classloader_current;
}

/**
* jclass_classloader_get_default
* 
* Gives the default class loader.
*
* @Since: 0.3
*
* Returns: A pointer to the default classloader.
*/
ClassLoader* jclass_classloader_get_default()
{
	return &_jclass_classloader_default;
}

/**
* jclass_classloader_set
* @classloader: The new classloader or NULL to revert to the default.
*
* Overrides the classloader for the library. If NULL is passed then
* the default classloader will be used.
*
* @Since: 0.3
*/
void jclass_classloader_set(ClassLoader* classloader)
{
	if(classloader == NULL)
	{
		_jclass_classloader_current.get_class_filename = _get_class_filename;
		_jclass_classloader_current.get_class_file = _get_class_file;
		_jclass_classloader_current.get_classpath = _get_classpath;
	}
	else
		_jclass_classloader_current.get_class_filename = classloader->get_class_filename;
		_jclass_classloader_current.get_class_file = classloader->get_class_file;
		_jclass_classloader_current.get_classpath = classloader->get_classpath;
}

/**
* jclass_classloader_get_class_filename
* @class_name: The fully qualified name of the class.
* @classpath: The classpath to use.
*
* Finds the URL that contains the class with the given name.
*
* @Since: 0.3
*
* Returns: A string allocated with malloc.
*
*/
char* jclass_classloader_get_class_filename(const char* class_name, const ClassPath *classpath)
{
	return _jclass_classloader_current.get_class_filename(class_name, classpath);
}

/**
* jclass_classloader_get_class_file
* @class_name: The fully qualified name of the class.
* @classpath: The classpath to use.
*
* Gives the given class in a file or a memory buffer.
*
* @Since: 0.3
* 
* Returns: A ClassFile struct allocated with malloc.
*/
ClassFile* jclass_classloader_get_class_file(const char* class_name,
	const ClassPath *classpath)
{
	return _jclass_classloader_current.get_class_file(class_name, classpath);
}

/**
* jclass_classloader_get_classpath
* @classpath_string: The classpathe.
* @bootclasspath_string: The classpath for bootstrap classes.
*
* Gives the classpath for the given classpath and bootstrap claspath strings.
*
* @Since: 0.4
* 
* Returns: A ClassPath struct.
*/
ClassPath* jclass_classloader_get_classpath(const char* classpath_string, const char* bootclasspath_string)
{
	return _jclass_classloader_current.get_classpath(classpath_string, bootclasspath_string);
}


/* The Default ClassLoader */

/*
* _get_class_filename
* @classname: The name of the class.
* @classpath: The classpath.
* @bootclasspath: The classpath for the core classes.
*
* Gives the name of the file that contains the given class.
*
* Returns: a string allocated with malloc.
*/
static char* _get_class_filename(const char* class_name, 
	const ClassPath *classpath)
{
	const ClassPath *path;
	char* class_filename;
	char* class_unixfilename;
	char* absolute_class_filename = NULL;
	int path_len;
	FILE *file_ptr;

	if((strlen(class_name) > 6) && !strcmp(".class", &class_name[strlen(class_name) - 6]))
	{
		class_filename = strdup(class_name);
		class_unixfilename = strdup(class_name);
	}
	else
	{
		class_filename = jclass_classname_to_filename(class_name, SYSTEM_PATH_SLASH);
		class_unixfilename = jclass_classname_to_filename(class_name, '/');
	}
				
	foreach_in_classpath(path, classpath) {
		path_len = strlen(path->path);
		/* Jar file */
		if ((path_len > 4) && !strcmp(&path->path[path_len - 4], ".jar"))
		{
			//if(isin_jar(path->path, class_unixfilename))
			//{
			//	absolute_class_filename = strdup(path->path);
			//	break;
			//}
		}
		else
		{
			absolute_class_filename = (char*) malloc(path_len + strlen(class_filename) + 2);
			strcpy(absolute_class_filename, path->path);
			absolute_class_filename[path_len] = SYSTEM_PATH_SLASH;
			absolute_class_filename[path_len + 1] = '\0';
			strcat(absolute_class_filename, class_filename);
	
			file_ptr = fopen(absolute_class_filename,"rb");
			if (file_ptr) {
				fclose(file_ptr);
				break;
			}
			free(absolute_class_filename);
			
		}
	}

	return absolute_class_filename;
}

/* Internal function. Do not use.
*
* Returns a _class_file struct for the given class.
* The struct members are a FILE pointer and a char pointer.
*
* If the class was found in a regular file (ie not inside a jar) 
* the FILE pointer returned is for the file opened with mode "rb"
* and the file offset is set at the beginning of the file.
*
* If the class was found in a jar archive the char pointer points to
* a buffer with the contents of the "extracted" class file.
*
* @bootclasspath_string: The bootstrap loader classpath. 
* If NULL then JAVA_HOME/jre/lib/rt.jar is used
* @classpath_string: The classpath. If NULL the environment variable
* CLASSPATH is used.
* @class_name: The fully qualified class name.
*/
static ClassFile* _get_class_file(const char* class_name, const ClassPath *classpath)
{
	char* class_filename;
	char* class_unixfilename;
	char* absolute_class_filename = NULL;
	ClassFile* class_file_info;
	const ClassPath* path;
	char* data;
	int path_len;

	class_file_info = (ClassFile*) malloc(sizeof(ClassFile));
	class_file_info->file_ptr = NULL;
	class_file_info->data = NULL;
	
	class_filename = jclass_classname_to_filename(class_name, SYSTEM_PATH_SLASH);
	class_unixfilename = jclass_classname_to_filename(class_name, '/');
			
	foreach_in_classpath(path, classpath) {
		path_len = strlen(path->path);
		/* Jar file */
		if ((path_len > 4) && !strcmp(&path->path[path_len - 4], ".jar"))
		{
			//JarFile *jar;
			//
			//jar = jclass_jar_open(path->path);
			//if (jar) {
			//	const JarEntry *entry;

			//	entry = jclass_jar_get_entry(jar, class_unixfilename);
			//	if (entry) {
			//		data = jclass_jar_entry_read(jar, entry);
			//		if(data != NULL)
			//		{
			//			class_file_info->data = data;
			//			jclass_jar_close(jar);
			//			break;
			//		}
			//	}
			//	jclass_jar_close(jar);
			//}
		}
		else
		{
			absolute_class_filename = (char*) malloc(path_len + strlen(class_filename) + 2);
			strcpy(absolute_class_filename, path->path);
			absolute_class_filename[path_len] = SYSTEM_PATH_SLASH;
			absolute_class_filename[path_len + 1] = '\0';
			strcat(absolute_class_filename, class_filename);
	
			class_file_info->file_ptr = fopen(absolute_class_filename,"rb");
			free(absolute_class_filename);
			if(class_file_info->file_ptr != NULL)
				break;
		}
	}
	free(class_filename);
	free(class_unixfilename);
	return class_file_info;
}

/**
* isin_jar
* @jar_filename: The filename of the jar file to check.
* @class_filename: The filename of the class to check for.
*
* Looks for the given file in the jar file.
*
* @Returns: 1 if the file is found, 0 otherwise.
*/
//static int isin_jar(const char* jar_filename, const char* class_filename)
//{
//	JarFile* zipptr;
//	int found;	
//	
//	zipptr = jclass_jar_open(jar_filename);
//	
//	if(zipptr == NULL)
//		return 0;
//	
//	found = (jclass_jar_get_entry(zipptr, class_filename) != NULL);
//	jclass_jar_close(zipptr);	
//	return found;
//}

/*
* Returns the absolute path for the jar.
*/
static char *get_jar_path(const char *jar) {
	char *path;
	int last_index;
	int i;

	last_index = -1;
	for (i = 0; jar[i] != '\0'; i++) {
		if (jar[i] == SYSTEM_PATH_SLASH)
			last_index = i;
	}
	if (last_index == -1)
		return NULL;

	path = (char*) malloc(last_index + 1);
	path = strncpy(path, jar, last_index);
	path[last_index] = '\0';
	return path;
}

static ClassPath *__get_classpath(const char* classpath_string, ClassPath *path_list) {
	char* strtok_buf = NULL;
	char* classpath;
	const char* classpath_token;
	
	if(!classpath_string)
		return NULL;

	classpath = strdup(classpath_string);
	classpath_token = strtok_r(classpath, ":;", &strtok_buf);

	while(classpath_token)
	{
		ClassPath *new_node;
		int path_len;
		int found = 0;
		path_len = strlen(classpath_token);

		foreach_in_classpath(new_node, path_list) {
			if (!strcmp(new_node->path, classpath_token)) {
				found = 1;
				break;
			}
		}

		if (found) {
			classpath_token = strtok_r(NULL, ":;", &strtok_buf);
			continue;
		}

		new_node = (ClassPath*) malloc(sizeof(ClassPath));
		new_node->path = strdup(classpath_token);
		new_node->next = NULL;
		if (!path_list)
			path_list = new_node;
		else
			path_list->tail->next = new_node;

		path_list->tail = new_node;
	
		if ((path_len > 4) && (strcmp(&classpath_token[path_len - 4], ".jar") == 0))
		{
			/*JarFile* jar;

			jar = jclass_jar_open(classpath_token);
			if (jar)
			{
				Manifest *manifest;

				manifest = jclass_jar_get_manifest(jar);
				if (manifest)
				{
					const char *additional_path;
					const char* path_token;

					additional_path = jclass_manifest_get_entry(manifest, NULL, "Class-Path");
					if (additional_path) {
						char *path;
						char *jar_path;
						jar_path = get_jar_path(classpath_token);
						path = strdup(additional_path);
						path_token = strtok_r(path, " :;", &strtok_buf);
						while (path_token) {
							char *full_path;
							full_path = (char*) malloc(strlen(jar_path) + strlen(path_token) + 2);
							full_path = strcpy(full_path, jar_path);
							full_path[strlen(jar_path)] = SYSTEM_PATH_SLASH;
							full_path[strlen(jar_path) + 1] = '\0';
							full_path = strcat(full_path, path_token);
							found = 0;
							foreach_in_classpath(new_node, path_list) {
								if (!strcmp(new_node->path, full_path)) {
									found = 1;
									break;
								}
							}
							if (!found)
								__get_classpath(full_path, path_list);

							free(full_path);
							path_token = strtok_r(NULL, " :;", &strtok_buf);
						}
						free(jar_path);
					}

					jclass_manifest_free(manifest);
				}
				jclass_jar_close(jar);
			}*/
		}

		classpath_token = strtok_r(NULL, ":;", &strtok_buf);	
	}
		
	
	free(classpath);
	
	return path_list;
}

static ClassPath *_get_classpath(const char* classpath_string, const char* bootclasspath_string) {
	char *bootclasspath;
	char *real_classpath = NULL;
	const char *classpath;
	ClassPath *return_path;

	if(bootclasspath_string == NULL)
	{
		char *java_home = getenv("JAVA_HOME");
		if(java_home != NULL)
		{
			bootclasspath = (char*) malloc(strlen(java_home) + strlen(BOOTSTRAP_LIB) + 1);
			strcpy(bootclasspath, java_home);
			strcat(bootclasspath, BOOTSTRAP_LIB);
		}
		else
			bootclasspath = NULL;
	}
	else
		bootclasspath = strdup(bootclasspath_string);

	if(classpath_string == NULL)
	{
		if(getenv("CLASSPATH") != NULL)
			classpath = getenv("CLASSPATH");
		else
			classpath = NULL;
	}
	else
		classpath = classpath_string;

	if (bootclasspath && classpath)
	{
		real_classpath = (char*) malloc(strlen(classpath) + strlen(bootclasspath) + 2);
		real_classpath = strcpy(real_classpath, bootclasspath);
		real_classpath[strlen(bootclasspath)] = ':';
		real_classpath[strlen(bootclasspath) + 1] = '\0';
		real_classpath = strcat(real_classpath, classpath);
		free(bootclasspath);
	}
	else if (bootclasspath)
	{
		real_classpath = bootclasspath;
	}
	else if (classpath)
	{
		real_classpath = strdup(classpath);
	}
	
	return_path = __get_classpath(real_classpath, NULL);
	if (real_classpath)
		free(real_classpath);

	return return_path;
}

/**
* jclass_classloader_classpath_free
* @path: The classpath to free.
*
* Frees the given classpath struct.
*/
void jclass_classloader_classpath_free(ClassPath *path)
{
	ClassPath *path_node, *next;
	next = path;
	for (path_node = path; next; path_node = next)
	{
		free(path_node->path);
		next = path_node->next;
		free(path_node);
	}
}
