#ifndef _RL_UTIL_H
#define _RL_UTIL_H

#include <float.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "utilfromredis.h"

// http://stackoverflow.com/questions/1701055/what-is-the-maximum-length-in-chars-needed-to-represent-any-double-value
#define MAX_DOUBLE_DIGITS (3 + DBL_MANT_DIG - DBL_MIN_EXP)
#define MAX_LLONG_DIGITS 22

#ifdef RL_DEBUG
int expect_fail();
extern int test_mode;
extern size_t test_mode_counter;
void *rl_malloc(size_t size);
void *rl_realloc(void *ptr, size_t size);
void rl_free(void *ptr);
#else
#define rl_malloc malloc
#define rl_realloc realloc
#define rl_free free
#endif

#define RL_MALLOC(obj, size)\
	obj = rl_malloc(size);\
	if (!obj) {\
		retval = RL_OUT_OF_MEMORY;\
		goto cleanup;\
	}

#define RL_REALLOC(ptr, size)\
	tmp = rl_realloc(ptr, size);\
	if (!tmp) {\
		retval = RL_OUT_OF_MEMORY;\
		goto cleanup;\
	}\
	ptr = tmp;

#define RL_CALL(func, expected, ...)\
	retval = func(__VA_ARGS__);\
	if (expected != retval) goto cleanup;

#define RL_CALL2(func, expected, expected2, ...)\
	retval = func(__VA_ARGS__);\
	if (expected != retval && expected2 != retval) goto cleanup;

#define RL_CALL_EXACT2(func, expected, arg1, arg2)\
	retval = func(arg1, arg2);\
	if (expected != retval) goto cleanup;

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

int get_4bytes(const unsigned char *p);
void put_4bytes(unsigned char *p, int64_t v);
uint64_t get_8bytes(const unsigned char *p);
void put_8bytes(unsigned char *p, uint64_t v);
int long_cmp(void *v1, void *v2);
int sha1_cmp(void *v1, void *v2);
int double_cmp(void *v1, void *v2);
#ifdef RL_DEBUG
int long_formatter(void *v2, char **formatted, int *size);
int double_formatter(void *v2, char **formatted, int *size);
#endif
int sha1_formatter(void *v2, char **formatted, int *size);
double get_double(const unsigned char *p);
void put_double(unsigned char *p, double v);
int sha1(const unsigned char *data, int64_t datalen, unsigned char digest[20]);
uint64_t rl_mstime();
double rl_strtod(unsigned char *str, int64_t strlen, unsigned char **eptr);
char *rl_get_filename_with_suffix(const char *filename, char *suffix);

#endif
