#include <stdio.h>

#include "packman.h"

static void example_print_hex(const unsigned char *buf, size_t len)
{
	unsigned i;

	for (i = 0; i < len; i++)
		printf("%02x ", buf[i]);
}

static struct packman_encoding_int example_le16_encoding = {
	.base = {
		.type = PACKMAN_ENCODING_INT,
		.elem_size = sizeof(uint16_t),
	},
	.desc = {
		.size = sizeof(uint16_t),
		.endianness = PACKMAN_ENDIANNESS_LITTLE,
	},
};

static struct packman_encoding_int example_be16_encoding = {
	.base = {
		.type = PACKMAN_ENCODING_INT,
		.elem_size = sizeof(uint16_t),
	},
	.desc = {
		.size = sizeof(uint16_t),
		.endianness = PACKMAN_ENDIANNESS_BIG,
	},
};

static void example_encode_int(void)
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

static void example_encode_struct(void)
{
	struct example_struct x;
	unsigned char buf[sizeof(x)];

	x.be = 0x1234;
	x.le = 0x5678;
	packman_encode(&example_struct_encoding.base, &x, buf, sizeof(buf));
	printf("struct with be 0x1234 and le 0x5678: ");
	example_print_hex(buf, sizeof(buf));
	printf("\n");
}

static void example_decode_struct(void)
{
	struct example_struct x;
	unsigned char buf[sizeof(x)] = { 0x01, 0x02, 0x03, 0x04, };
	
	packman_decode(&example_struct_encoding.base, &x, buf, sizeof(buf));
	printf("buf { ");
	example_print_hex(buf, sizeof(buf));
	printf("} to struct with be16 and le16: 0x%04x, 0x%04x\n", x.be, x.le);
}

int main()
{
	printf("host is %ce\n", (htole16(0x1234) == 0x1234) ? 'l' : 'b');
	example_encode_int();
	example_encode_struct();
	example_decode_struct();
	return 0;
}
