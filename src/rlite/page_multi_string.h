#ifndef _RL_OBJ_STRING_H
#define _RL_OBJ_STRING_H

struct rlite;

int rl_normalize_string_range(int64_t totalsize, int64_t *start, int64_t *stop);
int rl_multi_string_cmp(struct rlite *db, int64_t p1, int64_t p2, int *cmp);
int rl_multi_string_cmp_str(struct rlite *db, int64_t p1, unsigned char *str, int64_t len, int *cmp);
int rl_multi_string_getrange(struct rlite *db, int64_t number, unsigned char **_data, int64_t *size, int64_t start, int64_t stop);
int rl_multi_string_get(struct rlite *db, int64_t number, unsigned char **data, int64_t *size);
int rl_multi_string_setrange(struct rlite *db, int64_t number, const unsigned char *data, int64_t size, int64_t offset, int64_t *newlength);
int rl_multi_string_set(struct rlite *db, int64_t *number, const unsigned char *data, int64_t size);
int rl_multi_string_append(struct rlite *db, int64_t number, const unsigned char *data, int64_t datasize, int64_t *newlength);
int rl_multi_string_sha1(struct rlite *db, unsigned char data[20], int64_t number);
int rl_multi_string_pages(struct rlite *db, int64_t page, short *pages);
int rl_multi_string_delete(struct rlite *db, int64_t page);
int rl_multi_string_cpyrange(struct rlite *db, int64_t number, unsigned char *data, int64_t *size, int64_t start, int64_t stop);
int rl_multi_string_cpy(struct rlite *db, int64_t number, unsigned char *data, int64_t *size);

#endif
