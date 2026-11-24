#include "stdio.hpp"
#include "crypto.h"

int	main(int argc, const char **argv)
{
	// rsa test
	t_RSA_private_key	prv;
	t_RSA_public_key	pub;
	if (!rsa_private_key_require("/security/private.key", &prv, &pub))
	{
		fprintf(stderr, "Failed to get your RSA keypair!\n");
		return 1;
	}

	size_t	em_len = 0;
	uint8_t	*em = pkcs1_v1_5_generate_em(&pub, (uint8_t *)"Hello World!", 12, &em_len);
	if (!em)
	{
		printf("Failed to encode RSA message\n");
		return 1;
	}
	BigInt *c = rsa_encrypt(em, em_len, &pub);
	kfree(em);
	if (!c)
	{
		printf("Failed to encrypt RSA\n");
		return 1;
	}
	size_t	out_len = 0;
	uint8_t	*m = rsa_decrypt(c, &out_len, &prv);
	kfree(c);
	if (!m)
	{
		printf("Failed to decrypt RSA\n");
		return 1;
	}
	size_t	msg_len = 0;
	uint8_t	*msg = pkcs1_v1_5_decode_em(m, out_len, &msg_len);
	if (!msg)
	{
		printf("Failed to decrypt pkcs#1 v1.5\n");
		kfree(m);
		return 1;
	}

	printf("msg bytes: ");
	for (size_t i = 0; i < msg_len; i++)
		printf("%02X ", msg[i]);
	printf("\n");
	printf("msg: '");
	for (size_t i = 0; i < msg_len; i++)
		printf("%c", msg[i]);
	printf("'\n");
	kfree(m);

	return 0;
}
