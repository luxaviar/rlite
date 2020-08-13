#ifndef _RL_TYPE_ZSET_H
#define _RL_TYPE_ZSET_H

#include "page_skiplist.h"

#define RL_TYPE_ZSET 'Z'

#define RL_ZSET_AGGREGATE_SUM 0
#define RL_ZSET_AGGREGATE_MIN 1
#define RL_ZSET_AGGREGATE_MAX 2

struct rlite;

typedef struct {
	double min;
	int minex;
	double max;
	int maxex;
} rl_zrangespec;

typedef struct rl_skiplist_iterator rl_zset_iterator;

int rl_zset_iterator_next(rl_zset_iterator *iterator, int64_t *page, double *score, unsigned char **data, int64_t *datalen);
int rl_zset_iterator_destroy(rl_zset_iterator *iterator);

int rl_zadd(struct rlite *db, const unsigned char *key, int64_t keylen, double score, unsigned char *data, int64_t datalen);
int rl_zcard(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t *card);
int rl_zcount(struct rlite *db, const unsigned char *key, int64_t keylen, rl_zrangespec *range, int64_t *count);
int rl_zincrby(struct rlite *db, const unsigned char *key, int64_t keylen, double score, unsigned char *data, int64_t datalen, double *newscore);
int rl_zinterstore(struct rlite *db, int64_t keys_size, unsigned char **keys, int64_t *keys_len, double *weights, int aggregate);
int rl_zlexcount(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *min, int64_t minlen, unsigned char *max, int64_t maxlen, int64_t *lexcount);
int rl_zrange(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t start, int64_t end, rl_zset_iterator **iterator);
int rl_zrangebylex(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *min, int64_t minlen, unsigned char *max, int64_t maxlen, int64_t offset, int64_t count, rl_zset_iterator **iterator);
int rl_zrangebyscore(struct rlite *db, const unsigned char *key, int64_t keylen, rl_zrangespec *range, int64_t offset, int64_t count, rl_zset_iterator **iterator);
int rl_zrank(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *data, int64_t datalen, int64_t *rank);
int rl_zrevrange(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t start, int64_t end, rl_zset_iterator **iterator);
int rl_zrevrangebylex(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *max, int64_t maxlen, unsigned char *min, int64_t minlen, int64_t offset, int64_t count, rl_zset_iterator **iterator);
int rl_zrevrangebyscore(struct rlite *db, const unsigned char *key, int64_t keylen, rl_zrangespec *range, int64_t offset, int64_t count, rl_zset_iterator **iterator);
int rl_zrevrank(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *data, int64_t datalen, int64_t *revrank);
int rl_zrem(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t members_size, unsigned char **members, int64_t *members_len, int64_t *changed);
int rl_zremrangebylex(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *min, int64_t minlen, unsigned char *max, int64_t maxlen, int64_t *changed);
int rl_zremrangebyrank(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t start, int64_t end, int64_t *changed);
int rl_zremrangebyscore(struct rlite *db, const unsigned char *key, int64_t keylen, rl_zrangespec *range, int64_t *changed);
int rl_zscore(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *data, int64_t datalen, double *score);
int rl_zunionstore(struct rlite *db, int64_t keys_size, unsigned char **keys, int64_t *keys_len, double *weights, int aggregate);

int rl_zset_pages(struct rlite *db, int64_t page, short *pages);
int rl_zset_delete(struct rlite *db, int64_t value_page);

#endif
