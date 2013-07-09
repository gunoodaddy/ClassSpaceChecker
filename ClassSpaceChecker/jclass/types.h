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
 * $Id: types.h 526 2005-04-04 00:34:49Z anarxia $
 */

#ifndef __JCLASS_TYPES_H__
#define __JCLASS_TYPES_H__

#ifdef _cplusplus
  extern "C" {
#endif 

#include <jclass/jclassconfig.h>
#include JCLASS_INTTYPES_H

/* Byte ordering macros */

#ifdef JCLASS_BIGENDIAN
#define UINT16_NATIVE(val)  ((uint16_t) (val))
#define INT16_NATIVE(val)  ((int16_t) (val))
#define UINT32_NATIVE(val)  ((uint32_t) (val))
#define INT32_NATIVE(val)  ((int32_t) (val))

#else

#define UINT16_NATIVE(val)  ((uint16_t) ( \
    (uint16_t) ((uint16_t) (val) >> 8) |  \
    (uint16_t) ((uint16_t) (val) << 8)))

#define INT16_NATIVE(val)  ((int16_t) ( \
    (uint16_t) ((uint16_t) (val) >> 8) |  \
    (uint16_t) ((uint16_t) (val) << 8)))

#define UINT32_NATIVE(val) ((uint32_t) ( \
    (((uint32_t) (val) & (uint32_t) 0x000000ffU) << 24) | \
    (((uint32_t) (val) & (uint32_t) 0x0000ff00U) <<  8) | \
    (((uint32_t) (val) & (uint32_t) 0x00ff0000U) >>  8) | \
    (((uint32_t) (val) & (uint32_t) 0xff000000U) >> 24)))

#define INT32_NATIVE(val) ((int32_t) ( \
    (((uint32_t) (val) & (uint32_t) 0x000000ffU) << 24) | \
    (((uint32_t) (val) & (uint32_t) 0x0000ff00U) <<  8) | \
    (((uint32_t) (val) & (uint32_t) 0x00ff0000U) >>  8) | \
    (((uint32_t) (val) & (uint32_t) 0xff000000U) >> 24)))

#endif /* JCLASS_BIGENDIAN */

#ifdef _cplusplus
  }
#endif
	  
#endif
