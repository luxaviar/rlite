#ifndef _RL_OBJ_KEY_H
#define _RL_OBJ_KEY_H

#include <stdint.h>

struct rlite;
struct watched_key;

typedef struct {
	char identifier;
	const char *name;
	int (*delete)(struct rlite *db, int64_t value_page);
} rl_type;

extern rl_type types[];

int rl_key_get_or_create(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char type, int64_t *page, int64_t *version);
int rl_key_get(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *type, int64_t *string_page, int64_t *value_page, uint64_t *expires, int64_t *version);
int rl_check_watched_keys(struct rlite *db, int watched_count, struct watched_key** keys);
int rl_key_set(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char type, int64_t page, uint64_t expires, int64_t version);
int rl_key_delete(struct rlite *db, const unsigned char *key, int64_t keylen);
int rl_key_expires(struct rlite *db, const unsigned char *key, int64_t keylen, uint64_t expires);
int rl_key_delete_value(struct rlite *db, unsigned char identifier, int64_t value_page);
int rl_key_delete_with_value(struct rlite *db, const unsigned char *key, int64_t keylen);
int rl_watch(struct rlite *db, struct watched_key** _watched_key, const unsigned char *key, int64_t keylen);

#endif
