#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "../rlite.h"
#include "../type_zset.h"
#include "../page_key.h"
#include "test_util.h"

int basic_test_zadd_zscore(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zscore %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);
	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key);
	double score = 1.41, score2;
	unsigned char *data = (unsigned char *)"my data";
	long datalen = strlen((char *)data);

	RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, score, data, datalen);

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_zscore, RL_FOUND, db, key, keylen, data, datalen, &score2);

	if (score != score2) {
		fprintf(stderr, "Expected score %lf to match score2 %lf\n", score, score2);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	fprintf(stderr, "End basic_test_zadd_zscore\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

int basic_test_zadd_zscore2(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zscore2 %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);
	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key);
	double score = 8913.109, score2;
	unsigned char *data = (unsigned char *)"my data";
	long datalen = strlen((char *)data);
	unsigned char *data2 = (unsigned char *)"my data2";
	long datalen2 = strlen((char *)data2);
	long card;

	RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, score, data, datalen);

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_zcard, RL_OK, db, key, keylen, &card);
	if (card != 1) {
		fprintf(stderr, "Expected zcard to be 1, got %ld instead\n", card);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, score, data2, datalen2);

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_zcard, RL_OK, db, key, keylen, &card);
	if (card != 2) {
		fprintf(stderr, "Expected zcard to be 2, got %ld instead\n", card);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_zscore, RL_FOUND, db, key, keylen, data, datalen, &score2);

	if (score != score2) {
		fprintf(stderr, "Expected score %lf to match score2 %lf\n", score, score2);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_zscore, RL_FOUND, db, key, keylen, data2, datalen2, &score2);

	if (score != score2) {
		fprintf(stderr, "Expected score %lf to match score2 %lf\n", score, score2);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	fprintf(stderr, "End basic_test_zadd_zscore2\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

int basic_test_zadd_zrank(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zrank %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);
	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key);
	double score = 8913.109;
	long rank;
	unsigned char *data = (unsigned char *)"my data";
	long datalen = strlen((char *)data);
	unsigned char *data2 = (unsigned char *)"my data2";
	long datalen2 = strlen((char *)data2);

	RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, score, data, datalen);

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, score, data2, datalen2);

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_zrank, RL_FOUND, db, key, keylen, data, datalen, &rank);

	if (0 != rank) {
		fprintf(stderr, "Expected rank %d to be %ld\n", 0, rank);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_zrank, RL_FOUND, db, key, keylen, data2, datalen2, &rank);

	if (1 != rank) {
		fprintf(stderr, "Expected rank %d to be %ld\n", 1, rank);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_zrevrank, RL_FOUND, db, key, keylen, data, datalen, &rank);

	if (1 != rank) {
		fprintf(stderr, "Expected rank %d to be %ld\n", 1, rank);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_zrevrank, RL_FOUND, db, key, keylen, data2, datalen2, &rank);

	if (0 != rank) {
		fprintf(stderr, "Expected rank %d to be %ld\n", 0, rank);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	fprintf(stderr, "End basic_test_zadd_zrank\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

int basic_test_zadd_zrange()
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zrange\n");

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, 0, 1);

	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key);

	long i, setdatalen = 1;
	unsigned char setdata[1];
	for (i = 0; i < 200; i++) {
		setdata[0] = i;

		RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, i * 10.5, setdata, setdatalen);

		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	unsigned char *data;
	long datalen;
	double score;
	rl_zset_iterator* iterator;

	RL_CALL_VERBOSE(rl_zrange, RL_OK, db, key, keylen, 0, -1, &iterator);
	if (iterator->size != 200) {
		fprintf(stderr, "Expected size to be 200, got %ld instead\n", iterator->size);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	rl_zset_iterator_destroy(iterator);

	RL_CALL_VERBOSE(rl_zrevrange, RL_OK, db, key, keylen, 0, -1, &iterator);
	if (iterator->size != 200) {
		fprintf(stderr, "Expected size to be 200, got %ld instead\n", iterator->size);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	rl_zset_iterator_destroy(iterator);

	RL_CALL_VERBOSE(rl_zrange, RL_OK, db, key, keylen, 0, 0, &iterator);
	if (iterator->size != 1) {
		fprintf(stderr, "Expected size to be 1, got %ld instead\n", iterator->size);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	retval = rl_zset_iterator_next(iterator, NULL, &data, &datalen);
	if (retval != RL_OK) {
		fprintf(stderr, "Failed to fetch next element in zset iterator\n");
		goto cleanup;
	}
	if (data[0] != 0) {
		fprintf(stderr, "Expected data to be 0, got %d instead\n", data[0]);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	if (datalen != 1) {
		fprintf(stderr, "Expected data to be 1, got %ld instead\n", datalen);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	rl_free(data);
	rl_zset_iterator_destroy(iterator);

	RL_CALL_VERBOSE(rl_zrevrange, RL_OK, db, key, keylen, 0, -1, &iterator);
	if (iterator->size != 200) {
		fprintf(stderr, "Expected size to be 200, got %ld instead\n", iterator->size);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	retval = rl_zset_iterator_next(iterator, &score, &data, &datalen);
	if (retval != RL_OK) {
		fprintf(stderr, "Failed to fetch next element in zset iterator\n");
		goto cleanup;
	}
	if (score != 10.5 * 199) {
		fprintf(stderr, "Expected score to be %lf, got %lf instead\n", 10.5 * 199, score);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	if (data[0] != 199) {
		fprintf(stderr, "Expected data to be 199, got %d instead\n", data[0]);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	if (datalen != 1) {
		fprintf(stderr, "Expected data to be 1, got %ld instead\n", datalen);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	rl_free(data);
	rl_zset_iterator_destroy(iterator);

	fprintf(stderr, "End basic_test_zadd_zrange\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

static int test_zrangebylex(rlite *db, unsigned char *key, long keylen, long initial, long size, long total_size, unsigned char min[3], long minlen, unsigned char max[3], long maxlen, long offset, long limit)
{
	rl_zset_iterator *iterator;
	long lexcount;
	int retval;
	unsigned char *data2;
	long data2_len;
	long i;
	if (offset == 0 && limit == 0) {
		// These test expect different values if offset or limit exist

		retval = rl_zlexcount(db, key, keylen, min, minlen, max, maxlen, &lexcount);
		if (retval != RL_NOT_FOUND || size != 0) {
			if (retval != RL_OK) {
				fprintf(stderr, "Unable to rl_zlexcount, got %d\n", retval);
				goto cleanup;
			}
			if (size != lexcount) {
				fprintf(stderr, "Expected lexcount to be %ld, got %ld instead\n", size, lexcount);
				retval = RL_UNEXPECTED;
				goto cleanup;
			}
		}
	}

	retval = rl_zrangebylex(db, key, keylen, min, minlen, max, maxlen, offset, limit, &iterator);
	if (retval != RL_NOT_FOUND || size != 0) {
		if (retval != RL_OK) {
			fprintf(stderr, "Unable to zrangebylex, got %d\n", retval);
			goto cleanup;
		}
		if (iterator->size != size) {
			fprintf(stderr, "Expected zrangebylex size to be %ld, got %ld instead\n", size, iterator->size);
			retval = RL_UNEXPECTED;
			goto cleanup;
		}
		i = initial;
		while ((retval = rl_zset_iterator_next(iterator, NULL, &data2, &data2_len)) == RL_OK) {
			if (data2_len != ((i & 1) == 0 ? 1 : 2)) {
				fprintf(stderr, "Unexpected datalen %ld in element %ld in line %d\n", data2_len, i, __LINE__);
				goto cleanup;
			}
			if (data2[0] != 'a' + (i / 2)) {
				fprintf(stderr, "Unexpected data[0] %d, expected %ld in iterator %ld on line %d\n", data2[0], 'a' + (i / 2), i, __LINE__);
				goto cleanup;
			}
			if (data2_len == 2 && data2[1] != 'A' + i) {
				fprintf(stderr, "Unexpected data[1] %d, expected %ld in iterator %ld on line %d\n", data2[1], 'A' + i, i, __LINE__);
				goto cleanup;
			}
			i++;
			rl_free(data2);
		}

		if (i != size + initial) {
			fprintf(stderr, "Expected size to be %ld, got %ld instead\n", size + initial, i);
			retval = RL_UNEXPECTED;
			goto cleanup;
		}

		if (retval != RL_END) {
			fprintf(stderr, "Iterator finished without RL_END, got %d\n", retval);
			goto cleanup;
		}
	}

	retval = rl_zrevrangebylex(db, key, keylen, min, minlen, max, maxlen, offset, limit, &iterator);
	if (retval != RL_NOT_FOUND || size != 0) {
		if (retval != RL_OK) {
			fprintf(stderr, "Unable to zrangebylex, got %d\n", retval);
			goto cleanup;
		}
		if (iterator->size != size) {
			fprintf(stderr, "Expected zrangebylex size to be %ld, got %ld instead\n", size, iterator->size);
			retval = RL_UNEXPECTED;
			goto cleanup;
		}

		i = total_size - 1 - offset;
		while ((retval = rl_zset_iterator_next(iterator, NULL, &data2, &data2_len)) == RL_OK) {
			if (data2_len != ((i & 1) == 0 ? 1 : 2)) {
				fprintf(stderr, "Unexpected datalen %ld in element %ld in line %d\n", data2_len, i, __LINE__);
				goto cleanup;
			}
			if (data2[0] != 'a' + (i / 2)) {
				fprintf(stderr, "Unexpected data[0] %d, expected %ld in iterator %ld on line %d\n", data2[0], 'a' + (i / 2), i, __LINE__);
				goto cleanup;
			}
			if (data2_len == 2 && data2[1] != 'A' + i) {
				fprintf(stderr, "Unexpected data[1] %d, expected %ld in iterator %ld on line %d\n", data2[1], 'A' + i, i, __LINE__);
				goto cleanup;
			}
			i--;
			rl_free(data2);
		}

		if (retval != RL_END) {
			fprintf(stderr, "Iterator finished without RL_END, got %d\n", retval);
			goto cleanup;
		}

		if (i != total_size - size - 1 - offset) {
			fprintf(stderr, "Expected initial to be %ld, got %ld instead\n", total_size - size - 1 - offset, i);
			retval = RL_UNEXPECTED;
			goto cleanup;
		}
	}
	retval = 0;
cleanup:
	return retval;
}

int basic_test_zadd_zrangebylex(int _commit)
{
#define ZRANGEBYLEX_SIZE 20
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zrangebylex %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);

	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key);

	unsigned char data[2];
	long i;
	for (i = 0; i < ZRANGEBYLEX_SIZE; i++) {
		data[0] = 'a' + (i / 2);
		data[1] = 'A' + i;
		RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, 1.0, data, ((i & 1) == 0) ? 1 : 2);

		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	unsigned char min[3];
	unsigned char max[3];

#define run_test_zrangebylex(min0, min1, minlen, max0, max1, maxlen, initial, size, total_size, offset, limit)\
	min[0] = min0;\
	min[1] = min1;\
	max[0] = max0;\
	max[1] = max1;\
	retval = test_zrangebylex(db, key, keylen, initial, size, total_size, min, minlen, max, maxlen, offset, limit);\
	if (retval != 0) {\
		fprintf(stderr, "Failed zrangebylex on line %d\n", __LINE__);\
		goto cleanup;\
	}

	run_test_zrangebylex('-', 0, 1, '(', 'a', 2, 0, 0, 0, 0, 0)
	run_test_zrangebylex('-', 0, 1, '+', 0, 1, 0, ZRANGEBYLEX_SIZE, ZRANGEBYLEX_SIZE, 0, 0)
	run_test_zrangebylex('-', 0, 1, '+', 0, 1, 1, ZRANGEBYLEX_SIZE - 1, ZRANGEBYLEX_SIZE, 1, 0)
	run_test_zrangebylex('-', 0, 1, '+', 0, 1, 0, 1, ZRANGEBYLEX_SIZE, 0, 1)
	run_test_zrangebylex('(', 'c', 2, '+', 0, 1, 5, ZRANGEBYLEX_SIZE - 5, ZRANGEBYLEX_SIZE, 0, 0)
	run_test_zrangebylex('[', 'c', 2, '+', 0, 1, 4, ZRANGEBYLEX_SIZE - 4, ZRANGEBYLEX_SIZE, 0, 0)
	run_test_zrangebylex('[', 'c', 2, '[', 'f', 2, 4, 7, 11, 0, 0)
	run_test_zrangebylex('-', 0, 1, '[', 'f', 2, 0, 11, 11, 0, 0)

	fprintf(stderr, "End basic_test_zadd_zrangebylex\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

static int test_zrangebyscore(rlite *db, unsigned char *key, long keylen, rl_zrangespec *range, long size, double base_score, double step)
{
	rl_zset_iterator *iterator;
	long offset = 0, limit = 0;
	int retval = rl_zrangebyscore(db, key, keylen, range, offset, limit, &iterator);
	if (size != 0 || retval != RL_NOT_FOUND) {
		if (retval != RL_OK) {
			fprintf(stderr, "Unable to zrangebyscore, got %d\n", retval);
			goto cleanup;
		}

		double score;
		long i = 0;
		while ((retval = rl_zset_iterator_next(iterator, &score, NULL, NULL)) == RL_OK) {
			if (score != base_score + i * step) {
				fprintf(stderr, "Expected score to be %lf, got %lf instead\n", base_score + i * step, score);
				retval = RL_UNEXPECTED;
				goto cleanup;
			}
			i++;
		}
		if (retval != RL_END) {
			fprintf(stderr, "Expected iterator to finish, got %d instead\n", retval);
			retval = RL_UNEXPECTED;
			goto cleanup;
		}
		if (i != size) {
			fprintf(stderr, "Expected size to be %ld, got %ld\n", size, i);
			retval = RL_UNEXPECTED;
			goto cleanup;
		}
	}

	retval = rl_zrevrangebyscore(db, key, keylen, range, offset, limit, &iterator);
	if (size != 0 || retval != RL_NOT_FOUND) {
		if (retval != RL_OK) {
			fprintf(stderr, "Unable to zrevrangebyscore, got %d\n", retval);
			goto cleanup;
		}

		double score;
		long i = size - 1;
		while ((retval = rl_zset_iterator_next(iterator, &score, NULL, NULL)) == RL_OK) {
			if (score != base_score + i * step) {
				fprintf(stderr, "Expected score to be %lf, got %lf instead in line %d\n", base_score + i * step, score, __LINE__);
				retval = RL_UNEXPECTED;
				goto cleanup;
			}
			i--;
		}
		if (retval != RL_END) {
			fprintf(stderr, "Expected iterator to finish, got %d instead\n", retval);
			retval = RL_UNEXPECTED;
			goto cleanup;
		}
		if (i != -1) {
			fprintf(stderr, "Expected size to be %d, got %ld\n", -1, i);
			retval = RL_UNEXPECTED;
			goto cleanup;
		}
	}

	retval = 0;
cleanup:
	return retval;
}

int basic_test_zadd_zrangebyscore(int _commit)
{
#define ZRANGEBYSCORE_SIZE 20
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zrangebyscore %d\n", _commit);
	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);

	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key);

	unsigned char data[1];
	long i;
	for (i = 0; i < ZRANGEBYSCORE_SIZE; i++) {
		data[0] = 'a' + i;
		RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, i, data, 1);

		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	rl_zrangespec range;
#define run_test_zrangebyscore(_min, _minex, _max, _maxex, size, base_score) \
	range.min = _min;\
	range.minex = _minex;\
	range.max = _max;\
	range.maxex = _maxex;\
	if (0 != test_zrangebyscore(db, key, keylen, &range, size, base_score, 1)) {\
		fprintf(stderr, "zrangebyscore test failed on line %d\n", __LINE__);\
		goto cleanup;\
	}
	run_test_zrangebyscore(-INFINITY, 1, INFINITY, 1, ZRANGEBYSCORE_SIZE, 0);
	run_test_zrangebyscore(-INFINITY, 0, INFINITY, 1, ZRANGEBYSCORE_SIZE, 0);
	run_test_zrangebyscore(-INFINITY, 1, INFINITY, 0, ZRANGEBYSCORE_SIZE, 0);
	run_test_zrangebyscore(-INFINITY, 0, INFINITY, 0, ZRANGEBYSCORE_SIZE, 0);
	run_test_zrangebyscore(-5, 0, INFINITY, 0, ZRANGEBYSCORE_SIZE, 0);
	run_test_zrangebyscore(5, 0, INFINITY, 0, ZRANGEBYSCORE_SIZE - 5, 5);
	run_test_zrangebyscore(5, 0, 6, 1, 1, 5);
	run_test_zrangebyscore(21, 0, 40, 0, 0, 0);
	run_test_zrangebyscore(-INFINITY, 0, -5, 0, 0, 0);
	run_test_zrangebyscore(0, 0, 0, 0, 1, 0);
	run_test_zrangebyscore(0, 1, 0, 1, 0, 0);
	run_test_zrangebyscore(1, 0, 1, 0, 1, 1);

	fprintf(stderr, "End basic_test_zadd_zrangebyscore\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

int basic_test_zadd_zrem(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zrem %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);

	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key);
	double score = 8913.109;
	long rank;
	unsigned char *data = (unsigned char *)"my data";
	long datalen = strlen((char *)data);
	unsigned char *data2 = (unsigned char *)"my data2";
	long datalen2 = strlen((char *)data2);

	RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, score, data, datalen);

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, score, data2, datalen2);

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	unsigned char *members[1] = {data};
	long members_len[1] = {datalen};
	long changed;
	RL_CALL_VERBOSE(rl_zrem, RL_OK, db, key, keylen, 1, members, members_len, &changed);
	if (changed != 1) {
		fprintf(stderr, "Expected to have removed 1 element, got %ld\n", changed);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_zrank, RL_NOT_FOUND, db, key, keylen, data, datalen, &rank);
	RL_CALL_VERBOSE(rl_zscore, RL_NOT_FOUND, db, key, keylen, data, datalen, NULL);
	RL_CALL_VERBOSE(rl_zrank, RL_FOUND, db, key, keylen, data2, datalen2, &rank);

	if (0 != rank) {
		fprintf(stderr, "Expected rank %d to be %ld\n", 0, rank);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	unsigned char *members2[2] = {data, data2};
	long members_len2[2] = {datalen, datalen2};
	RL_CALL_VERBOSE(rl_zrem, RL_OK, db, key, keylen, 2, members2, members_len2, &changed);
	if (changed != 1) {
		fprintf(stderr, "Expected to have removed 1 element, got %ld\n", changed);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	retval = rl_key_get(db, key, keylen, NULL, NULL, NULL, NULL);
	if (retval != RL_NOT_FOUND) {
		fprintf(stderr, "Expected not to find key after removing all zset elements, got %ld\n", changed);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	fprintf(stderr, "End basic_test_zadd_zrem\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

#define ZCOUNT_SIZE 100
int basic_test_zadd_zcount(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zcount %d\n", _commit);

	rlite *db = NULL;
	long datalen = 20;
	unsigned char *data = malloc(sizeof(unsigned char) * datalen);
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);

	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key);
	long i, count;

	for (i = 0; i < datalen; i++) {
		data[i] = i;
	}

	for (i = 0; i < ZCOUNT_SIZE; i++) {
		data[0] = i;
		RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, i * 1.0, data, datalen);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}
	rl_free(data);
	data = NULL;

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	rl_zrangespec range;
	range.min = -1000;
	range.minex = 0;
	range.max = 1000;
	range.maxex = 0;
	RL_CALL_VERBOSE(rl_zcount, RL_OK, db, key, keylen, &range, &count);
	if (count != ZCOUNT_SIZE) {
		fprintf(stderr, "Expected zcount to be %d, got %ld on line %d\n", ZCOUNT_SIZE, count, __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	range.min = 0;
	range.minex = 0;
	range.max = ZCOUNT_SIZE - 1;
	range.maxex = 0;
	RL_CALL_VERBOSE(rl_zcount, RL_OK, db, key, keylen, &range, &count);
	if (count != ZCOUNT_SIZE) {
		fprintf(stderr, "Expected zcount to be %d, got %ld on line %d\n", ZCOUNT_SIZE, count, __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	range.min = 0;
	range.minex = 1;
	range.max = ZCOUNT_SIZE - 1;
	range.maxex = 1;
	RL_CALL_VERBOSE(rl_zcount, RL_OK, db, key, keylen, &range, &count);
	if (count != ZCOUNT_SIZE - 2) {
		fprintf(stderr, "Expected zcount to be %d, got %ld\n", ZCOUNT_SIZE - 2, count);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	range.min = 1;
	range.minex = 1;
	range.max = 2;
	range.maxex = 0;
	RL_CALL_VERBOSE(rl_zcount, RL_OK, db, key, keylen, &range, &count);
	if (count != 1) {
		fprintf(stderr, "Expected zcount to be %d, got %ld\n", 1, count);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	fprintf(stderr, "End basic_test_zadd_zcount\n");
	retval = 0;
cleanup:
	rl_free(data);
	if (db) {
		rl_close(db);
	}
	return retval;
}

int basic_test_zadd_zincrby(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zincrby %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);

	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key);
	double score = 4.2;
	unsigned char *data = (unsigned char *)"my data";
	long datalen = strlen((char *)data);
	double newscore;

	RL_CALL_VERBOSE(rl_zincrby, RL_OK, db, key, keylen, score, data, datalen, &newscore);

	if (newscore != score) {
		fprintf(stderr, "Expected new score %lf to match incremented score %lf\n", newscore, score);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_zincrby, RL_OK, db, key, keylen, score, data, datalen, &newscore);
	if (newscore != score * 2) {
		fprintf(stderr, "Expected new score %lf to match incremented twice score %lf\n", newscore, 2 * score);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	fprintf(stderr, "End basic_test_zadd_zincrby\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

int basic_test_zadd_dupe(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_dupe %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);

	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key);
	double score = 4.2, newscore;
	unsigned char *data = (unsigned char *)"my data";
	long datalen = strlen((char *)data);

	RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, score, data, datalen);
	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_zadd, RL_FOUND, db, key, keylen, score * 2, data, datalen);
	RL_CALL_VERBOSE(rl_zscore, RL_FOUND, db, key, keylen, data, datalen, &newscore);
	if (newscore != score * 2) {
		fprintf(stderr, "Expected new score %lf to match incremented twice score %lf\n", newscore, 2 * score);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	fprintf(stderr, "End basic_test_zadd_dupe\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

#define ZINTERSTORE_KEYS 4
#define ZINTERSTORE_MEMBERS 10
int basic_test_zadd_zinterstore(int _commit, long params[5])
{
	unsigned char *data = NULL;
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zinterstore %d %ld %ld %ld %ld %ld\n", _commit, params[0], params[1], params[2], params[3], params[4]);

	long keys_len[ZINTERSTORE_KEYS];
	unsigned char *keys[ZINTERSTORE_KEYS];
	long i, j;
	for (i = 0; i < ZINTERSTORE_KEYS; i++) {
		keys[i] = malloc(sizeof(unsigned char));
		keys[i][0] = 'a' + i;
		keys_len[i] = 1;
	}

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);

	unsigned char members[ZINTERSTORE_MEMBERS][1];
	for (i = 0; i < ZINTERSTORE_MEMBERS; i++) {
		members[i][0] = 'A' + i;
	}

	for (i = 1; i < ZINTERSTORE_KEYS; i++) {
		for (j = 0; j < ZINTERSTORE_MEMBERS; j++) {
			RL_CALL_VERBOSE(rl_zadd, RL_OK, db, keys[i], 1, i * j, members[j], 1);
			RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
		}
		unsigned char own_member[1];
		own_member[0] = (unsigned char)(CHAR_MAX - i);
		RL_CALL_VERBOSE(rl_zadd, RL_OK, db, keys[i], 1, i, own_member, 1);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	double weights[3];
	weights[0] = params[1];
	weights[1] = params[2];
	weights[2] = params[3];
	RL_CALL_VERBOSE(rl_zinterstore, RL_OK, db, ZINTERSTORE_KEYS, keys, keys_len, params[1] == 0 && params[2] == 0 && params[3] == 0 ? NULL : weights, params[0]);
	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	rl_zset_iterator *iterator;
	RL_CALL_VERBOSE(rl_zrange, RL_OK, db, keys[0], 1, 0, -1, &iterator);
	if (iterator->size != ZINTERSTORE_MEMBERS) {
		fprintf(stderr, "Expected size to be %d, got %ld instead\n", ZINTERSTORE_MEMBERS, iterator->size);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	i = 0;
	long datalen;
	double score;
	while ((retval = rl_zset_iterator_next(iterator, &score, &data, &datalen)) == RL_OK) {
		if (score != i * params[4]) {
			fprintf(stderr, "Expected score to be %ld, got %lf instead\n", i * params[4], score);
			retval = RL_UNEXPECTED;
			goto cleanup;
		}
		if (data[0] != members[i][0]) {
			fprintf(stderr, "Member mismatch\n");
			goto cleanup;
		}
		if (datalen != 1) {
			fprintf(stderr, "Member len mismatch\n");
			goto cleanup;
		}
		rl_free(data);
		data = NULL;
		i++;
	}

	if (retval != RL_END) {
		fprintf(stderr, "Expected iterator to finish, got %d instead\n", retval);
		goto cleanup;
	}

	fprintf(stderr, "End basic_test_zadd_zinterstore\n");

	retval = 0;
cleanup:
	for (i = 0; i < ZINTERSTORE_KEYS; i++) {
		rl_free(keys[i]);
	}
	rl_free(data);
	if (db) {
		rl_close(db);
	}
	return retval;
}

#define ZUNIONSTORE_KEYS 4
#define ZUNIONSTORE_MEMBERS 10
int basic_test_zadd_zunionstore(int _commit, long params[5])
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zunionstore %d %ld %ld %ld %ld %ld\n", _commit, params[0], params[1], params[2], params[3], params[4]);

	long keys_len[ZUNIONSTORE_KEYS];
	unsigned char *keys[ZUNIONSTORE_KEYS];
	long i, j;
	for (i = 0; i < ZUNIONSTORE_KEYS; i++) {
		keys[i] = malloc(sizeof(unsigned char));
		keys[i][0] = 'a' + i;
		keys_len[i] = 1;
	}

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);

	unsigned char members[ZUNIONSTORE_MEMBERS][1];
	for (i = 0; i < ZUNIONSTORE_MEMBERS; i++) {
		members[i][0] = 'A' + i;
	}

	for (i = 1; i < ZUNIONSTORE_KEYS; i++) {
		for (j = 0; j < ZUNIONSTORE_MEMBERS; j++) {
			RL_CALL_VERBOSE(rl_zadd, RL_OK, db, keys[i], 1, i * j, members[j], 1);
			RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
		}
		unsigned char own_member[1];
		own_member[0] = (unsigned char)(CHAR_MAX - i);
		RL_CALL_VERBOSE(rl_zadd, RL_OK, db, keys[i], 1, i, own_member, 1);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	double weights[3];
	weights[0] = params[1];
	weights[1] = params[2];
	weights[2] = params[3];
	RL_CALL_VERBOSE(rl_zunionstore, RL_OK, db, ZUNIONSTORE_KEYS, keys, keys_len, params[1] == 0 && params[2] == 0 && params[3] == 0 ? NULL : weights, params[0]);

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	rl_zset_iterator *iterator;
	RL_CALL_VERBOSE(rl_zrange, RL_OK, db, keys[0], 1, 0, -1, &iterator);
	if (iterator->size != ZUNIONSTORE_MEMBERS + ZUNIONSTORE_KEYS - 1) {
		fprintf(stderr, "Expected size to be %d, got %ld instead\n", ZUNIONSTORE_MEMBERS + ZUNIONSTORE_KEYS - 1, iterator->size);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	i = 0;
	unsigned char *data;
	long datalen;
	double score, exp_score;
	long pos;
	while ((retval = rl_zset_iterator_next(iterator, &score, &data, &datalen)) == RL_OK) {
		if (data[0] == (unsigned char)(CHAR_MAX - 1) || data[0] == (unsigned char)(CHAR_MAX - 2) || data[0] == (unsigned char)(CHAR_MAX - 3)) {
			pos = (unsigned char)CHAR_MAX - data[0];
			exp_score = pos * (params[1] == 0 && params[2] == 0 && params[3] == 0 ? 1 : params[pos]);
			if (exp_score != score) {
				fprintf(stderr, "Member score mismatch, expected %lf, got %lf\n", exp_score, score);
				goto cleanup;
			}
			if (datalen != 1) {
				fprintf(stderr, "Member len mismatch\n");
				goto cleanup;
			}
			i--;
		} else {
			if (score != i * params[4]) {
				fprintf(stderr, "Expected score to be %ld, got %lf instead\n", i * params[4], score);
				retval = RL_UNEXPECTED;
				goto cleanup;
			}
			if (data[0] != members[i][0]) {
				fprintf(stderr, "Member mismatch\n");
				retval = RL_UNEXPECTED;
				goto cleanup;
			}
			if (datalen != 1) {
				fprintf(stderr, "Member len mismatch\n");
				retval = RL_UNEXPECTED;
				goto cleanup;
			}
		}
		rl_free(data);
		i++;
	}

	if (retval != RL_END) {
		fprintf(stderr, "Expected iterator to finish, got %d instead\n", retval);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	fprintf(stderr, "End basic_test_zadd_zunionstore\n");

	retval = 0;
cleanup:
	for (i = 0; i < ZUNIONSTORE_KEYS; i++) {
		rl_free(keys[i]);
	}
	if (db) {
		rl_close(db);
	}
	return retval;
}

int basic_test_zadd_zremrangebyrank(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zremrangebyrank %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);

#define ZREMRANGEBYRANK_SIZE 20
	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key), i, changed;
	unsigned char data[1];
	for (i = 0; i < ZREMRANGEBYRANK_SIZE; i++) {
		data[0] = i;
		RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, i * 1.0, data, 1);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

#define run_test_zremrangebyrank(start, end, changed_expected, data_rank, rank_expected)\
	retval = rl_zremrangebyrank(db, key, keylen, start, end, &changed);\
	if (retval != RL_OK && retval != RL_DELETED) {\
		fprintf(stderr, "Failed to zremrangebyrank, got %d on line %d\n", retval, __LINE__);\
		goto cleanup;\
	}\
	if (changed != changed_expected) {\
		fprintf(stderr, "Expected to delete %d elements, got %ld on line %d\n", changed_expected, changed, __LINE__);\
		retval = RL_UNEXPECTED;\
		goto cleanup;\
	}\
	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);\
	data[0] = data_rank;\
	retval = rl_zrank(db, key, keylen, data, 1, &i);\
	if (rank_expected == -1) {\
		if (retval != RL_NOT_FOUND) {\
			fprintf(stderr, "Failed to zrank, got %d on line %d\n", retval, __LINE__);\
			goto cleanup;\
		}\
	} else {\
		if (retval != RL_FOUND) {\
			fprintf(stderr, "Failed to zrank, got %d on line %d\n", retval, __LINE__);\
			goto cleanup;\
		}\
		if (i != rank_expected) {\
			fprintf(stderr, "Expected rank to be %d, got %ld on line %d\n", rank_expected, i, __LINE__);\
			retval = RL_UNEXPECTED;\
			goto cleanup;\
		}\
	}

	run_test_zremrangebyrank(0, 10, 11, 19, 8);
	run_test_zremrangebyrank(-5, -1, 5, 14, 3);
	run_test_zremrangebyrank(0, 0, 1, 14, 2);
	run_test_zremrangebyrank(0, -1, 3, 0, -1);

	fprintf(stderr, "End basic_test_zadd_zremrangebyrank\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

int basic_test_zadd_zremrangebyscore(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zremrangebyscore %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);

#define ZREMRANGEBYSCORE_SIZE 20
	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key), i, changed;
	unsigned char data[1];
	for (i = 0; i < ZREMRANGEBYSCORE_SIZE; i++) {
		data[0] = i;
		RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, i * 10.0, data, 1);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	rl_zrangespec range;

#define run_test_zremrangebyscore(_min, _minex, _max, _maxex, changed_expected, data_score, rank_expected)\
	range.min = _min;\
	range.minex = _minex;\
	range.max = _max;\
	range.maxex = _maxex;\
	retval = rl_zremrangebyscore(db, key, keylen, &range, &changed);\
	if ((changed_expected > 0 && retval != RL_OK) || (changed_expected == 0 && retval != RL_NOT_FOUND)) {\
		fprintf(stderr, "Failed to zremrangebyscore, got %d on line %d\n", retval, __LINE__);\
		goto cleanup;\
	}\
	if (changed_expected && changed != changed_expected) {\
		fprintf(stderr, "Expected to delete %d elements, got %ld on line %d\n", changed_expected, changed, __LINE__);\
		retval = RL_UNEXPECTED;\
		goto cleanup;\
	}\
	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);\
	data[0] = data_score;\
	retval = rl_zrank(db, key, keylen, data, 1, &i);\
	if (rank_expected == -1) {\
		if (retval != RL_NOT_FOUND) {\
			fprintf(stderr, "Failed to zscore, got %d on line %d\n", retval, __LINE__);\
			goto cleanup;\
		}\
	} else {\
		if (retval != RL_FOUND) {\
			fprintf(stderr, "Failed to zscore, got %d on line %d\n", retval, __LINE__);\
			goto cleanup;\
		}\
		if (i != rank_expected) {\
			fprintf(stderr, "Expected score to be %d, got %ld on line %d\n", rank_expected, i, __LINE__);\
			retval = RL_UNEXPECTED;\
			goto cleanup;\
		}\
	}

	run_test_zremrangebyscore(0, 0, 100, 1, 10, 19, 9);
	run_test_zremrangebyscore(0, 0, 100, 1, 0, 19, 9);
	run_test_zremrangebyscore(-INFINITY, 0, 100, 0, 1, 19, 8);
	run_test_zremrangebyscore(-INFINITY, 0, 100, 0, 0, 19, 8);
	run_test_zremrangebyscore(180, 0, INFINITY, 0, 2, 17, 6);
	run_test_zremrangebyscore(-INFINITY, 0, INFINITY, 0, 7, 0, -1);

	fprintf(stderr, "End basic_test_zadd_zremrangebyscore\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

int basic_test_zadd_zremrangebylex(int _commit)
{
#define ZRANGEBYLEX_SIZE 20
	int retval = 0;
	fprintf(stderr, "Start basic_test_zadd_zremrangebylex%d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);

	unsigned char *key = (unsigned char *)"my key";
	long keylen = strlen((char *)key);

	unsigned char data[2];
	long i;
	for (i = 0; i < ZRANGEBYLEX_SIZE; i++) {
		data[0] = 'a' + (i / 2);
		data[1] = 'A' + i;
		RL_CALL_VERBOSE(rl_zadd, RL_OK, db, key, keylen, 1.0, data, ((i & 1) == 0) ? 1 : 2);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	unsigned char min[3];
	unsigned char max[3];

	long changed;
#define run_remrangebylex(min, max, expected_changed)\
	retval = rl_zremrangebylex(db, key, keylen, (unsigned char *)min, strlen(min), (unsigned char *)max, strlen(max), &changed);\
	if (retval != RL_OK) {\
		fprintf(stderr, "Failed rl_zremrangebylex on line %d, got %d\n", __LINE__, retval);\
		goto cleanup;\
	}\
	if (changed != expected_changed) {\
		fprintf(stderr, "Expected to change %d but changed %ld on line %d\n", expected_changed, changed, __LINE__);\
		retval = RL_UNEXPECTED;\
		goto cleanup;\
	}\
	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	run_remrangebylex("-", "(a", 0);
	run_test_zrangebylex('-', 0, 1, '+', 0, 1, 0, ZRANGEBYLEX_SIZE, ZRANGEBYLEX_SIZE, 0, 0)
	run_remrangebylex("-", "[a", 1);
	run_test_zrangebylex('-', 0, 1, '+', 0, 1, 1, ZRANGEBYLEX_SIZE - 1, ZRANGEBYLEX_SIZE, 0, 0)
	run_remrangebylex("(a", "[b", 2);
	run_test_zrangebylex('-', 0, 1, '+', 0, 1, 3, ZRANGEBYLEX_SIZE - 3, ZRANGEBYLEX_SIZE, 0, 0)

	fprintf(stderr, "End basic_test_zadd_zremrangebylex\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

#define ZINTERSTORE_TESTS 7
RL_TEST_MAIN_START(type_zset_test)
{
	long zinterunionstore_tests[ZINTERSTORE_TESTS][5] = {
		{RL_ZSET_AGGREGATE_SUM, 0, 0, 0, 6},
		{RL_ZSET_AGGREGATE_SUM, 1, 1, 1, 6},
		{RL_ZSET_AGGREGATE_MIN, 1, 1, 1, 1},
		{RL_ZSET_AGGREGATE_MAX, 1, 1, 1, 3},
		{RL_ZSET_AGGREGATE_SUM, 5, 1, 1, 10},
		{RL_ZSET_AGGREGATE_MIN, 5, 1, 1, 2},
		{RL_ZSET_AGGREGATE_MAX, 5, 1, 1, 5},
	};
	int i, j;
	for (i = 0; i < 2; i++) {
		RL_TEST(basic_test_zadd_zscore, i);
		RL_TEST(basic_test_zadd_zscore2, i);
		RL_TEST(basic_test_zadd_zrank, i);
		RL_TEST(basic_test_zadd_zrem, i);
		RL_TEST(basic_test_zadd_zcount, i);
		RL_TEST(basic_test_zadd_zincrby, i);
		RL_TEST(basic_test_zadd_zrangebylex, i);
		RL_TEST(basic_test_zadd_zrangebyscore, i);
		RL_TEST(basic_test_zadd_zremrangebyrank, i);
		RL_TEST(basic_test_zadd_zremrangebyscore, i);
		RL_TEST(basic_test_zadd_zremrangebylex, i);
		RL_TEST(basic_test_zadd_dupe, i);
		for (j = 0; j < ZINTERSTORE_TESTS; j++) {
			RL_TEST(basic_test_zadd_zinterstore, i, zinterunionstore_tests[j]);
			RL_TEST(basic_test_zadd_zunionstore, i, zinterunionstore_tests[j]);
		}
	}
	RL_TEST(basic_test_zadd_zrange, 0);
}
RL_TEST_MAIN_END
