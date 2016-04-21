#include "packman.h"

typedef enum packman_error (*packman_decoding_fn)(
		struct packman_encoding_base *,
		void *, const void **, size_t *);

static enum packman_error __packman_decode(struct packman_encoding_base *,
		void *, const void **, size_t *);

static enum packman_error packman_decode_int(
		struct packman_encoding_base *base,
		void *obj, const void **buf, size_t *count)
{
	struct packman_encoding_int *encoding = (void *)base;

	if (*count < encoding->size)
		return PACKMAN_ERROR_BUF_TOO_SMALL;
	switch (encoding->size) {
	case sizeof(uint8_t):
		*(uint8_t *)obj = *(const uint8_t *)*buf;
		break;
	#define PACKMAN_DECODE_INT_CASE(n) \
	case sizeof(uint##n##_t): { \
		uint##n##_t value = *(const uint##n##_t *)*buf; \
		uint##n##_t *target = obj; \
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
	PACKMAN_DECODE_INT_CASE(16);
	PACKMAN_DECODE_INT_CASE(32);
	PACKMAN_DECODE_INT_CASE(64);
	}
	*buf += encoding->size;
	*count -= encoding->size;
	return PACKMAN_ERROR_SUCCESS;
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
