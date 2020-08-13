#ifndef _RL_TYPE_LIST_H
#define _RL_TYPE_LIST_H

#include "page_list.h"

#define RL_TYPE_LIST 'L'

struct rlite;

typedef rl_list_iterator rl_llist_iterator;

int rl_llist_iterator_next(rl_llist_iterator *iterator, int64_t *page, unsigned char **value, int64_t *valuelen);
int rl_llist_iterator_destroy(rl_llist_iterator *iterator);

int rl_push(struct rlite *db, const unsigned char *key, int64_t keylen, int create, int left, int valuec, unsigned char **values, int64_t *valueslen, int64_t *size);
int rl_llen(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t *len);
int rl_pop(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char **value, int64_t *valuelen, int left);
int rl_lindex(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t index, unsigned char **value, int64_t *valuelen);
int rl_linsert(struct rlite *db, const unsigned char *key, int64_t keylen, int after, unsigned char *pivot, int64_t pivotlen, unsigned char *value, int64_t valuelen, int64_t *size);
int rl_lrange_iterator(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t start, int64_t stop, int64_t *size, rl_list_iterator **_iterator);
int rl_lrange(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t start, int64_t stop, int64_t *size, unsigned char ***values, int64_t **valueslen);
int rl_lrem(struct rlite *db, const unsigned char *key, int64_t keylen, int direction, int64_t maxcount, unsigned char *value, int64_t valuelen, int64_t *count);
int rl_lset(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t index, unsigned char *value, int64_t valuelen);
int rl_ltrim(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t start, int64_t stop);

int rl_llist_pages(struct rlite *db, int64_t page, short *pages);
int rl_llist_delete(struct rlite *db, int64_t value_page);

#endif
