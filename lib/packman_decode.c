#include "packman_internal.h"

typedef enum packman_error (*packman_decoding_fn)(
		struct packman_encoding_base *,
		void *, const void **, size_t *);

static enum packman_error __packman_decode(struct packman_encoding_base *,
		void *, const void **, size_t *);

static enum packman_error __packman_decode_int(
		struct packman_int_description *desc,
		void *obj, const void **buf, size_t *count)
{
	enum packman_error ret;

	ret = packman_do_int_desc(desc, obj, *buf, count);
	*buf += desc->size;
	return ret;
}

static enum packman_error packman_decode_int(
		struct packman_encoding_base *base,
		void *obj, const void **buf, size_t *count)
{
	struct packman_encoding_int *encoding = (void *)base;

	return __packman_decode_int(&encoding->desc, obj, buf, count);
}

static enum packman_error packman_decode_struct(
		struct packman_encoding_base *base,
		void *obj, const void **buf, size_t *count)
{
	struct packman_encoding_struct *encoding = (void *)base;
	unsigned i;
	enum packman_error err;

	for (i = 0; i < encoding->nfields; i++) {
		err = __packman_decode(encoding->fields[i], obj, buf, count);
		if (err)
			return err;
		obj += base->elem_size;
	}
	return PACKMAN_ERROR_SUCCESS;
}

static const packman_decoding_fn packman_decode_fns[] = {
	[PACKMAN_ENCODING_INT] = packman_decode_int,
	[PACKMAN_ENCODING_STRUCT] = packman_decode_struct,
};

static enum packman_error __packman_decode(struct packman_encoding_base *base,
		void *obj, const void **buf, size_t *count)
{
	if (base->type >= PACKMAN_ENCODING_MAX_VALUE)
		return PACKMAN_ERROR_NO_SUCH_ENCODING_TYPE;
	return packman_decode_fns[base->type](base, obj, buf, count);
}

enum packman_error packman_decode(struct packman_encoding_base *base,
		void *obj, const void *buf, size_t count)
{
	return __packman_decode(base, obj, &buf, &count);
}
