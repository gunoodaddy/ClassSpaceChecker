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
 * $Id: jstring.h 523 2004-10-20 15:15:42Z anarxia $
 */

#ifndef __JCLASS_JSTRING_H__
#define __JCLASS_JSTRING_H__

#ifdef _cplusplus
  extern "C" {
#endif 

#include <jclass/types.h>

char* jclass_utf8_to_string(const uint8_t* utf8_string, uint16_t length);
char* jclass_get_printable_string(const char* raw_string);
char* jclass_float_to_string(uint32_t float_bytes);
char* jclass_double_to_string(uint64_t double_bytes);
char* jclass_descriptor_get_type(const char* descriptor);
char** jclass_descriptor_get_parameters_array(const char* descriptor);
char* jclass_descriptor_get_parameters(const char* descriptor);
char* jclass_access_flag_to_string(uint16_t access_flag, int is_class);
char* jclass_get_package_from_class_name(const char* class_name);
char* jclass_get_class_from_method_signature(const char* method_signature);
char* jclass_classname_to_filename(const char* class_name, char path_slash);
int jclass_string_is_primitive_type(const char* type_string);

#ifdef _cplusplus
  }
#endif 

#endif
