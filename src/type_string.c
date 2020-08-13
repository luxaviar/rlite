#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>
#include "rlite/rlite.h"
#include "rlite/page_multi_string.h"
#include "rlite/type_string.h"
#include "rlite/util.h"
#include "rlite/hyperloglog.h"

static int rl_string_get_objects(rlite *db, const unsigned char *key, int64_t keylen, int64_t *_page_number, uint64_t *expires, int64_t *version)
{
	int64_t page_number;
	int retval;
	unsigned char type;
	retval = rl_key_get(db, key, keylen, &type, NULL, &page_number, expires, version);
	if (retval != RL_FOUND) {
		goto cleanup;
	}
	if (type != RL_TYPE_STRING) {
		retval = RL_WRONG_TYPE;
		goto cleanup;
	}
	if (_page_number) {
		*_page_number = page_number;
	}
	retval = RL_OK;
cleanup:
	return retval;
}

int rl_set(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *value, int64_t valuelen, int nx, uint64_t expires)
{
	int retval;
	int64_t page_number;
	unsigned char type;
	int64_t value_page, version;
	retval = rl_key_get(db, key, keylen, &type, NULL, &value_page, NULL, &version);
	if (retval == RL_FOUND) {
		if (nx) {
			goto cleanup;
		}
		else {
			RL_CALL(rl_key_delete_with_value, RL_OK, db, key, keylen);
		}
	} else {
		version = rand();
	}
	RL_CALL(rl_multi_string_set, RL_OK, db, &page_number, value, valuelen);
	RL_CALL(rl_key_set, RL_OK, db, key, keylen, RL_TYPE_STRING, page_number, expires, version + 1);
	retval = RL_OK;
cleanup:
	return retval;
}

int rl_get(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char **value, int64_t *valuelen)
{
	int64_t page_number;
	int retval;
	RL_CALL(rl_string_get_objects, RL_OK, db, key, keylen, &page_number, NULL, NULL);
	if (valuelen) {
		RL_CALL(rl_multi_string_get, RL_OK, db, page_number, value, valuelen);
	}
	retval = RL_OK;
cleanup:
	return retval;
}

int rl_get_cpy(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *value, int64_t *valuelen)
{
	int64_t page_number;
	int retval;
	RL_CALL(rl_string_get_objects, RL_OK, db, key, keylen, &page_number, NULL, NULL);
	if (value || valuelen) {
		RL_CALL(rl_multi_string_cpy, RL_OK, db, page_number, value, valuelen);
	}
	retval = RL_OK;
cleanup:
	return retval;
}

int rl_append(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *value, int64_t valuelen, int64_t *newlength)
{
	int retval;
	int64_t page_number;
	int64_t version;
	RL_CALL2(rl_string_get_objects, RL_OK, RL_NOT_FOUND, db, key, keylen, &page_number, NULL, &version);
	if (retval == RL_NOT_FOUND) {
		RL_CALL(rl_multi_string_set, RL_OK, db, &page_number, value, valuelen);
		version = rand();
		RL_CALL(rl_key_set, RL_OK, db, key, keylen, RL_TYPE_STRING, page_number, 0, version);
		if (newlength) {
			*newlength = valuelen;
		}
	}
	else {
		RL_CALL(rl_key_set, RL_OK, db, key, keylen, RL_TYPE_STRING, page_number, 0, version + 1);
		RL_CALL(rl_multi_string_append, RL_OK, db, page_number, value, valuelen, newlength);
	}
	retval = RL_OK;
cleanup:
	return retval;
}

int rl_getrange(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t start, int64_t stop, unsigned char **value, int64_t *valuelen)
{
	int64_t page_number;
	int retval;
	RL_CALL(rl_string_get_objects, RL_OK, db, key, keylen, &page_number, NULL, NULL);
	RL_CALL(rl_multi_string_getrange, RL_OK, db, page_number, value, valuelen, start, stop);
	retval = RL_OK;
cleanup:
	return retval;
}

int rl_setrange(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t index, unsigned char *value, int64_t valuelen, int64_t *newlength)
{
	int64_t page_number;
	int64_t version;
	uint64_t expires;
	int retval;
	if (valuelen + index > 512*1024*1024) {
		retval = RL_INVALID_PARAMETERS;
		goto cleanup;
	}
	RL_CALL2(rl_string_get_objects, RL_OK, RL_NOT_FOUND, db, key, keylen, &page_number, &expires, &version);
	if (retval == RL_NOT_FOUND) {
		unsigned char *padding;
		RL_MALLOC(padding, sizeof(unsigned char) * index);
		memset(padding, 0, index);
		RL_CALL(rl_set, RL_OK, db, key, keylen, padding, index, 0, 0);
		rl_free(padding);
		RL_CALL(rl_append, RL_OK, db, key, keylen, value, valuelen, newlength);
	}
	else if (retval == RL_OK) {
		RL_CALL(rl_key_set, RL_OK, db, key, keylen, RL_TYPE_STRING, page_number, expires, version + 1);
		RL_CALL(rl_multi_string_setrange, RL_OK, db, page_number, value, valuelen, index, newlength);
	}
	retval = RL_OK;
cleanup:
	return retval;
}

int rl_incr(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t increment, int64_t *newvalue)
{
	int64_t page_number;
	int retval;
	unsigned char *value = NULL;
	char *end;
	int64_t valuelen;
	int64_t lvalue;
	uint64_t expires;
	RL_CALL2(rl_string_get_objects, RL_OK, RL_NOT_FOUND, db, key, keylen, &page_number, &expires, NULL);
	if (retval == RL_NOT_FOUND) {
		RL_MALLOC(value, sizeof(unsigned char) * MAX_LLONG_DIGITS);
		valuelen = snprintf((char *)value, MAX_LLONG_DIGITS, "%" PRId64 "", increment);
		if (newvalue) {
			*newvalue = increment;
		}
		retval = rl_set(db, key, keylen, value, valuelen, 1, 0);
		goto cleanup;
	}
	RL_CALL(rl_multi_string_getrange, RL_OK, db, page_number, &value, &valuelen, 0, MAX_LLONG_DIGITS + 1);
	if (valuelen == MAX_LLONG_DIGITS + 1) {
		retval = RL_NAN;
		goto cleanup;
	}
	lvalue = strtoll((char *)value, &end, 10);
	if (isspace(((char *)value)[0]) || end[0] != '\0' || errno == ERANGE) {
		retval = RL_NAN;
		goto cleanup;
	}
	rl_free(value);
	value = NULL;
	if ((increment < 0 && lvalue < 0 && increment < (LLONG_MIN - lvalue)) ||
	        (increment > 0 && lvalue > 0 && increment > (LLONG_MAX - lvalue))) {
		retval = RL_OVERFLOW;
		goto cleanup;
	}
	lvalue += increment;
	if (newvalue) {
		*newvalue = lvalue;
	}
	RL_MALLOC(value, sizeof(unsigned char) * MAX_LLONG_DIGITS);
	valuelen = snprintf((char *)value, MAX_LLONG_DIGITS, "%" PRId64 "", lvalue);
	RL_CALL(rl_set, RL_OK, db, key, keylen, value, valuelen, 0, expires);
	retval = RL_OK;
cleanup:
	rl_free(value);
	return retval;
}

int rl_incrbyfloat(struct rlite *db, const unsigned char *key, int64_t keylen, double increment, double *newvalue)
{
	int64_t page_number;
	int retval;
	unsigned char *value = NULL;
	char *end;
	int64_t valuelen;
	double dvalue;
	uint64_t expires;
	RL_CALL2(rl_string_get_objects, RL_OK, RL_NOT_FOUND, db, key, keylen, &page_number, &expires, NULL);
	if (retval == RL_NOT_FOUND) {
		RL_MALLOC(value, sizeof(unsigned char) * MAX_DOUBLE_DIGITS);
		valuelen = snprintf((char *)value, MAX_DOUBLE_DIGITS, "%lf", increment);
		if (newvalue) {
			*newvalue = increment;
		}
		retval = rl_set(db, key, keylen, value, valuelen, 1, 0);
		goto cleanup;
	}
	RL_CALL(rl_multi_string_getrange, RL_OK, db, page_number, &value, &valuelen, 0, MAX_DOUBLE_DIGITS + 1);
	if (valuelen == MAX_DOUBLE_DIGITS + 1) {
		retval = RL_NAN;
		goto cleanup;
	}
	dvalue = strtold((char *)value, &end);
	if (isspace(((char *)value)[0]) || end[0] != '\0' || errno == ERANGE || isnan(dvalue)) {
		retval = RL_NAN;
		goto cleanup;
	}
	rl_free(value);
	value = NULL;
	dvalue += increment;
	if (isinf(dvalue) || isnan(dvalue)) {
		retval = RL_OVERFLOW;
		goto cleanup;
	}
	if (newvalue) {
		*newvalue = dvalue;
	}
	RL_MALLOC(value, sizeof(unsigned char) * MAX_DOUBLE_DIGITS);
	valuelen = snprintf((char *)value, MAX_DOUBLE_DIGITS, "%lf", dvalue);
	RL_CALL(rl_set, RL_OK, db, key, keylen, value, valuelen, 0, expires);
	retval = RL_OK;
cleanup:
	rl_free(value);
	return retval;
}

int rl_getbit(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t bitoffset, int *value)
{
	int retval;
	unsigned char *rangevalue = NULL;
	int64_t rangevaluelen, start;
	start = bitoffset >> 3;
	int64_t bit = 7 - (bitoffset & 0x7);
	RL_CALL2(rl_getrange, RL_OK, RL_NOT_FOUND, db, key, keylen, start, start, &rangevalue, &rangevaluelen);
	if (retval == RL_NOT_FOUND || rangevaluelen == 0) {
		*value = 0;
		retval = RL_OK;
		goto cleanup;
	}
	*value = (rangevalue[0] & (1 << bit)) ? 1 : 0;
	retval = RL_OK;
cleanup:
	rl_free(rangevalue);
	return retval;
}

int rl_setbit(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t bitoffset, int on, int *previousvalue)
{
	int retval;
	unsigned char *rangevalue = NULL;
	int64_t rangevaluelen, start;
	char val;
	start = bitoffset >> 3;
	int64_t bit = 7 - (bitoffset & 0x7);

	/* Limit offset to 512MB in bytes */
	if ((bitoffset < 0) || ((uint64_t)bitoffset >> 3) >= (512*1024*1024))
	{
		retval = RL_INVALID_PARAMETERS;
		goto cleanup;
	}

	RL_CALL2(rl_getrange, RL_OK, RL_NOT_FOUND, db, key, keylen, start, start, &rangevalue, &rangevaluelen);
	if (retval == RL_NOT_FOUND || rangevaluelen == 0) {
		val = 0;
		val &= ~(1 << bit);
		val |= ((on & 0x1) << bit);
		retval = rl_setrange(db, key, keylen, start, (unsigned char *)&val, 1, NULL);
		if (previousvalue) {
			*previousvalue = 0;
		}
		goto cleanup;
	}

	int bitval = rangevalue[0] & (1 << bit);
	if (previousvalue) {
		*previousvalue = bitval ? 1 : 0;
	}

	rangevalue[0] &= ~(1 << bit);
	rangevalue[0] |= ((on & 0x1) << bit);
	retval = rl_setrange(db, key, keylen, start, rangevalue, 1, NULL);
cleanup:
	rl_free(rangevalue);
	return retval;
}

int rl_bitop(struct rlite *db, int op, const unsigned char *dest, int64_t destlen, uint64_t keyc, const unsigned char **keys, int64_t *keyslen)
{
	int64_t ltmp;
	int retval;
	unsigned char **values = NULL, *result;
	uint64_t *valueslen = NULL, i;
	int64_t resultlen;
	RL_MALLOC(valueslen, sizeof(uint64_t) * keyc);
	RL_MALLOC(values, sizeof(unsigned char *) * keyc);
	values[0] = NULL;
	for (i = 0; i < keyc; i++) {
		RL_CALL2(rl_get, RL_OK, RL_NOT_FOUND, db, keys[i], keyslen[i], &values[i], &ltmp);
		if (retval == RL_NOT_FOUND) {
			values[i] = NULL;
			valueslen[i] = 0;
		}
		else if (retval == RL_OK) {
			valueslen[i] = ltmp;
		} else {
			goto cleanup;
		}
	}
	rl_internal_bitop(op, keyc, values, valueslen, &result, &resultlen);
	RL_CALL(rl_set, RL_OK, db, dest, destlen, result, resultlen, 0, 0);
	rl_free(result);
	retval = RL_OK;
cleanup:
	if (values) {
		if (i > 0) {
			for (i--; i > 0; i--) {
				rl_free(values[i]);
			}
		}
		rl_free(values[0]);
	}
	rl_free(values);
	rl_free(valueslen);
	return retval;
}

int rl_bitcount(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t start, int64_t stop, int64_t *bitcount)
{
	int retval;
	unsigned char *value;
	int64_t valuelen;
	RL_CALL(rl_getrange, RL_OK, db, key, keylen, start, stop, &value, &valuelen);
	*bitcount = (int64_t)rl_redisPopcount(value, valuelen);
	rl_free(value);
cleanup:
	return retval;
}

int rl_bitpos(struct rlite *db, const unsigned char *key, int64_t keylen, int bit, int64_t start, int64_t stop, int end_given, int64_t *position)
{
	int retval;
	unsigned char *value = NULL;
	int64_t valuelen, totalsize;

	if (bit != 0 && bit != 1) {
		retval = RL_INVALID_PARAMETERS;
		goto cleanup;
	}

	RL_CALL(rl_getrange, RL_OK, db, key, keylen, start, stop, &value, &valuelen);

	if (valuelen == 0) {
		*position = -1;
		retval = RL_OK;
		goto cleanup;
	}

	RL_CALL(rl_get, RL_OK, db, key, keylen, NULL, &totalsize);
	RL_CALL(rl_normalize_string_range, RL_OK, totalsize, &start, &stop);

	int64_t bytes = stop - start + 1;
	// stop may be after the end of the string and in that case it is treated
	// as if it had 0 padding
	//
	// however, it can also be negative! the following check will handle
	// the negative case (which cannot exceed the string size)
	if (bytes < valuelen) {
		bytes = valuelen;
	}
	int64_t pos = rl_internal_bitpos(value, bytes, bit);

	/* If we are looking for clear bits, and the user specified an exact
	 * range with start-end, we can't consider the right of the range as
	 * zero padded (as we do when no explicit end is given).
	 *
	 * So if rl_internal_bitpos() returns the first bit outside the range,
	 * we return -1 to the caller, to mean, in the specified range there
	 * is not a single "0" bit. */
	if (end_given && bit == 0 && pos == bytes * 8) {
		*position = -1;
		retval = RL_OK;
		goto cleanup;
	}

	if (pos != -1) {
		pos += start * 8; /* Adjust for the bytes we skipped. */
	}

	*position = pos;
	retval = RL_OK;
cleanup:
	rl_free(value);
	return retval;
}

int rl_pfadd(struct rlite *db, const unsigned char *key, int64_t keylen, int elementc, unsigned char **elements, int64_t *elementslen, int *updated)
{
	int retval;
	unsigned char *value = NULL;
	int64_t valuelen = 0;
	uint64_t expires = 0;

	RL_CALL2(rl_get, RL_OK, RL_NOT_FOUND, db, key, keylen, &value, &valuelen);
	if (retval == RL_OK) {
		RL_CALL(rl_key_get, RL_FOUND, db, key, keylen, NULL, NULL, NULL, &expires, NULL);
	}
	retval = rl_str_pfadd(value, valuelen, elementc, elements, elementslen, &value, &valuelen);
	if (retval != 0 && retval != 1) {
		if (retval == -1) {
			retval = RL_INVALID_STATE;
		} else {
			retval = RL_UNEXPECTED;
		}
		goto cleanup;
	}
	if (updated) {
		*updated = retval;
	}
	if (retval == 1) { // updated?
		RL_CALL(rl_set, RL_OK, db, key, keylen, value, valuelen, 0, expires)
	}
	retval = RL_OK;
cleanup:
	rl_free(value);
	return retval;
}

int rl_pfcount(struct rlite *db, int keyc, const unsigned char **keys, int64_t *keyslen, int64_t *count)
{
	int retval;
	unsigned char **argv = NULL;
	int64_t *argvlen = NULL;
	int64_t i;
	unsigned char *newvalue = NULL;
	int64_t newvaluelen;
	uint64_t expires = 0;

	RL_MALLOC(argvlen, sizeof(unsigned char *) * keyc);
	RL_MALLOC(argv, sizeof(unsigned char *) * keyc);
	for (i = 0; i < keyc; i++) {
		argv[i] = NULL;
	}
	for (i = 0; i < keyc; i++) {
		RL_CALL2(rl_get, RL_OK, RL_NOT_FOUND, db, keys[i], keyslen[i], &argv[i], &argvlen[i]);
	}

	retval = rl_str_pfcount(keyc, argv, argvlen, count, &newvalue, &newvaluelen);
	if (retval != 0) {
		if (retval == -1) {
			retval = RL_INVALID_STATE;
		} else {
			retval = RL_UNEXPECTED;
		}
		goto cleanup;
	}
	if (newvalue) {
		RL_CALL2(rl_key_get, RL_FOUND, RL_NOT_FOUND, db, keys[0], keyslen[0], NULL, NULL, NULL, &expires, NULL);
		RL_CALL(rl_set, RL_OK, db, keys[0], keyslen[0], newvalue, newvaluelen, 0, expires);
	}
	retval = RL_OK;
cleanup:
	for (i = 0; i < keyc; i++) {
		rl_free(argv[i]);
	}
	rl_free(argv);
	rl_free(argvlen);
	return retval;
}

int rl_pfmerge(struct rlite *db, const unsigned char *destkey, int64_t destkeylen, int keyc, const unsigned char **keys, int64_t *keyslen)
{
	int retval;
	unsigned char **argv = NULL, *newvalue = NULL;
	int64_t *argvlen = NULL, newvaluelen;
	int64_t i;
	int argc = keyc + 1;
	uint64_t expires = 0;

	RL_MALLOC(argvlen, sizeof(unsigned char *) * argc);
	RL_MALLOC(argv, sizeof(unsigned char *) * argc);
	for (i = 0; i < argc; i++) {
		argv[i] = NULL;
	}
	for (i = 0; i < keyc; i++) {
		RL_CALL2(rl_get, RL_OK, RL_NOT_FOUND, db, keys[i], keyslen[i], &argv[i], &argvlen[i]);
	}
	RL_CALL2(rl_get, RL_OK, RL_NOT_FOUND, db, destkey, destkeylen, &argv[keyc], &argvlen[keyc]);
	retval = rl_str_pfmerge(argc, argv, argvlen, &newvalue, &newvaluelen);
	if (retval != 0) {
		if (retval == -1) {
			retval = RL_INVALID_STATE;
		} else {
			retval = RL_UNEXPECTED;
		}
		goto cleanup;
	}

	RL_CALL2(rl_key_get, RL_FOUND, RL_NOT_FOUND, db, destkey, destkeylen, NULL, NULL, NULL, &expires, NULL);
	RL_CALL(rl_set, RL_OK, db, destkey, destkeylen, newvalue, newvaluelen, 0, expires);
	retval = RL_OK;
cleanup:
	for (i = 0; i < argc; i++) {
		rl_free(argv[i]);
	}
	rl_free(argv);
	rl_free(argvlen);
	rl_free(newvalue);
	return retval;
}

int rl_pfdebug_getreg(struct rlite *db, const unsigned char *key, int64_t keylen, int *size, int64_t **elements)
{
	int retval;
	unsigned char *value = NULL;
	int64_t valuelen = 0;
	RL_CALL(rl_get, RL_OK, db, key, keylen, &value, &valuelen);
	retval = rl_str_pfdebug_getreg(value, valuelen, size, elements, &value, &valuelen);
	if (retval != 0) {
		if (retval == -1) {
			retval = RL_INVALID_STATE;
		} else if (retval == -3) {
			retval = RL_OUT_OF_MEMORY;
		} else {
			retval = RL_UNEXPECTED;
		}
		goto cleanup;
	}
	retval = RL_OK;
cleanup:
	rl_free(value);
	return retval;
}

int rl_pfdebug_decode(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char **value, int64_t *valuelen)
{
	int retval;
	unsigned char *str = NULL;
	int64_t strlen;
	RL_CALL(rl_get, RL_OK, db, key, keylen, &str, &strlen);
	retval = rl_str_pfdebug_decode(str, strlen, value, valuelen);
	if (retval != 0) {
		if (retval == -1 || retval == -2) {
			retval = RL_INVALID_STATE;
		} else {
			retval = RL_UNEXPECTED;
		}
		goto cleanup;
	}
	retval = RL_OK;
cleanup:
	rl_free(str);
	return retval;
}

int rl_pfdebug_encoding(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char **value, int64_t *valuelen)
{
	int retval;
	unsigned char *str = NULL;
	int64_t strlen;
	RL_CALL(rl_get, RL_OK, db, key, keylen, &str, &strlen);
	retval = rl_str_pfdebug_encoding(str, strlen, value, valuelen);
	if (retval != 0) {
		if (retval == -1) {
			retval = RL_INVALID_STATE;
		} else {
			retval = RL_UNEXPECTED;
		}
		goto cleanup;
	}
	retval = RL_OK;
cleanup:
	rl_free(str);
	return retval;
}

int rl_pfdebug_todense(struct rlite *db, const unsigned char *key, int64_t keylen, int *converted)
{
	int retval;
	unsigned char *str = NULL;
	int64_t strlen;
	uint64_t expires = 0;
	RL_CALL(rl_get, RL_OK, db, key, keylen, &str, &strlen);
	retval = rl_str_pfdebug_todense(str, strlen, &str, &strlen);
	if (retval != 0 && retval != 1) {
		if (retval == -1) {
			retval = RL_INVALID_STATE;
		} else {
			retval = RL_UNEXPECTED;
		}
		goto cleanup;
	}
	*converted = retval;
	if (retval == 1) {
		RL_CALL2(rl_key_get, RL_FOUND, RL_NOT_FOUND, db, key, keylen, NULL, NULL, NULL, &expires, NULL);
		RL_CALL(rl_set, RL_OK, db, key, keylen, str, strlen, 0, expires);
	}
	retval = RL_OK;
cleanup:
	rl_free(str);
	return retval;
}

int rl_string_pages(struct rlite *db, int64_t page, short *pages)
{
	return rl_multi_string_pages(db, page, pages);
}

int rl_string_delete(struct rlite *db, int64_t value_page)
{
	return rl_multi_string_delete(db, value_page);
}
