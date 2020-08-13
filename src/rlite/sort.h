#ifndef _RL_SORT_H
#define _RL_SORT_H
#include "rlite.h"

typedef struct rliteSortObject {
	unsigned char *obj;
	int64_t objlen;
	union {
		double score;
		struct {
			unsigned char *obj;
			int64_t objlen;
		} cmpobj;
	} u;
} rliteSortObject;

int rl_sort(struct rlite *db, unsigned char *key, int64_t keylen, unsigned char *sortby, int64_t sortbylen, int dontsort, int inLuaScript, int alpha, int desc, int64_t limit_start, int64_t limit_count, int getc, unsigned char **getv, int64_t *getvlen, unsigned char *storekey, int64_t storekeylen, int64_t *retobjc, unsigned char ***retobjv, int64_t **retobjvlen);
#endif
