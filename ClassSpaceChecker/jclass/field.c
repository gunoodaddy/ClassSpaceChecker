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
 * $Id: field.c 523 2004-10-20 15:15:42Z anarxia $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <jclass/class.h>
#include <jclass/jstring.h>

int jclass_field_is_visible(const Field* field, const ConstantPool* constant_pool, JCVisibility visib)
{
	uint16_t i;
	int is_synthetic = 0;
	
	if (visib == V_SYNTHETIC)
	{
		for(i=0; i < field->attributes_count; i++)
		{
			if (jclass_attribute_container_has_attribute(&field->attributes[i],"Synthetic", constant_pool))
			{
				is_synthetic = 1;
				break;
			}
		}
	}
	
	if(
		((visib < V_SYNTHETIC) && is_synthetic) ||
		((visib < V_PRIVATE) && (field->access_flags & ACC_PRIVATE)) ||
		((visib < V_PROTECTED) && (field->access_flags & ACC_PROTECTED)) ||
		((visib == V_PUBLIC) && !(field->access_flags & ACC_PUBLIC))
	  )
	{
		return 0;
	}
	else
		return 1;
}

CodeAttribute* jclass_field_get_code_attribute(const Field* field, const ConstantPool* cpool)
{
	uint16_t i;
	
	if(field == NULL)
		return NULL;
	
	for(i = 0; i < field->attributes_count; i++)
	{
		if(jclass_attribute_container_has_attribute(&field->attributes[i], "Code", cpool))
			return jclass_code_attribute_new(&field->attributes[i]);
	}
	
	return NULL;
}

char* jclass_field_get_name(const Field* field, const ConstantPool* cpool)
{	
	if(field == NULL)
		return NULL;
	
	return jclass_cp_get_constant_value(cpool, field->name_index, INT_IS_INT);
}

char* jclass_field_get_descriptor(const Field* field, const ConstantPool* cpool)
{	
	if(field == NULL)
		return NULL;
	
	return jclass_cp_get_constant_value(cpool, field->descriptor_index, INT_IS_INT);
}
