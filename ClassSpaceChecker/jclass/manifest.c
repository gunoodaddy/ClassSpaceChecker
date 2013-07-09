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
 * $Id: manifest.c 523 2004-10-20 15:15:42Z anarxia $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <jclass/manifest.h>

/**
* jclass_manifest_new_from_buffer
* @buf: A memory buffer containing a manifest file.
* @length: The length of the buffer in bytes.
* If the buffer is NULL terminated set to 0.
*
* Creates a Manifest struct representing the given memory buffer. If the length
* given is 0 it is assumed that the buffer is NULL terminated. 
*
* Returns: A Manifest struct or NULL if something went wrong.
*/
Manifest *jclass_manifest_new_from_buffer(const char* orig_buf, uint32_t buf_len) {
	Manifest *manifest;
	ManifestEntry *entry;
	const char *p, *p1;
	char *buf;
	int entry_count;
	int section_count;
	int token_length;
	int i;

	if (!orig_buf)
		return NULL;

	/* Copy the buffer and add a terminating null */
	if (buf_len > 0) {
		buf = (char*) malloc(buf_len + 1);
		buf = (char*) memcpy(buf, orig_buf, buf_len);
		buf[buf_len] = '\0';
	} else {
		buf = (char*)orig_buf;
	}

	manifest = (Manifest*) malloc(sizeof(Manifest));
	manifest->section_count = 1;

	/* count sections */
	for(p = buf; *p ; p++) {
		while (*p && *p == ' ')
			p++;

		if (*p == '\0')
			break;

		if (!strncmp("Name:", p, 5)) {
			manifest->section_count++;
		}
		while (*p && *p != '\n')
			p++;
	}

	manifest->sections = (ManifestSection*) malloc(sizeof(ManifestSection) * manifest->section_count);
	manifest->sections[0].name = NULL;
	manifest->sections[0].entry_count = 0;
	section_count = 0;

	/* count entries per section and get section names */
	for(p = buf; *p; p++) {
		while(*p && (*p == '\r' || *p == '\n' || *p == ' '))
			p++;

		if (*p == '\0')
			break;

		if (!strncmp("Name:", p, 5)) {
			section_count++;
			manifest->sections[section_count].entry_count = 0;
			p += 5;
			while (*p == ' ')
				p++;
			if (*p == ' ')
				p++;

			p1 = p;
			token_length = 0;
			while (*p1 && *p1 != '\n' && *p1 != '\r') {
				p1++;
				token_length++;
			}
			manifest->sections[section_count].entry_count = 0;
			manifest->sections[section_count].name = (char*) malloc(token_length + 1);
			manifest->sections[section_count].name =
				strncpy(manifest->sections[section_count].name, p, token_length);
			manifest->sections[section_count].name[token_length] = '\0';
			p = p1;
		}

		while (*p && *p != '\n' && *p != ':')
			p++;

		if (*p == ':')
			manifest->sections[section_count].entry_count++;

		while (*p && *p != '\n')
			p++;
	}

	/* Allocate memory for entries */
	for (i = 0; i < manifest->section_count; i++) {
		entry_count = manifest->sections[i].entry_count;
		if (!(entry_count))
			manifest->sections[i].entries = NULL;
		else {
			manifest->sections[i].entries = (ManifestEntry*) malloc(sizeof(ManifestEntry) * entry_count);
		}
	}

	section_count = 0;
	entry_count = 0;

	/* Read entries */
	for(p = buf; *p; p++) {
		while(*p && (*p == '\r' || *p == '\n' || *p == ' '))
			p++;

		if (*p == '\0')
			break;

		if (!strncmp("Name:", p, 5)) {
			section_count++;
			entry_count = 0;

			while (*p && *p != '\n')
				p++;

			if (*p == '\0')
				break;

			if (*p == '\n')
				p++;
		}

		/* Key */
		token_length = 0;
		for (p1 = p; *p1 && *p != '\n' && *p1 != ':'; p1++) {
			token_length++;
		}
		if (token_length && *p1 == ':') {
			/* Check if we have more entries than we counted before.
			* This happens on manifests with multi-line entries.
			*/
			if (entry_count >= manifest->sections[section_count].entry_count) {
				continue;
			}
			entry = &manifest->sections[section_count].entries[entry_count];
			entry->key = (char*) malloc(token_length + 1);
			entry->key = strncpy(entry->key, p, token_length);
			entry->key[token_length] = '\0';

			/* Skip leading spaces */
			for (p = p1 + 1; *p == ' '; p++);
			
			/* Value */
			token_length = 0;
			for (p1 = p; *p1 && *p1 != '\r' && *p1 != '\n'; p1++) {
				token_length++;
			}
			if (token_length) {
				entry->value = (char*) malloc(token_length + 1);
				entry->value = strncpy(entry->value, p, token_length);
				entry->value[token_length] = '\0';
			} else {
				entry->value = NULL;
			}
			p = p1;

			entry_count++;
			while (*p && *p != '\n')
				p++;
		}
	}

	if (buf_len > 0)
		free(buf);

	return manifest;
}

static const ManifestSection *get_section(const Manifest *manifest, const char *section) {
	int i;
	if (!manifest)
		return NULL;

	for (i = 0; i < manifest->section_count; i++) {
		if (!section && !manifest->sections[i].name)
			return &manifest->sections[i];
		if (manifest->sections[i].name && strcmp(manifest->sections[i].name, section))
			return &manifest->sections[i];
	}
	return NULL;
}

/**
* jclass_manifest_get_entry
* @manifest: The manifest to get the entry from.
* @section_name: The name of the section containing the entry or NULL for the main section.
* @key: The name of the entry.
*
* Gets the value of the given entry from a manifest.
*
* Returns: A NULL terminated string you should not modify.
*/
const char *jclass_manifest_get_entry(const Manifest *manifest, const char *section_name, const char *key) {
	const ManifestSection *section;
	int i;
	
	section = get_section(manifest, section_name);
	if (!section)
		return NULL;

	for (i = 0; i < section->entry_count; i++) {
		if (!strcmp(section->entries[i].key, key))
			return section->entries[i].value;
	}
	return NULL;
}

/**
* jclass_manifest_free
* @manifest: The manifest to free.
*
* Frees the given manifest structure.
*/
void jclass_manifest_free(Manifest *manifest) {
	int i, j;
	ManifestEntry *entry;

	if (!manifest)
		return;

	for (i = 0; i < manifest->section_count; i++) {
		if (manifest->sections[i].name)
			free(manifest->sections[i].name);

		if (manifest->sections[i].entries) {
			for (j = 0; j < manifest->sections[i].entry_count; j++) {
				entry = &manifest->sections[i].entries[j];
				free(entry->key);
				if (entry->value)
					free(entry->value);
			}
			free(manifest->sections[i].entries);
		}
	}
	free(manifest->sections);
	free(manifest);
}
