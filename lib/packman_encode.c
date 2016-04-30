#include "packman_internal.h"

typedef enum packman_error (*packman_encoding_fn)(
		struct packman_encoding_base *,
		const void *, void **, size_t *);

static enum packman_error __packman_encode(struct packman_encoding_base *,
		const void *, void **, size_t *);

static enum packman_error __packman_encode_int(
		struct packman_int_description *desc,
		const void *obj, void **buf, size_t *count)
{
	enum packman_error ret;

	ret = packman_do_int_desc(desc, *buf, obj, count);
	*buf += desc->size;
	return ret;
}

static enum packman_error packman_encode_int(
		struct packman_encoding_base *base,
		const void *obj, void **buf, size_t *count)
{
	struct packman_encoding_int *encoding = (void *)base;

	return __packman_encode_int(&encoding->desc, obj, buf, count);
}

static enum packman_error packman_encode_struct(
		struct packman_encoding_base *base,
		const void *obj, void **buf, size_t *count)
{
	struct packman_encoding_struct *encoding = (void *)base;
	unsigned i;
	enum packman_error err;

	for (i = 0; i < encoding->nfields; i++) {
		err = __packman_encode(encoding->fields[i], obj, buf, count);
		if (err)
			return err;
		obj += base->elem_size;
	}
	return PACKMAN_ERROR_SUCCESS;
}

static const packman_encoding_fn packman_encode_fns[] = {
	[PACKMAN_ENCODING_INT] = packman_encode_int,
	[PACKMAN_ENCODING_STRUCT] = packman_encode_struct,
};

static enum packman_error __packman_encode(struct packman_encoding_base *base,
		const void *obj, void **buf, size_t *count)
{
	if (base->type >= PACKMAN_ENCODING_MAX_VALUE)
		return PACKMAN_ERROR_NO_SUCH_ENCODING_TYPE;
	return packman_encode_fns[base->type](base, obj, buf, count);
}

enum packman_error packman_encode(struct packman_encoding_base *base,
		const void *obj, void *buf, size_t count)
{
	return __packman_encode(base, obj, &buf, &count);
}
