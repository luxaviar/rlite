#include "rlite.h"

/**
 * Dumps the content of `key` into `data`.
 *
 * It will contain a two bytes of a redis-compatible dump version,
 * and a crc32 checksum at the end.
 */
int rl_dump(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char **data, int64_t *datalen);
