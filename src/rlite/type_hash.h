#ifndef _RL_TYPE_HASH_H
#define _RL_TYPE_HASH_H

#include "page_btree.h"

#define RL_TYPE_HASH 'H'

struct rlite;

typedef rl_btree_iterator rl_hash_iterator;

int rl_hash_iterator_next(rl_hash_iterator *iterator, int64_t *fieldpage, unsigned char **field, int64_t *fieldlen, int64_t *memberpage, unsigned char **member, int64_t *memberlen);
int rl_hash_iterator_destroy(rl_hash_iterator *iterator);

int rl_hset(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *field, int64_t fieldlen, unsigned char *data, int64_t datalen, int64_t *added, int update);
int rl_hget(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *field, int64_t fieldlen, unsigned char **data, int64_t *datalen);
int rl_hexists(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *field, int64_t fieldlen);
int rl_hdel(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t fieldsc, unsigned char **fields, int64_t *fieldslen, int64_t *delcount);
int rl_hgetall(struct rlite *db, rl_hash_iterator **iterator, const unsigned char *key, int64_t keylen);
int rl_hlen(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t *len);
int rl_hmget(struct rlite *db, const unsigned char *key, int64_t keylen, int fieldc, unsigned char **fields, int64_t *fieldslen, unsigned char ***_data, int64_t **_datalen);
int rl_hmset(struct rlite *db, const unsigned char *key, int64_t keylen, int fieldc, unsigned char **fields, int64_t *fieldslen, unsigned char **datas, int64_t *dataslen);
int rl_hincrby(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *field, int64_t fieldlen, int64_t increment, int64_t *newvalue);
int rl_hincrbyfloat(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *field, int64_t fieldlen, double increment, double *newvalue);

int rl_hash_pages(struct rlite *db, int64_t page, short *pages);
int rl_hash_delete(struct rlite *db, int64_t value_page);

#endif
