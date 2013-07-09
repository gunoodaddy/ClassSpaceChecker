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
 * $Id: strtok_r.h 523 2004-10-20 15:15:42Z anarxia $
*/

#ifndef __JCLASS_STRTOK_R_H__
#define __JCLASS_STRTOK_R_H__

#if HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifndef HAVE_STRTOK_R
char * strtok_r(char *str, const char *sep, char **lasts);
#endif

#endif
