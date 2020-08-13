#include <float.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include "rlite/rlite.h"
#include "rlite/page_multi_string.h"
#include "rlite/type_hash.h"
#include "rlite/page_btree.h"
#include "rlite/util.h"

static int rl_hash_create(rlite *db, int64_t btree_page, rl_btree **btree)
{
	rl_btree *hash = NULL;

	int retval;
	RL_CALL(rl_btree_create, RL_OK, db, &hash, &rl_btree_type_hash_sha1_hashkey);
	RL_CALL(rl_write, RL_OK, db, &rl_data_type_btree_hash_sha1_hashkey, btree_page, hash);

	if (btree) {
		*btree = hash;
	}
cleanup:
	return retval;
}

static int rl_hash_read(rlite *db, int64_t hash_page_number, rl_btree **btree)
{
	void *tmp;
	int retval;
	RL_CALL(rl_read, RL_FOUND, db, &rl_data_type_btree_hash_sha1_hashkey, hash_page_number, &rl_btree_type_hash_sha1_hashkey, &tmp, 1);
	*btree = tmp;
	retval = RL_OK;
cleanup:
	return retval;
}

static int rl_hash_get_objects(rlite *db, const unsigned char *key, int64_t keylen, int64_t *_hash_page_number, rl_btree **btree, int update_version, int create)
{
	int64_t hash_page_number = 0, version = 0;
	int retval;
	uint64_t expires = 0;
	if (create) {
		retval = rl_key_get_or_create(db, key, keylen, RL_TYPE_HASH, &hash_page_number, &version);
		if (retval != RL_FOUND && retval != RL_NOT_FOUND) {
			goto cleanup;
		}
		else if (retval == RL_NOT_FOUND) {
			retval = rl_hash_create(db, hash_page_number, btree);
			goto cleanup;
		}
		else {
			RL_CALL(rl_hash_read, RL_OK, db, hash_page_number, btree);
		}
	}
	else {
		unsigned char type;
		retval = rl_key_get(db, key, keylen, &type, NULL, &hash_page_number, &expires, &version);
		if (retval != RL_FOUND) {
			goto cleanup;
		}
		if (type != RL_TYPE_HASH) {
			retval = RL_WRONG_TYPE;
			goto cleanup;
		}
		RL_CALL(rl_hash_read, RL_OK, db, hash_page_number, btree);
	}
	if (update_version) {
		RL_CALL(rl_key_set, RL_OK, db, key, keylen, RL_TYPE_HASH, hash_page_number, expires, version + 1);
	}
cleanup:
	if (_hash_page_number) {
		*_hash_page_number = hash_page_number;
	}
	return retval;
}

int rl_hset(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *field, int64_t fieldlen, unsigned char *data, int64_t datalen, int64_t *added, int update)
{
	int retval;
	int64_t hash_page_number;
	rl_btree *hash;
	unsigned char *digest = NULL;
	rl_hashkey *hashkey = NULL;
	int64_t add = 1;
	void *tmp;
	RL_CALL(rl_hash_get_objects, RL_OK, db, key, keylen, &hash_page_number, &hash, 1, 1);

	RL_MALLOC(digest, sizeof(unsigned char) * 20);
	RL_CALL(sha1, RL_OK, field, fieldlen, digest);

	retval = rl_btree_find_score(db, hash, digest, &tmp, NULL, NULL);
	if (retval == RL_FOUND) {
		add = 0;
		if (!update) {
			goto cleanup;
		}
		hashkey = tmp;
		rl_multi_string_delete(db, hashkey->string_page);
		rl_multi_string_delete(db, hashkey->value_page);
	}

	RL_MALLOC(hashkey, sizeof(*hashkey));
	RL_CALL(rl_multi_string_set, RL_OK, db, &hashkey->string_page, field, fieldlen);
	RL_CALL(rl_multi_string_set, RL_OK, db, &hashkey->value_page, data, datalen);
	if (update) {
		retval = rl_btree_update_element(db, hash, digest, hashkey);
		if (retval == RL_OK) {
			add = 0;
			rl_free(digest);
			digest = NULL;
		}
		else if (retval != RL_NOT_FOUND) {
			goto cleanup;
		}
	}
	if (add) {
		retval = rl_btree_add_element(db, hash, hash_page_number, digest, hashkey);
		if (retval == RL_FOUND) {
			add = 0;
		}
		if (retval != RL_OK) {
			goto cleanup;
		}
	}
	retval = RL_OK;
cleanup:
	if (added) {
		*added = add;
	}
	if (retval != RL_OK) {
		rl_free(digest);
		rl_free(hashkey);
	}
	return retval;
}

int rl_hget(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *field, int64_t fieldlen, unsigned char **data, int64_t *datalen)
{
	int retval;
	int64_t hash_page_number;
	rl_btree *hash;
	void *tmp;
	unsigned char *digest = NULL;
	rl_hashkey *hashkey;
	RL_CALL(rl_hash_get_objects, RL_OK, db, key, keylen, &hash_page_number, &hash, 0, 0);

	RL_MALLOC(digest, sizeof(unsigned char) * 20);
	RL_CALL(sha1, RL_OK, field, fieldlen, digest);

	retval = rl_btree_find_score(db, hash, digest, &tmp, NULL, NULL);
	if (retval == RL_FOUND) {
		if (data || datalen) {
			hashkey = tmp;
			rl_multi_string_get(db, hashkey->value_page, data, datalen);
		}
	}
cleanup:
	rl_free(digest);
	return retval;
}

int rl_hmget(struct rlite *db, const unsigned char *key, int64_t keylen, int fieldc, unsigned char **fields, int64_t *fieldslen, unsigned char ***_data, int64_t **_datalen)
{
	int retval;
	int64_t hash_page_number;
	rl_btree *hash;
	void *tmp;
	unsigned char *digest = NULL;
	rl_hashkey *hashkey;

	unsigned char **data = rl_malloc(sizeof(unsigned char *) * fieldc);
	int64_t *datalen = rl_malloc(sizeof(int64_t) * fieldc);
	RL_CALL(rl_hash_get_objects, RL_OK, db, key, keylen, &hash_page_number, &hash, 0, 0);

	RL_MALLOC(digest, sizeof(unsigned char) * 20);
	int i;
	for (i = 0; i < fieldc; i++) {
		RL_CALL(sha1, RL_OK, fields[i], fieldslen[i], digest);

		retval = rl_btree_find_score(db, hash, digest, &tmp, NULL, NULL);
		if (retval == RL_FOUND) {
			hashkey = tmp;
			rl_multi_string_get(db, hashkey->value_page, &data[i], &datalen[i]);
		}
		else if (retval == RL_NOT_FOUND) {
			data[i] = NULL;
			datalen[i] = -1;
		}
		else {
			goto cleanup;
		}
	}
	*_data = data;
	*_datalen = datalen;
	retval = RL_OK;
cleanup:
	if (retval != RL_OK) {
		rl_free(data);
		rl_free(datalen);
	}
	rl_free(digest);
	return retval;
}

int rl_hmset(struct rlite *db, const unsigned char *key, int64_t keylen, int fieldc, unsigned char **fields, int64_t *fieldslen, unsigned char **datas, int64_t *dataslen)
{
	int i, retval;
	int64_t hash_page_number;
	rl_btree *hash;
	unsigned char *digest = NULL;
	rl_hashkey *hashkey = NULL;
	void *tmp;
	RL_CALL(rl_hash_get_objects, RL_OK, db, key, keylen, &hash_page_number, &hash, 1, 1);

	for (i = 0; i < fieldc; i++) {
		RL_MALLOC(digest, sizeof(unsigned char) * 20);
		RL_CALL(sha1, RL_OK, fields[i], fieldslen[i], digest);

		retval = rl_btree_find_score(db, hash, digest, &tmp, NULL, NULL);
		if (retval == RL_FOUND) {
			hashkey = tmp;
			rl_multi_string_delete(db, hashkey->value_page);
			RL_CALL(rl_multi_string_set, RL_OK, db, &hashkey->value_page, datas[i], dataslen[i]);
			retval = rl_btree_update_element(db, hash, digest, hashkey);
			if (retval == RL_OK) {
				rl_free(digest);
				digest = NULL;
			}
			else {
				goto cleanup;
			}
		}
		else if (retval == RL_NOT_FOUND) {
			RL_MALLOC(hashkey, sizeof(*hashkey));
			RL_CALL(rl_multi_string_set, RL_OK, db, &hashkey->string_page, fields[i], fieldslen[i]);
			RL_CALL(rl_multi_string_set, RL_OK, db, &hashkey->value_page, datas[i], dataslen[i]);

			retval = rl_btree_add_element(db, hash, hash_page_number, digest, hashkey);
			if (retval != RL_FOUND && retval != RL_OK) {
				goto cleanup;
			}
		}
	}
	retval = RL_OK;
cleanup:
	if (retval != RL_OK) {
		rl_free(digest);
		rl_free(hashkey);
	}
	return retval;
}

int rl_hexists(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *field, int64_t fieldlen)
{
	int retval;
	int64_t hash_page_number;
	rl_btree *hash;
	unsigned char *digest = NULL;
	RL_CALL(rl_hash_get_objects, RL_OK, db, key, keylen, &hash_page_number, &hash, 0, 0);

	RL_MALLOC(digest, sizeof(unsigned char) * 20);
	RL_CALL(sha1, RL_OK, field, fieldlen, digest);

	retval = rl_btree_find_score(db, hash, digest, NULL, NULL, NULL);
cleanup:
	rl_free(digest);
	return retval;
}

int rl_hdel(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t fieldsc, unsigned char **fields, int64_t *fieldslen, int64_t *delcount)
{
	int retval;
	int64_t hash_page_number;
	rl_btree *hash;
	rl_hashkey *hashkey;
	void *tmp;
	int64_t i;
	int64_t deleted = 0;
	int keydeleted = 0;
	unsigned char *digest = NULL;
	RL_CALL(rl_hash_get_objects, RL_OK, db, key, keylen, &hash_page_number, &hash, 1, 0);
	RL_MALLOC(digest, sizeof(unsigned char) * 20);

	for (i = 0; i < fieldsc; i++) {
		RL_CALL(sha1, RL_OK, fields[i], fieldslen[i], digest);
		retval = rl_btree_find_score(db, hash, digest, &tmp, NULL, NULL);
		if (retval == RL_FOUND) {
			deleted++;
			hashkey = tmp;
			rl_multi_string_delete(db, hashkey->string_page);
			rl_multi_string_delete(db, hashkey->value_page);
			retval = rl_btree_remove_element(db, hash, hash_page_number, digest);
			if (retval != RL_OK && retval != RL_DELETED) {
				goto cleanup;
			}
			if (retval == RL_DELETED) {
				keydeleted = 1;
				break;
			}
		}
	}
	if (delcount) {
		*delcount = deleted;
	}
	if (keydeleted) {
		RL_CALL(rl_key_delete, RL_OK, db, key, keylen);
	}
	retval = RL_OK;
cleanup:
	rl_free(digest);
	return retval;
}

int rl_hgetall(struct rlite *db, rl_hash_iterator **iterator, const unsigned char *key, int64_t keylen)
{
	int retval;
	rl_btree *hash;
	RL_CALL(rl_hash_get_objects, RL_OK, db, key, keylen, NULL, &hash, 0, 0);
	RL_CALL(rl_btree_iterator_create, RL_OK, db, hash, iterator);
cleanup:
	return retval;
}

int rl_hlen(struct rlite *db, const unsigned char *key, int64_t keylen, int64_t *len)
{
	int retval;
	rl_btree *hash;
	RL_CALL(rl_hash_get_objects, RL_OK, db, key, keylen, NULL, &hash, 0, 0);
	*len = hash->number_of_elements;
	retval = RL_OK;
cleanup:
	return retval;
}

int rl_hincrby(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *field, int64_t fieldlen, int64_t increment, int64_t *newvalue)
{
	int retval;
	rl_btree *hash;
	void *tmp;
	unsigned char *digest = NULL, *data = NULL;
	char *end;
	int64_t datalen, hash_page_number;
	int64_t value;
	rl_hashkey *hashkey;

	RL_CALL(rl_hash_get_objects, RL_OK, db, key, keylen, &hash_page_number, &hash, 1, 1);

	RL_MALLOC(digest, sizeof(unsigned char) * 20);
	RL_CALL(sha1, RL_OK, field, fieldlen, digest);

	retval = rl_btree_find_score(db, hash, digest, &tmp, NULL, NULL);
	if (retval == RL_FOUND) {
		hashkey = tmp;
		rl_multi_string_get(db, hashkey->value_page, &data, &datalen);
		tmp = rl_realloc(data, sizeof(unsigned char) * (datalen + 1));
		if (!tmp) {
			retval = RL_OUT_OF_MEMORY;
			goto cleanup;
		}
		data = tmp;
		data[datalen] = '\0';
		value = strtoll((char *)data, &end, 10);
		if (isspace(((char *)data)[0]) || end[0] != '\0' || errno == ERANGE) {
			rl_free(digest);
			retval = RL_NAN;
			goto cleanup;
		}
		rl_free(data);
		data = NULL;

		if ((increment < 0 && value < 0 && increment < (LLONG_MIN - value)) ||
		        (increment > 0 && value > 0 && increment > (LLONG_MAX - value))) {
			rl_free(digest);
			retval = RL_OVERFLOW;
			goto cleanup;
		}

		value += increment;
		rl_multi_string_delete(db, hashkey->value_page);

		RL_MALLOC(data, sizeof(unsigned char) * MAX_LLONG_DIGITS);
		datalen = snprintf((char *)data, MAX_LLONG_DIGITS, "%" PRId64 "", value);
		RL_CALL(rl_multi_string_set, RL_OK, db, &hashkey->value_page, data, datalen);

		retval = rl_btree_update_element(db, hash, digest, hashkey);
		if (retval == RL_OK) {
			rl_free(digest);
			digest = NULL;
			if (newvalue) {
				*newvalue = value;
			}
		}
		else if (retval != RL_NOT_FOUND) {
			goto cleanup;
		}
	}
	else if (retval == RL_NOT_FOUND) {
		RL_MALLOC(data, sizeof(unsigned char) * MAX_LLONG_DIGITS);
		datalen = snprintf((char *)data, MAX_LLONG_DIGITS, "%" PRId64 "", increment);

		RL_MALLOC(hashkey, sizeof(*hashkey));
		RL_CALL(rl_multi_string_set, RL_OK, db, &hashkey->string_page, field, fieldlen);
		RL_CALL(rl_multi_string_set, RL_OK, db, &hashkey->value_page, data, datalen);
		RL_CALL(rl_btree_add_element, RL_OK, db, hash, hash_page_number, digest, hashkey);
		if (newvalue) {
			*newvalue = increment;
		}
	}
	else {
		goto cleanup;
	}

	retval = RL_OK;
cleanup:
	rl_free(data);
	return retval;
}

int rl_hincrbyfloat(struct rlite *db, const unsigned char *key, int64_t keylen, unsigned char *field, int64_t fieldlen, double increment, double *newvalue)
{
	int retval;
	rl_btree *hash;
	void *tmp;
	unsigned char *digest = NULL, *data = NULL;
	char *end;
	int64_t dataalloc, datalen, hash_page_number;
	double value;
	rl_hashkey *hashkey;

	RL_CALL(rl_hash_get_objects, RL_OK, db, key, keylen, &hash_page_number, &hash, 1, 1);

	RL_MALLOC(digest, sizeof(unsigned char) * 20);
	RL_CALL(sha1, RL_OK, field, fieldlen, digest);

	retval = rl_btree_find_score(db, hash, digest, &tmp, NULL, NULL);
	if (retval == RL_FOUND) {
		hashkey = tmp;
		rl_multi_string_get(db, hashkey->value_page, &data, &datalen);
		dataalloc = (datalen / 8 + 1) * 8;
		tmp = rl_realloc(data, sizeof(unsigned char) * dataalloc);
		if (!tmp) {
			retval = RL_OUT_OF_MEMORY;
			goto cleanup;
		}
		data = tmp;
		// valgrind reads 8 bytes at a time
		memset(&data[datalen], 0, dataalloc - datalen);
		value = strtod((char *)data, &end);
		if (isspace(((char *)data)[0]) || end[0] != '\0' ||
		        (errno == ERANGE && (value == HUGE_VAL || value == -HUGE_VAL || value == 0)) ||
		        errno == EINVAL || isnan(value)) {
			rl_free(digest);
			retval = RL_NAN;
			goto cleanup;
		}
		rl_free(data);
		data = NULL;
		value += increment;
		rl_multi_string_delete(db, hashkey->value_page);

		RL_MALLOC(data, sizeof(unsigned char) * MAX_DOUBLE_DIGITS);
		datalen = snprintf((char *)data, MAX_DOUBLE_DIGITS, "%lf", value);
		RL_CALL(rl_multi_string_set, RL_OK, db, &hashkey->value_page, data, datalen);

		retval = rl_btree_update_element(db, hash, digest, hashkey);
		if (retval == RL_OK) {
			rl_free(digest);
			digest = NULL;
			if (newvalue) {
				*newvalue = value;
			}
		}
		else if (retval != RL_NOT_FOUND) {
			goto cleanup;
		}
	}
	else if (retval == RL_NOT_FOUND) {
		RL_MALLOC(data, sizeof(unsigned char) * MAX_DOUBLE_DIGITS);
		datalen = snprintf((char *)data, MAX_DOUBLE_DIGITS, "%lf", increment);

		RL_MALLOC(hashkey, sizeof(*hashkey));
		RL_CALL(rl_multi_string_set, RL_OK, db, &hashkey->string_page, field, fieldlen);
		RL_CALL(rl_multi_string_set, RL_OK, db, &hashkey->value_page, data, datalen);
		RL_CALL(rl_btree_add_element, RL_OK, db, hash, hash_page_number, digest, hashkey);
		if (newvalue) {
			*newvalue = increment;
		}
	}
	else {
		goto cleanup;
	}

	retval = RL_OK;
cleanup:
	rl_free(data);
	return retval;
}

int rl_hash_iterator_next(rl_hash_iterator *iterator, int64_t *fieldpage, unsigned char **field, int64_t *fieldlen, int64_t *memberpage, unsigned char **member, int64_t *memberlen)
{
	void *tmp;
	rl_hashkey *hashkey = NULL;
	int retval;
	if (member && !memberlen) {
		fprintf(stderr, "If member is provided, memberlen is required\n");
		return RL_UNEXPECTED;
	}

	if (field && !fieldlen) {
		fprintf(stderr, "If field is provided, fieldlen is required\n");
		return RL_UNEXPECTED;
	}

	RL_CALL(rl_btree_iterator_next, RL_OK, iterator, NULL, &tmp);
	hashkey = tmp;

	if (fieldpage) {
		*fieldpage = hashkey->string_page;
	}
	if (fieldlen) {
		retval = rl_multi_string_get(iterator->db, hashkey->string_page, field, fieldlen);
		if (retval != RL_OK) {
			rl_btree_iterator_destroy(iterator);
			goto cleanup;
		}
	}

	if (memberpage) {
		*memberpage = hashkey->value_page;
	}
	if (memberlen) {
		retval = rl_multi_string_get(iterator->db, hashkey->value_page, member, memberlen);
		if (retval != RL_OK) {
			rl_btree_iterator_destroy(iterator);
			goto cleanup;
		}
	}
cleanup:
	rl_free(hashkey);
	return retval;
}

int rl_hash_iterator_destroy(rl_hash_iterator *iterator)
{
	return rl_btree_iterator_destroy(iterator);
}

int rl_hash_pages(struct rlite *db, int64_t page, short *pages)
{
	rl_btree *btree;
	rl_btree_iterator *iterator = NULL;
	int retval;
	void *tmp;
	rl_hashkey *hashkey;

	RL_CALL(rl_read, RL_FOUND, db, &rl_data_type_btree_hash_sha1_hashkey, page, &rl_btree_type_hash_sha1_hashkey, &tmp, 1);
	btree = tmp;

	RL_CALL(rl_btree_pages, RL_OK, db, btree, pages);

	RL_CALL(rl_btree_iterator_create, RL_OK, db, btree, &iterator);
	while ((retval = rl_btree_iterator_next(iterator, NULL, &tmp)) == RL_OK) {
		hashkey = tmp;
		pages[hashkey->value_page] = 1;
		RL_CALL(rl_multi_string_pages, RL_OK, db, hashkey->value_page, pages);
		pages[hashkey->string_page] = 1;
		RL_CALL(rl_multi_string_pages, RL_OK, db, hashkey->string_page, pages);
		rl_free(hashkey);
	}
	iterator = NULL;

	if (retval != RL_END) {
		goto cleanup;
	}
	retval = RL_OK;
cleanup:
	if (retval != RL_OK) {
		if (iterator) {
			rl_btree_iterator_destroy(iterator);
		}
	}
	return retval;
}

int rl_hash_delete(rlite *db, int64_t value_page)
{
	rl_btree *hash;
	rl_btree_iterator *iterator;
	rl_hashkey *hashkey = NULL;
	int retval;
	void *tmp;
	RL_CALL(rl_read, RL_FOUND, db, &rl_data_type_btree_hash_sha1_hashkey, value_page, &rl_btree_type_hash_sha1_hashkey, &tmp, 1);
	hash = tmp;
	RL_CALL(rl_btree_iterator_create, RL_OK, db, hash, &iterator);
	while ((retval = rl_btree_iterator_next(iterator, NULL, &tmp)) == RL_OK) {
		hashkey = tmp;
		rl_multi_string_delete(db, hashkey->string_page);
		rl_multi_string_delete(db, hashkey->value_page);
		rl_free(hashkey);
	}
	iterator = NULL;

	if (retval != RL_END) {
		goto cleanup;
	}

	RL_CALL(rl_btree_delete, RL_OK, db, hash);
	RL_CALL(rl_delete, RL_OK, db, value_page);
	retval = RL_OK;
cleanup:
	return retval;
}
