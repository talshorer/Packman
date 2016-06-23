#include "packman_internal.h"

typedef enum packman_error (*packman_encoding_fn)(
		const struct packman_encoding_base *,
		const void *, void **, size_t *);

static enum packman_error __packman_encode(const struct packman_encoding_base *,
		const void *, void **, size_t *);

static enum packman_error __packman_encode_int(
		const struct packman_int_description *desc,
		const void *obj, void **buf, size_t *count)
{
	enum packman_error ret;

	ret = packman_do_int_desc(desc, *buf, obj, count);
	*buf += desc->size;
	return ret;
}

static enum packman_error packman_encode_int(
		const struct packman_encoding_base *base,
		const void *obj, void **buf, size_t *count)
{
	const struct packman_encoding_int *encoding = (const void *)base;

	return __packman_encode_int(&encoding->desc, obj, buf, count);
}

static enum packman_error packman_encode_struct(
		const struct packman_encoding_base *base,
		const void *obj, void **buf, size_t *count)
{
	const struct packman_encoding_struct *encoding = (const void *)base;
	unsigned i;
	enum packman_error err;

	for (i = 0; i < encoding->nfields; i++) {
		err = __packman_encode(encoding->fields[i], obj, buf, count);
		if (err)
			return err;
		obj += encoding->fields[i]->elem_size;
	}
	return PACKMAN_ERROR_SUCCESS;
}

static enum packman_error packman_encode_enum(
		const struct packman_encoding_base *base,
		const void *obj, void **buf, size_t *count)
{
	const struct packman_encoding_enum *encoding = (const void *)base;
	enum packman_error err;
	const void *value = obj;

	err = __packman_encode_int(&encoding->desc, obj, buf, count);
	if (err)
		return err;
	if (!packman_enum_value_valid(encoding, value))
		return PACKMAN_ERROR_INVALID_VALUE;
	return PACKMAN_ERROR_SUCCESS;
}

static const packman_encoding_fn packman_encode_fns[] = {
	[PACKMAN_ENCODING_INT] = packman_encode_int,
	[PACKMAN_ENCODING_STRUCT] = packman_encode_struct,
	[PACKMAN_ENCODING_ENUM] = packman_encode_enum,
};

static enum packman_error __packman_encode(
		const struct packman_encoding_base *base, const void *obj,
		void **buf, size_t *count)
{
	if (base->type >= PACKMAN_ENCODING_MAX_VALUE)
		return PACKMAN_ERROR_NO_SUCH_ENCODING_TYPE;
	return packman_encode_fns[base->type](base, obj, buf, count);
}

enum packman_error packman_encode(const struct packman_encoding_base *base,
		const void *obj, void *buf, size_t count)
{
	return __packman_encode(base, obj, &buf, &count);
}
