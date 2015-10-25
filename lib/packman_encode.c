#include "packman.h"

typedef enum packman_error (*packman_encoding_fn)(
		struct packman_encoding_base *,
		const void *, void **, size_t *);

static enum packman_error __packman_encode(struct packman_encoding_base *,
		const void *, void **, size_t *);

static enum packman_error packman_encode_int(
		struct packman_encoding_base *base,
		const void *obj, void **out, size_t *count)
{
	struct packman_encoding_int *encoding = (void *)base;

	if (*count < encoding->size)
		return PACKMAN_ERROR_BUF_TOO_SMALL;
	switch (encoding->size) {
	case sizeof(uint8_t):
		*(uint8_t *)*out = *(const uint8_t *)obj;
		break;
	#define PACKMAN_ENCODE_INT_CASE(n) \
	case sizeof(uint##n##_t): { \
		uint##n##_t value = *(const uint##n##_t *)obj; \
		uint##n##_t *target = *out; \
		switch (encoding->endianness) { \
		case PACKMAN_ENDIANNESS_BIG: \
			*target = htobe##n(value); \
			break; \
		case PACKMAN_ENDIANNESS_LITTLE: \
			*target = htole##n(value); \
			break; \
		} \
		} \
		break
	PACKMAN_ENCODE_INT_CASE(16);
	PACKMAN_ENCODE_INT_CASE(32);
	PACKMAN_ENCODE_INT_CASE(64);
	}
	*out += encoding->size;
	*count -= encoding->size;
	return PACKMAN_ERROR_SUCCESS;
}

static enum packman_error packman_encode_struct(
		struct packman_encoding_base *base,
		const void *obj, void **out, size_t *count)
{
	struct packman_encoding_struct *encoding = (void *)base;
	unsigned i;
	enum packman_error err;

	for (i = 0; i < encoding->nfields; i++) {
		err = __packman_encode(encoding->fields[i], obj, out, count);
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
		const void *obj, void **out, size_t *count)
{
	if (base->type >= PACKMAN_ENCODING_MAX_VALUE)
		return PACKMAN_ERROR_NO_SUCH_ENCODING_TYPE;
	return packman_encode_fns[base->type](base, obj, out, count);
}

enum packman_error packman_encode(struct packman_encoding_base *base,
		const void *obj, void *out, size_t count)
{
	return __packman_encode(base, obj, &out, &count);
}
