#ifndef _RL_PUBSUB_H
#define _RL_PUBSUB_H

#include "rlite/port/time.h"
#include "rlite.h"

int rl_subscribe(rlite *db, int channelc, unsigned char **channelv, int64_t *channelvlen);
int rl_psubscribe(rlite *db, int patternc, unsigned char **patternv, int64_t *patternvlen);
int rl_unsubscribe(rlite *db, int channelc, unsigned char **channelv, int64_t *channelvlen);
int rl_punsubscribe(rlite *db, int patternc, unsigned char **patternv, int64_t *patternvlen);
int rl_unsubscribe_all(rlite *db);
int rl_poll(rlite *db, int *elementc, unsigned char ***_elements, int64_t **_elementslen);
/**
 * When timeout is NULL, it will wait for ever.
 */
int rl_poll_wait(rlite *db, int *elementc, unsigned char ***_elements, int64_t **_elementslen, struct timeval *timeout);
int rl_publish(rlite *db, unsigned char *channel, size_t channellen, const char *data, size_t datalen, int64_t *recipients);

int rl_pubsub_count_subscriptions(rlite *db, int64_t *numsubscriptions);
int rl_pubsub_channels(rlite *db, unsigned char *pattern, int64_t patternlen, int64_t* channelc, unsigned char ***channelv, int64_t **channelvlen);
int rl_pubsub_patterns(rlite *db, int64_t* patternc, unsigned char ***patternv, int64_t **patternvlen);
int rl_pubsub_numsub(rlite *db, int channelc, unsigned char **channelv, int64_t *channelvlen, int64_t *numsub);
int rl_pubsub_numpat(rlite *db, int64_t *numpat);

#endif
