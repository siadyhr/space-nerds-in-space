/*
	Copyright (C) 2014 Stephen M. Cameron
	Author: Stephen M. Cameron

	This file is part of Spacenerds In Space.

	Spacenerds in Space is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Spacenerds in Space is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Spacenerds in Space; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
	*/

/*
 * Link this in to make all calls to setlocale do setlocale(LC_ALL, "C");
 * regardless of what parameters are passed.
 *
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <locale.h>

typedef char *(*setlocale_prototype)(int category, const char *locale);

static setlocale_prototype real_setlocale = NULL;

static char *the_only_locale = "C";

char *setlocale(__attribute__((unused)) int category,
		__attribute__((unused)) const char *locale)
{
	char *msg;

	/* printf("setlocale intercepted, using locale of 'C'\n"); */
	if (!real_setlocale) {
		/* Any use of dlsym to extract a function pointer rather than a
		 * data pointer is going to be on thin ice (it is actually forbidden by
		 * the standard, since it is not guaranteed that a function pointer will
		 * fit into a void * (think about DOS's NEAR and FAR pointers -- or maybe
		 * on a modern arch, you might have 32 bit data ptrs and 48 bit code ptrs.)
		 * Plugin libraries typically get around this by using dlsym to extract a
		 * pointer to an array of function or similar.  We don't have that luxury
		 * since we're trying to get the ptr to libc's "setlocale" function, and
		 * our only way to get it is via the void * that dlsym returns.
		 *
		 * What we're doing here is probably pretty thin ice anyhow, but
		 * what sscanf does with floats and the locale is arguably idiotic,
		 * and this makes things work consistently.
		 *
		 * We can suppress the warnings from GCC with some pragmas though.
		 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
		real_setlocale = (setlocale_prototype) dlsym(RTLD_NEXT, "setlocale");
#pragma GCC diagnostic pop
		msg = dlerror();
		if (msg) {
			fprintf(stderr, "Failed to override setlocale(): %s\n", msg);
			fflush(stderr);
			real_setlocale = NULL;
			return the_only_locale;
		}
	}
	/* C is the locale, and the locale shall be C */
	return real_setlocale(LC_ALL, "C");
}

