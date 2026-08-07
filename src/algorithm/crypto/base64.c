#include "crypto.h"

static uint8_t	 CHARSET[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char	*base64_encode(uint8_t *data, size_t len)
{
	int		counts = 0;
	char	buffer[3];
	int		c = 0;

	size_t	size = len * 4 / 3 + 4;
	if (size == 0)
		return 0;

	char*	out = kmalloc(size);
	if (!out)
		return 0;
	

	for(size_t i=0; i < len; i++)
	{
		buffer[counts++] = data[i];
		if(counts == 3)
		{
			out[c++] = CHARSET[buffer[0] >> 2];
			out[c++] = CHARSET[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
			out[c++] = CHARSET[((buffer[1] & 0x0f) << 2) + (buffer[2] >> 6)];
			out[c++] = CHARSET[buffer[2] & 0x3f];
			counts = 0;
		}
	}

	if(counts > 0)
	{
		out[c++] = CHARSET[buffer[0] >> 2];
		if(counts == 1)
		{
			out[c++] = CHARSET[(buffer[0] & 0x03) << 4];
			out[c++] = '=';
		}
		else
		{
			out[c++] = CHARSET[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
			out[c++] = CHARSET[(buffer[1] & 0x0f) << 2];
		}
		out[c++] = '=';
	}
	out[c] = '\0';
	return out;
}

uint8_t		*base64_decode(char *data, size_t *out_len)
{
	int		counts = 0;
	char	buffer[4];
	int		p = 0;

	*out_len = ft_strlen(data) * 3 / 4;
	if (*out_len == 0)
		return 0;
	
	uint8_t	*plain = kmalloc(*out_len);
	if (!plain)
		return 0;
	
	for(int i = 0; data[i]; i++)
	{
		char k;
		for(k='\0'; k < 64 && CHARSET[(int)k] != data[i]; k++);
		buffer[counts++] = k;
		if(counts == 4)
		{
			plain[p++] = (buffer[0] << 2) + (buffer[1] >> 4);
			if(buffer[2] != 64)
				plain[p++] = (buffer[1] << 4) + (buffer[2] >> 2);
			if(buffer[3] != 64)
				plain[p++] = (buffer[2] << 6) + buffer[3];
			counts = 0;
		}
	}
	plain[p] = '\0';
	return plain;
}