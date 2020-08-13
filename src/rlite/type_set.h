#ifndef _RL_TYPE_SET_H
#define _RL_TYPE_SET_H

#include "page_btree.h"

#define RL_TYPE_SET 'S'

struct rlite;

typedef rl_btree_iterator rl_set_iterator;

int rl_set_get_objects(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t *_set_page_number, rl_btree **btree, int update_version, int create);
int rl_set_iterator_next(rl_set_iterator *iterator, int64_t *page, unsigned char **member, int64_t *memberlen);
int rl_set_iterator_destroy(rl_set_iterator *iterator);

int rl_sadd(struct rlite *db, const unsigned char *key, int64_t keylen, int memberc, unsigned char **members, int64_t *memberslen, int64_t *added);
int rl_sismember(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *data, int64_t datalen);
int rl_scard(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t *card);
int rl_srem(struct rlite *db, const unsigned char *key, int64_t keylen, int membersc, unsigned char **members, int64_t *memberslen, int64_t *delcount);
int rl_smove(struct rlite *db, const unsigned char *source, int64_t sourcelen, const unsigned char *destination, int64_t destinationlen, unsigned char *member, int64_t memberlen);
int rl_smembers(struct rlite *db, rl_set_iterator **iterator, const unsigned char *key, int64_t keylen);
int rl_srandmembers(struct rlite *db, const unsigned char *key, int64_t keylen, int repeat, int64_t *memberc, unsigned char ***members, int64_t **memberslen);
int rl_spop(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char **member, int64_t *memberlen);
int rl_sdiff(struct rlite *db, int keyc, unsigned char **keys, int64_t *keyslen, int64_t *_membersc, unsigned char ***_members, int64_t **_memberslen);
int rl_sdiffstore(struct rlite *db, unsigned char *target, int64_t targetlen, int keyc, unsigned char **keys, int64_t *keyslen, int64_t *added);
int rl_sinter(struct rlite *db, int keyc, unsigned char **keys, int64_t *keyslen, int64_t *_membersc, unsigned char ***_members, int64_t **_memberslen);
int rl_sinterstore(struct rlite *db, unsigned char *target, int64_t targetlen, int keyc, unsigned char **keys, int64_t *keyslen, int64_t *added);
int rl_sunion(struct rlite *db, int keyc, unsigned char **keys, int64_t *keyslen, int64_t *_membersc, unsigned char ***_members, int64_t **_memberslen);
int rl_sunionstore(struct rlite *db, unsigned char *target, int64_t targetlen, int keyc, unsigned char **keys, int64_t *keyslen, int64_t *added);

int rl_set_pages(struct rlite *db, int64_t page, short *pages);
int rl_set_delete(struct rlite *db, int64_t value_page);

#endif
