#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

extern size_t rl_hll_sparse_max_bytes;
int rl_str_pfadd(unsigned char *str, int64_t strlen, int argc, unsigned char **argv, int64_t *argvlen, unsigned char **_str, int64_t *_strlen);
int rl_str_pfcount(int argc, unsigned char **argv, int64_t *argvlen, int64_t *_card, unsigned char **updatevalue, int64_t *updatevaluelen);
int rl_str_pfmerge(int argc, unsigned char **argv, int64_t *argvlen, unsigned char **_str, int64_t *_strlen);
int rl_str_pfselftest();
int rl_str_pfdebug_getreg(unsigned char *str, int64_t strlen, int *size, int64_t **elements, unsigned char **_str, int64_t *_strlen);
int rl_str_pfdebug_decode(unsigned char *str, int64_t strlen, unsigned char **response, int64_t *responselen);
int rl_str_pfdebug_encoding(unsigned char *str, int64_t strlen, unsigned char **response, int64_t *responselen);
int rl_str_pfdebug_todense(unsigned char *str, int64_t strlen, unsigned char **_str, int64_t *_strlen);
