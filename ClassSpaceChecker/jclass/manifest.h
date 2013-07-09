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
 * $Id: manifest.h 527 2005-10-01 00:42:22Z anarxia $
 */
#ifndef __JCLASS_MANIFEST_H__
#define __JCLASS_MANIFEST_H__

#ifdef _cplusplus
 extern "C" {
#endif 

#include <jclass/types.h>

typedef struct {
	char *key;
	char *value;
} ManifestEntry;

typedef struct {
	char *name;
	int entry_count;
	ManifestEntry *entries;
} ManifestSection;

typedef struct {
	int section_count;
	ManifestSection *sections;
} Manifest;

Manifest *jclass_manifest_new_from_buffer(const char* buf, uint32_t length);
void jclass_manifest_free(Manifest *manifest);
const char *jclass_manifest_get_entry(const Manifest *manifest, const char *section_name, const char *key);

#ifdef _cplusplus
 }
#endif 

#endif
