#ifndef __PACKMAN_INTERNAL_H
#define __PACKMAN_INTERNAL_H

#include "packman.h"

extern enum packman_error packman_do_int_desc(
		const struct packman_int_description *, void *, const void *,
		size_t *);

extern bool packman_enum_value_valid(const struct packman_encoding_enum *,
		const void *);

#endif /* __PACKMAN_INTERNAL_H */
