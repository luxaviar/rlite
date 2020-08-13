#include <stdlib.h>
#include <stdint.h>

#define BITOP_AND 0
#define BITOP_OR 1
#define BITOP_XOR 2
#define BITOP_NOT 3

int rl_stringmatchlen(const char *pattern, int patternLen, const char *string, int stringLen, int nocase);
void rl_internal_bitop(int op, uint64_t numkeys, unsigned char **objects, uint64_t *objectslen, unsigned char **result, int64_t *resultlen);
size_t rl_redisPopcount(void *s, int64_t count);
int64_t rl_internal_bitpos(void *s, uint64_t count, int bit);
