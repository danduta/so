/**
 * SO, 2017
 * Lab #2, Operatii I/O simple
 *
 * Useful structures/macros
 */

#ifndef LIN_UTILS_H_
#define LIN_UTILS_H_ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#ifndef DEBUG
#define DEBUG 0
#endif

/* useful macro for handling error codes */
#define DIE(assertion, call_description)  \
	do                                    \
	{                                     \
		if (assertion)                    \
		{                                 \
			fprintf(stderr, "(%s, %d): ", \
					__FILE__, __LINE__);  \
			perror(call_description);     \
			exit(EXIT_FAILURE);           \
		}                                 \
	} while (0)

#define BUFFER_LIMIT 256

int compare_strings(void *, void *);
unsigned long long hash_string(void *);

#define TRACE(x)          \
	do                    \
	{                     \
		if (DEBUG)        \
			dbg_printf x; \
	} while (0)

void dbg_printf(const char *fmt, ...);

#endif
