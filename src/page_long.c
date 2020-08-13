#include <stdlib.h>
#include <string.h>
#include "rlite/rlite.h"
#include "rlite/page_long.h"
#include "rlite/util.h"

int rl_long_serialize(rlite *UNUSED(db), void *obj, unsigned char *data)
{
	put_4bytes(data, *(int64_t *)obj);
	return RL_OK;
}

int rl_long_deserialize(rlite *UNUSED(db), void **obj, void *UNUSED(context), unsigned char *data)
{
	int retval;
	int64_t *value;
	RL_MALLOC(value, sizeof(int64_t));
	*value = get_4bytes(data);
	*obj = value;
	retval = RL_OK;
cleanup:
	return retval;
}

int rl_long_destroy(rlite *UNUSED(db), void *obj)
{
	rl_free(obj);
	return RL_OK;
}

int rl_long_set(rlite *db, int64_t value, int64_t number)
{
	int retval;
	int64_t *val;
	RL_MALLOC(val, sizeof(*val));
	*val = value;
	RL_CALL(rl_write, RL_OK, db, &rl_data_type_long, number, val);

	retval = RL_OK;
cleanup:
	return retval;
}

int rl_long_create(struct rlite *db, int64_t value, int64_t *number)
{
	if (number) {
		*number = db->next_empty_page;
	}
	return rl_long_set(db, value, db->next_empty_page);
}

int rl_long_get(rlite *db, int64_t *value, int64_t number)
{
	void *data;
	int retval;
	RL_CALL(rl_read, RL_FOUND, db, &rl_data_type_long, number, NULL, &data, 1);
	*value = *(int64_t *)data;
	retval = RL_OK;
cleanup:
	return retval;
}
