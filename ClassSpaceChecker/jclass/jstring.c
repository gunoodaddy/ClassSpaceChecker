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
 * $Id: jstring.c 523 2004-10-20 15:15:42Z anarxia $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <jclass/jstring.h>
#include <jclass/class.h>

/**
* jclass_utf8_to_string
* @utf8_string: A pointer to the UTF-8 string.
* @length: The length of the string in bytes.
*
* Converts a java UTF-8 string to its ASCII equivalent.
* Java uses a slightly different format than standard UTF-8.
*
* Returns: A string allocated with malloc.
*/
char* jclass_utf8_to_string(const uint8_t* utf8_string, uint16_t length)
{
	char* internal_string;
	uint16_t count;
	uint8_t character[3];
	uint16_t extra_chars = 0;
	
	internal_string = (char*) malloc(length + 1);
	internal_string[0] = '\0';
	
	for(count = 0; count < length; count++)
	{
		character[0] = utf8_string[count];
		if(character[0] == '\0')
		{
			internal_string[0] = '\0';
			break;
		}
				
		if(character[0] < 0x7f)
			internal_string[count - extra_chars] = character[0];
		else 	/* multi byte */
		{
		    count++;
			extra_chars++;
			character[1] = utf8_string[count];

			if (character[1] < 0x7f)	/* 2 byte */
			{
				internal_string[count - extra_chars] = ((character[0] & 0x1f) << 6) + (character[1] & 0x3f);
			}
			else	/* 3 byte */
			{
				count++;
				extra_chars++;
				character[2] = utf8_string[count];
				internal_string[count - extra_chars] = ((character[0] & 0xf) << 12) + 
														((character[1] & 0x3f) << 6) + 
														(character[2] & 0x3f);
			}
		}
	}
	internal_string[length - extra_chars] = '\0';
	return internal_string;
}

/**
* jclass_get_printable_string
* @raw_string: The string to translate (it is not changed).
*
* Translates a string to a more human readable form.
* All control characters are converted to escape characters.
*
* Returns: A string allocated with malloc.
*/
char* jclass_get_printable_string(const char* raw_string)
{
	char* new_string;
	const char* str_ptr;
	int string_length;
	char* to_ptr;

	if(raw_string == NULL)
		return NULL;

	/* count the length of the string */
	str_ptr = raw_string;
	string_length = 0;
	while(*str_ptr != '\0')
	{
		string_length++;
		if(*str_ptr >= 7 && *str_ptr <= 13)
			string_length++;
		else if(*str_ptr < ' ')
			string_length += 2;

		str_ptr++;
	}

	new_string = (char*) malloc(string_length + 1);

	str_ptr = raw_string;
	to_ptr = new_string;
	while(*str_ptr != '\0')
	{
		if((*str_ptr < ' ') || (*str_ptr == '\\') || (*str_ptr == '\"') )
		{
			*(to_ptr++) = '\\';

			switch(*str_ptr)
			{
				case 7:
					*to_ptr = 'a';
					break;
				case 8:
					*to_ptr = 'b';
					break;
				case 9:
					*to_ptr = 't';
					break;
				case 10:
					*to_ptr = 'n';
					break;
				case 11:
					*to_ptr = 'v';
					break;
				case 12:
					*to_ptr = 'f';
					break;
				case 13:
					*to_ptr = 'r';
					break;
				case '\\':
					*to_ptr = '\\';
					break;
				case '\"':
					*to_ptr = '\"';
					break;
				default:
					*(to_ptr++) = (*str_ptr / 10) + 30;
					*to_ptr = (*str_ptr % 10) + 30;
			}
		}
		else
			*to_ptr = *str_ptr;

		str_ptr++;
		to_ptr++;
	}
	*to_ptr = '\0';

	return new_string;
}

/**
* jclass_float_to_string
* @float_bytes: The float contained in a uint32 integer.
*
* Gives a string representation of a float.
*
* Returns: A string allocated with malloc.
*/
char* jclass_float_to_string(uint32_t float_bytes)
{
	char* number;
	int exponent;
	int mantissa;
	double float_num;

	if(float_bytes == 0x7f800000)
		number = strdup("+infinity");
	else if(float_bytes == 0xff800000)
		number = strdup("-infinity");
	else if((float_bytes >= 0x7f800001 && float_bytes <= 0x7fffffff) ||
					(float_bytes >= 0xff800001 && float_bytes <= 0xffffffff))
		number = strdup("NaN");
	else if(float_bytes == 0x00000000 || float_bytes == 0x80000000)
		number = strdup("0");
	else
	{
		number = (char*) malloc(40);
		number[40] = '\0';

		/* set sign */
		if (float_bytes & 0x80000000)
			number[0] = '-';
		else
			number[0] = ' ';

		exponent = (float_bytes >> 23) & 0xff;
		mantissa = (exponent == 0) ?
    			(float_bytes & 0x7fffff) << 1 :
    			(float_bytes & 0x7fffff) | 0x800000;

		float_num = pow((float)2, (exponent - 150));
		float_num *= mantissa;
		sprintf(&number[1],"%f", float_num);

	}
	return number;
}

/**
* jclass_double_to_string
* @double_bytes: The double contained in a uint64.
*
* Converts a double to a string.
*
* Returns: A string allocated with malloc.
*/
char* jclass_double_to_string(uint64_t double_bytes)
{
	char* number;
	int exponent;
	uint64_t mantissa;
	double double_num;
	
	if(double_bytes == 0x7ff0000000000000LL)
		number = strdup("+infinity");
	else if(double_bytes == 0xfff0000000000000LL)
		number = strdup("-infinity");
	else if((double_bytes >= 0x7ff0000000000001LL && double_bytes <= 0x7fffffffffffffffLL) ||
					(double_bytes >= 0xfff0000000000001LL && double_bytes <= 0xffffffffffffffffLL))
		number = strdup("NaN");
	else if(double_bytes == 0x0000000000000000LL || double_bytes == 0x8000000000000000LL)
		number = strdup("0");
	else
	{
		number = (char*) malloc(80);
		number[40] = '\0';

		/* set sign */
		if ((double_bytes >> 63))
			number[0] = '-';
		else
			number[0] = ' ';

		exponent = (double_bytes >> 52)  & 0x7ffL;
		mantissa = (exponent == 0) ?
    			(double_bytes & 0xfffffffffffffLL) << 1 :
    			(double_bytes & 0xfffffffffffffLL) | 0x10000000000000LL;

		double_num = pow((float)2, (exponent - 1075));
		double_num *= mantissa;

		sprintf(&number[1],"%f", double_num);
	}
	return number;
}

/**
* jclass_descriptor_get_type
* @descriptor: The coded descriptor string.
*
* Gives the type of a coded descriptor.
*
* Returns: A string allocated with malloc containg the type in the descriptor.
*/
char* jclass_descriptor_get_type(const char* descriptor)
{
	char* type;
	int length;
	int param_length;
	int array_length;
	int i;
	int j;
	
	length = strlen(descriptor);
	i = 0;
	/* the descriptor contains parameter information 
	 * Skip it
	*/
	if(descriptor[0] == '(')
	{
		while(descriptor[i] != ')')
			i++;
			
		i++;
	}
	
	param_length = length - i;
	
	array_length = 0;
	while(descriptor[i] == '[')
	{
		i++;	
		array_length++;
	}
	
	if (descriptor[i] == 'L')
	{
		type = (char*) malloc(param_length + 1);
		i++;
		j = 0;
		while(descriptor[i] != ';' && descriptor[i] != '\0')
		{
			if (descriptor[i] == '/')
				type[j] = '.';
			else
				type[j] = descriptor[i];
			
			j++;			
			i++;
		}
		type[j] = '\0';
	}
	else
	{
		switch(descriptor[i])
		{
			case 'B':
				type = strdup("byte");
				break;
			case 'C':
				type = strdup("char");
				break;
			case 'D':
				type = strdup("double");
				break;
			case 'F':
				type = strdup("float");
				break;
			case 'I':
				type = strdup("int");
				break;
			case 'J':
				type = strdup("long");
				break;
			case 'S':
				type = strdup("short");
				break;
			case 'V':
				type = strdup("void");
				break;
			case 'Z':
				type = strdup("boolean");
				break;
			default:
				type = (char*) malloc(2);
				type[0] = descriptor[i];
				type[1] = '\0';
		}
	}
	
	if(array_length)
	{
		type = (char*) realloc(type,((array_length * 2) + strlen(type) + 1));
		for(i = 0; i < array_length; i++)
			strcat(type,"[]");
	}
	return type;
}

/**
* jclass_descriptor_get_parameters_array
* @descriptor: The coded descriptor.
*
* Gives the parameters part of a coded descriptor.
*
* Returns: A NULL terminated string array allocated with malloc containg the parameters
* in the descriptor.
*/
char** jclass_descriptor_get_parameters_array(const char* descriptor)
{
	char** params;
	const char *p;
	int i;
	int token_length;
	int params_count;
	int parsing_class;
	int parsing_array;

	if(descriptor[0] != '(') {
		params = (char**) malloc(1 * sizeof(char*));
		params[0] = NULL;
		return params;
	}
	
	params_count = 0;
	parsing_class = 0;
	parsing_array = 0;

	for (p = &descriptor[1]; *p != '\0' && *p != ')'; p++) {
		/* Array */
		if (*p == '[')
			continue;

		if (!parsing_class) {
			switch (*p) {
				case 'L':	/* class */
					parsing_class = 1;
				case 'B':	/* byte */
				case 'C':	/* char */
				case 'D':	/* double */
				case 'F':	/* float */
				case 'I':	/* int */
				case 'J':	/* long */
				case 'S':	/* short */
				case 'V':	/* void */
				case 'Z':	/* boolean */
					params_count++;
			}
		}
		else if (*p == ';')
			parsing_class = 0;
	}
				
	params = (char**) malloc((params_count + 1) * sizeof(char*));
	params[params_count] = NULL;	
	token_length = 0;
	i = 0;
	p = &descriptor[1];
	for (i = 0; i < params_count; i++) {
		params[i] = jclass_descriptor_get_type(p);
		/* skip arrays */
		while (*p == '[' && *p != '\0')
			p++;

		/* skip classes */
		if (*p == 'L') {
			while (*p != ';' && *p != '\0')
				p++;
			if (*p == ';')
				p++;
		}
		else	/* skip primitives */
			p++;
	}

	return params;
}

/**
* jclass_descriptor_get_parameters
* @descriptor: The coded descriptor.
*
* Deprecated. Use jclass_descriptor_get_parameters_array() instead.
* Gives the parameters part of a coded descriptor.
*
* Returns: A string allocated with malloc containg the parameters 
* in the descriptor.
*/
char* jclass_descriptor_get_parameters(const char* descriptor)
{
	char* params;
	char* internal_params;
	char* curr_param;
	int index;
	char temp_char;
	int token_length;
	int params_length;
	
	if(descriptor[0] != '(')
	{
		params = strdup("");
		return params;
	}
	
	internal_params = strdup(&descriptor[1]);
	
	params_length = 0;
	while(internal_params[params_length] != ')')
		params_length++;
				
	params = strdup("(");
	
	index = 0;
	while(index < params_length)
	{
		token_length = 0;
		
		while(internal_params[index + token_length] == '[')
			token_length++;
			
		if(internal_params[index + token_length] == 'L')
		{
			while(internal_params[index + token_length] != ';')
				token_length++;
		}		
		token_length++;
		
		temp_char = internal_params[index + token_length];
		internal_params[index + token_length] = '\0';
		curr_param = jclass_descriptor_get_type(&(internal_params[index]));
		internal_params[index + token_length] = temp_char;
		
		index += token_length;
	
		params = (char*) realloc(params, (strlen(params) + strlen(curr_param) + 3));
		
		if(params[1] != '\0')
			strcat(params, ", ");
		
		strcat(params, curr_param);
		free(curr_param);
	}
	free(internal_params);
	
	params = (char*) realloc(params, (strlen(params) + 2));
	strcat(params, ")");
	
	return params;
}

/**
* jclass_get_class_from_method_signature
* @method_signature: The method signature.
*
* Gives the name of the class given a method signature for one
* of its methods.
*
* Returns: A string allocated with malloc.
*/
char* jclass_get_class_from_method_signature(const char* method)
{
	char* class_name;
	int start_index;
	int finish_index;
	
	if(method == NULL)
		return NULL;
	
	finish_index = strlen(method) - 1;
	while(method[finish_index] != '(' && finish_index > 0)
		finish_index--;
	
	while(method[finish_index] != '.' && finish_index > 0)
		finish_index--;
		
	start_index = 0;
	while(method[start_index] != '\0' && method[start_index] != ' ' 
													&& method[start_index] != '(')
		start_index++;

	/* special case for constructors */
	if(method[start_index] == '(')
	{
		finish_index = start_index;
		start_index = 0;
	}
	else	
		start_index++;
		
	if ((finish_index - start_index) > 0)
	{
		class_name = (char*) malloc(sizeof(char) * (1 + finish_index - start_index));
		strncpy(class_name, &method[start_index], (finish_index - start_index));
		class_name[finish_index - start_index] = '\0';
	}
	else
		return NULL;
		
	return class_name;
}

/**
* jclass_get_package_from_class_name
* @class_name: The fully qualified class name.
*
* Given a full class name it returns the package the class is a member of.
* If the class is not part of a package it returns NULL.
*
* Returns: A string allocated with malloc.
*/
char* jclass_get_package_from_class_name(const char* classname)
{
	char* package;
	int index;
	int last_index;

	if(classname == NULL)
		return NULL;
	
	index = 0;
	last_index = 0;
	while(classname[index] != '\0' && classname[index] != '(')
	{
		if(classname[index] == '.')
			last_index = index;
		
		index++;
	}
	
	if(last_index)
	{
		package = (char*) malloc(sizeof(char) * last_index + 1);
		strncpy(package,	classname, last_index);
		package[last_index] = '\0';
	}
	else
		package = NULL;
	
	return package;
}

/**
* jclass_string_is_primitive_type
* @type_string: A null terminated string.
*
* Checks whether the given string is the name of a primitive type.
*
* Returns: 1 if the type given is a pritive type, 0 otherwise.
*/
int jclass_string_is_primitive_type(const char* type)
{
	int prim;
	
	prim = (strcmp("void", type) == 0)	||
			(strcmp("byte", type) == 0)	||
			(strcmp("char", type) == 0)	||
			(strcmp("double", type) == 0) ||
			(strcmp("float", type) == 0) ||
			(strcmp("int", type) == 0) ||
			(strcmp("long", type) == 0) ||
			(strcmp("short", type) == 0) ||
			(strcmp("boolean", type) == 0);
	return prim;
}

/**
* jclass_classname_to_filename
* @class_name: The name of the class.
* @path_slash: The slash for your OS ('/' for Unices, '\\' for Win).
*
* Converts the given class name to a filename.
* For example java.lang.String will be converted to: java/lang/String.class.
*
* Returns: A string allocated with malloc with the filename.
*/
char* jclass_classname_to_filename(const char* class_name, char slash)
{
	char* class_filename;
	int i;
	
	class_filename = (char*) malloc(strlen(class_name) + 7);
	strcpy(class_filename, class_name);
	/* replace . with / */	
	for(i = 0; i < strlen(class_filename); i++)
	{
		if(class_filename[i] == '.')
			class_filename[i] = slash;
	}
	strcat(class_filename, ".class");
	
	return class_filename;
}

/* Macro to save some typing in the get_access_flag_string function.
*/
#define ADD_FLAG_TO_STRING(flag) do { \
			if(temp_string[0] != '\0') \
				strcat(temp_string, " ");\
			strcat(temp_string, flag); \
		}while(0)

/**
* jclass_access_flag_to_string
* @access_flag: The access flag.
* @is_class: Set to 1 if the flags are for a class, 0 otherwise.
*
* Gives the string representation of an access flag.
*
* Returns: A string allocated with malloc.
*/
char* jclass_access_flag_to_string(uint16_t flag, int is_class)
{
	char* access_string;
	char temp_string[256];

	temp_string[0] = '\0';
	
	if (flag & ACC_PUBLIC)
		strcat(temp_string, "public");
	else if (flag & ACC_PRIVATE)
		strcat(temp_string, "private");
	else if (flag & ACC_PROTECTED)
		strcat(temp_string, "protected");
	
	if (flag & ACC_STATIC)
		ADD_FLAG_TO_STRING("static");
	
	if (flag & ACC_FINAL)
		ADD_FLAG_TO_STRING("final");
	
	if ((flag & ACC_SYNCHRONIZED) && !is_class)
		ADD_FLAG_TO_STRING("synchronized");
	
	if (flag & ACC_VOLATILE)
		ADD_FLAG_TO_STRING("volatile");
		
	if (flag & ACC_TRANSIENT)
		ADD_FLAG_TO_STRING("transient");
		
	if (flag & ACC_NATIVE)
		ADD_FLAG_TO_STRING("native");
			
	if (flag & ACC_STRICTFP)
		ADD_FLAG_TO_STRING("strictfp");
	
	if (flag & ACC_INTERFACE)
		ADD_FLAG_TO_STRING("interface");
	else if(is_class) {

		if (flag & ACC_ABSTRACT)
			ADD_FLAG_TO_STRING("abstract");

		ADD_FLAG_TO_STRING("class");
	}
	access_string = strdup(temp_string);
	return access_string;
}

#undef ADD_FLAG_TO_STRING
