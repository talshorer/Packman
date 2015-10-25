#include <stdio.h>

#include "packman.h"

static struct packman_encoding_int example_le16_encoding = {
	.base = {
		.type = PACKMAN_ENCODING_INT,
		.elem_size = sizeof(uint16_t),
	},
	.size = sizeof(uint16_t),
	.endianness = PACKMAN_ENDIANNESS_LITTLE,
};

static struct packman_encoding_int example_be16_encoding = {
	.base = {
		.type = PACKMAN_ENCODING_INT,
		.elem_size = sizeof(uint16_t),
	},
	.size = sizeof(uint16_t),
	.endianness = PACKMAN_ENDIANNESS_BIG,
};

static void example_int(void)
{
	uint16_t x = 0xf00d;
	char buf[sizeof(x)];
	printf("h  %04x\n", x);
	packman_encode(&example_le16_encoding.base, &x, buf, sizeof(buf));
	printf("le %04x\n", *(uint16_t *)buf);
	packman_encode(&example_be16_encoding.base, &x, buf, sizeof(buf));
	printf("be %04x\n", *(uint16_t *)buf);
}

__attribute__((__packed__)) struct example_struct {
	uint16_t be;
	uint16_t le;
};

static struct packman_encoding_base *__example_struct_encoding_fields[] = {
	&example_be16_encoding.base,
	&example_le16_encoding.base,
};

static struct packman_encoding_struct example_struct_encoding = {
	.base = {
		.type = PACKMAN_ENCODING_STRUCT,
		.elem_size = sizeof(uint16_t),
	},
	.fields = __example_struct_encoding_fields,
	.nfields = 2,
};

static void example_struct(void)
{
	struct example_struct x;
	x.be = 0x1234;
	x.le = 0x5678;
	unsigned char buf[sizeof(x)];
	unsigned i;
	packman_encode(&example_struct_encoding.base, &x, buf, sizeof(buf));
	printf("struct ");
	for (i = 0; i < sizeof(buf); i++)
		printf("%02x ", buf[i]);
	printf("\n");
}

int main()
{
	printf("host is %ce\n", htole16(0x1234) == 0x1234 ? 'l' : 'b');
	example_int();
	example_struct();
	return 0;
}
