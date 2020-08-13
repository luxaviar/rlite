#include "rlite.h"

typedef struct rl_restore_streamer {
	void *context;
	int (*read)(struct rl_restore_streamer *streamer, unsigned char *str, int64_t len);
} rl_restore_streamer;

int rl_restore(struct rlite *db, const unsigned char *key, int64_t keylen, uint64_t expires, unsigned char *data, int64_t datalen);
int rl_restore_stream(struct rlite *db, const unsigned char *key, int64_t keylen, uint64_t expires, rl_restore_streamer *streamer);
