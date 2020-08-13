#ifndef _RL_TYPE_STRING_H
#define _RL_TYPE_STRING_H

#include "page_multi_string.h"

#define RL_TYPE_STRING 'T'

struct rlite;

int rl_set(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *value, int64_t valuelen, int nx, uint64_t expires);
int rl_get(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char **value, int64_t *valuelen);
int rl_get_cpy(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *value, int64_t *valuelen);
int rl_append(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *value, int64_t valuelen, int64_t *newlength);
int rl_getrange(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t start, int64_t stop, unsigned char **value, int64_t *valuelen);
int rl_setrange(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t index, unsigned char *value, int64_t valuelen, int64_t *newlength);
int rl_incr(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t increment, int64_t *newvalue);
int rl_incrbyfloat(struct rlite *db, const unsigned char *key, int64_t keylen, double increment, double *newvalue);
int rl_getbit(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t offset, int *value);
int rl_setbit(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t bitoffset, int on, int *previousvalue);
int rl_bitop(struct rlite *db, int op, const unsigned char *dest, int64_t destlen, uint64_t keylen, const unsigned char **keys, int64_t *keyslen);
int rl_bitcount(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t start, int64_t stop, int64_t *bitcount);
int rl_bitpos(struct rlite *db, const unsigned char *key, int64_t keylen, int bit, int64_t start, int64_t stop, int end_given, int64_t *position);

int rl_pfadd(struct rlite *db, const unsigned char *key, int64_t keylen, int elementc, unsigned char **elements, int64_t *elementslen, int *updated);
int rl_pfcount(struct rlite *db, int keyc, const unsigned char **key, int64_t *keylen, int64_t *count);
int rl_pfmerge(struct rlite *db, const unsigned char *destkey, int64_t destkeylen, int keyc, const unsigned char **keys, int64_t *keyslen);
int rl_pfdebug_getreg(struct rlite *db, const unsigned char *key, int64_t keylen, int *size, int64_t **elements);
int rl_pfdebug_decode(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char **value, int64_t *valuelen);
int rl_pfdebug_encoding(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char **value, int64_t *valuelen);
int rl_pfdebug_todense(struct rlite *db, const unsigned char *key, int64_t keylen, int *converted);

int rl_string_pages(struct rlite *db, int64_t page, short *pages);
int rl_string_delete(struct rlite *db, int64_t value_page);

#endif
