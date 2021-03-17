/**
 * SO, 2017
 * Lab #2, Operatii I/O simple
 *
 * Useful structures/macros
 */

#ifndef LIN_UTILS_H_
#define LIN_UTILS_H_	1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* useful macro for handling error codes */
#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(EXIT_FAILURE);				\
		}							\
	} while (0)

#define LINE_LIMIT 256

int compare_strings(void*, void*);
u_int64_t hash_string(void*);

#endif
