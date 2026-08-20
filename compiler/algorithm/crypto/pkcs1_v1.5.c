#include "crypto.h"
#include "random.h"

uint8_t	*pkcs1_v1_5_generate_em(t_RSA_public_key *pub, uint8_t *message, size_t len, size_t *out_len)
{
	if (!pub || !out_len || (!message && len != 0))
		return 0;

	size_t bits = big_int_bit_length(&pub->n);
	size_t k = (bits + 7) / 8;

	if (len > k - 11)
		return 0;

	uint8_t *buff = kmalloc(sizeof(uint8_t) * k);
	if (!buff)
		return 0;

	size_t padding_length = k - len - 3;
	// EM = 00 || 02 || PS || 00 || M
	buff[0] = 0x00;
	buff[1] = 0x02;
	for (size_t i = 0; i < padding_length; i++)
	{
		uint8_t b = 0;
		while (b == 0)
			b = (uint8_t)(random_u32() & 0xFF);
		buff[2 + i] = b;
	}
	buff[2 + padding_length] = 0x00;

	if (len > 0)
		memcpy(&buff[3 + padding_length], message, len);

	*out_len = k;
	return buff;
}

uint8_t	*pkcs1_v1_5_decode_em(uint8_t *em, size_t len, size_t *out_len)
{
	size_t i;

	if (!em || !out_len)
		return 0;

	*out_len = 0;

	if (len < 11)
		return 0;

	// check block type
	if (em[0] != 0x00 || em[1] != 0x02)
		return 0;

	// find separator byte after PS
	i = 2;
	while (i < len && em[i] != 0x00)
		i++;

	if (i == len)
		return 0;

	// PS >= 8 bytes
	if (i < 10)
		return 0;

	*out_len = len - (i + 1);
	return &em[i + 1];
}