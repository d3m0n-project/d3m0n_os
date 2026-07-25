#include "crypto.h"
#include "get_next_line.h"

#define PRIVATE_KEY_INTEGER_SEQUENCE	{&key->n, &key->e, &key->d, &key->p, &key->q, &key->dp, &key->dq, &key->qInv}
#define MAX_PRIVATE_KEY_LINE_LEN		70

typedef struct
{
	uint8_t	*data;
	size_t	len;
	size_t	cap;
}	DERBuffer;

static void	der_append(DERBuffer *b, const void *src, size_t n)
{
	if (b->len + n > b->cap)
	{
		size_t newCap = b->cap ? b->cap * 2 : 512;
		while (newCap < b->len + n)
			newCap *= 2;

		uint8_t	*new_data = malloc(newCap);
		if (!new_data)
			return;
		ft_memcpy(new_data, b->data, b->len);
		free(b->data);
		b->data = new_data;
		b->cap = newCap;
	}

	ft_memcpy(b->data + b->len, src, n);
	b->len += n;
}

static void	der_byte(DERBuffer *b, uint8_t x)
{
	der_append(b, &x, 1);
}

static void	der_length(DERBuffer *b, size_t len)
{
	if (len < 128)
		der_byte(b, (uint8_t)len);
	else
	{
		uint8_t	tmp[8];
		int		n = 0;
		while (len)
		{
			tmp[n++] = len & 0xff;
			len >>= 8;
		}
		der_byte(b, 0x80 | n);
		while (n--)
			der_byte(b, tmp[n]);
	}
}

static void	der_integer(DERBuffer *b, const uint8_t *buf, size_t len)
{
	der_byte(b, 0x02);

	// add zeros
	int prependZero = (len > 0 && (buf[0] & 0x80));

	der_length(b, len + prependZero);
	if (prependZero)
		der_byte(b, 0x00);

	der_append(b, buf, len);
}

static void	der_integer_u32(DERBuffer *b, uint32_t v)
{
	uint8_t	tmp[5];
	size_t	len = 0;
	do
	{
		tmp[4 - len++] = v & 0xff;
		v >>= 8;
	} while (v);
	der_integer(b, tmp + 5 - len, len);
}

static int	der_read_length(const uint8_t *buf, size_t buf_len, size_t *off, size_t *len)
{
	if (*off >= buf_len)
		return -1;

	uint8_t c = buf[(*off)++];
	if (!(c & 0x80))
	{
		*len = c;
		return (0);
	}

	size_t n = c & 0x7f;
	if (n == 0 || n > sizeof(size_t) || n > buf_len - *off)
		return (-1);

	*len = 0;
	while (n--)
		*len = (*len << 8) | buf[(*off)++];

	return 0;
}

static int	der_read_integer(const uint8_t *buf, size_t buf_len, size_t *off, BigInt *dst)
{
	size_t	len;
	size_t	start;
	size_t	value_off;
	size_t	value_len;

	if (*off >= buf_len || buf[*off] != 0x02)
		return -1;

	(*off)++;
	if (der_read_length(buf, buf_len, off, &len))
		return -1;

	if (len > buf_len - *off)
		return -1;

	start = *off;
	value_off = start;
	value_len = len;
	if (value_len > 1 && buf[value_off] == 0x00)
	{
		value_off++;
		value_len--;
	}

	BigInt *tmp = big_int_from_bytes(buf + value_off, value_len);
	if (!tmp)
		return -1;

	ft_memcpy(dst, tmp, sizeof(BigInt));
	free(tmp);
	*off = start + len;
	return 0;
}

int	rsa_private_key_write_der(char *filename, t_RSA_private_key *key)
{
	DERBuffer	body = {0};
	DERBuffer	out = {0};
	uint8_t		*buf;
	size_t		len;

	// version = 0
	der_integer_u32(&body, 0);

	// add private key intergers
	BigInt	*lst[] = PRIVATE_KEY_INTEGER_SEQUENCE;
	for (size_t i=0; i<sizeof(lst)/sizeof(lst[0]); i++)
	{
		len = 0;
		buf = big_int_get_bytes(lst[i], &len);
		if (!buf)
		{
			free(body.data);
			free(out.data);
			return -1;
		}
		der_integer(&body, buf, len);
		free(buf);
	}	

	// wrap sequence
	der_byte(&out, 0x30);
	der_length(&out, body.len);
	der_append(&out, body.data, body.len);

	int	fd = open(filename, O_WRITE | O_CREATE);
	if (fd < 0)
	{
		log("DER: Could not write certificate to '%s'\n", LOG_ERROR, filename);
		free(body.data);
		free(out.data);
		return -1;
	}
	char	*b64_buf = base64_encode(out.data, out.len);
	if (!b64_buf)
	{
		free(body.data);
		free(out.data);
		return -1;
	}
	size_t	b64_len = ft_strlen(b64_buf);
	size_t	line_count = b64_len / 64;

	write(fd, "-----BEGIN RSA PRIVATE KEY-----\n", 32);
	for (size_t i = 0; i < line_count; i++)
	{
		write(fd, b64_buf + i * 64, 64);
		write(fd, "\n", 1);
	}

	if (b64_len % 64 > 0)
	{
		write(fd, b64_buf + line_count * 64, b64_len % 64);
		write(fd, "\n", 1);
	}
	write(fd, "-----END RSA PRIVATE KEY-----\n", 30);
	close(fd);

	free(body.data);
	free(out.data);
	free(b64_buf);

	return 0;
}

int	rsa_private_key_read_der(char *filename, t_RSA_private_key *key, t_RSA_public_key *pub)
{
	int fd = open(filename, O_READ);
	char	*line = 0;
	int		line_i = 0;
	int		valid_header_and_footer = 0;
	char	*b64_buff = 0;
	size_t	b64_buff_size = 0;
	if (fd < 0)
	{
		log("DER: Could not read certificate from '%s'\n", LOG_ERROR, filename);
		return -1;
	}
	while ((line = get_next_line(fd)) && line_i < MAX_PRIVATE_KEY_LINE_LEN)
	{
		size_t len = ft_strlen(line);
		while (line[len - 1] == '\n' || line[len - 1] == '\r')
		{
			line[len - 1] = '\0';
			len--;
		}

		if (line_i == 0 && ft_strncmp(line, "-----BEGIN RSA PRIVATE KEY-----", len) != 0)
		{
			free(line);
			goto invalid_key;
		}
		else if (line_i == 0)
		{
			valid_header_and_footer = 1;
			line_i++;
			free(line);
			continue;
		}
		else if (line_i > 0 && ft_strcmp(line, "-----END RSA PRIVATE KEY-----") == 0)
		{
			free(line);
			if (valid_header_and_footer != 1)
			{
				log("DER: Invalid header-footer structure in certificate\n", LOG_ERROR);
				if (b64_buff)
					free(b64_buff);
				return -1;
			}
			
			valid_header_and_footer = 2;
			break;
		}
		else if (len > 64)
		{
			free(line);
			goto invalid_key;
		}

		// copy line to base64 buffer
		if (!b64_buff)
		{
			b64_buff_size = 65;
			b64_buff = malloc(sizeof(char) * 64 + 1);
			if (!b64_buff)
			{
				free(line);
				return -1;
			}
			ft_strlcpy(b64_buff, line, 65);
			b64_buff[64] = '\0';
		}
		else
		{
			b64_buff_size += len;
			char *tmp_buff = ft_strjoin(b64_buff, line);
			if (!tmp_buff)
			{
				free(line);
				return -1;
			}
			
			free(b64_buff);
			b64_buff = tmp_buff;
		}

		line_i++;
		free(line);
	}
	close(fd);
	if (valid_header_and_footer != 2)
	{
		log("DER: Invalid header and footer for certificate\n", LOG_ERROR);
		free(b64_buff);
		return -1;
	}

	size_t	b64_decoded_len = 0;
	uint8_t	*b64_decoded = base64_decode(b64_buff, &b64_decoded_len);
	free(b64_buff);
	if (!b64_decoded)
		return -1;

	size_t off = 0;
	size_t seq_len;

	// SEQUENCE
	if (b64_decoded[off++] != 0x30)
		goto parse_error;

	if (der_read_length(b64_decoded, b64_decoded_len, &off, &seq_len))
		goto parse_error;

	// version
	BigInt	version = {0};
	if (der_read_integer(b64_decoded, b64_decoded_len, &off, &version))
		goto parse_error;

	// integers
	BigInt	*lst[] = PRIVATE_KEY_INTEGER_SEQUENCE;
	for (size_t i = 0; i < sizeof(lst) / sizeof(lst[0]); i++)
	{
		if (der_read_integer(b64_decoded, b64_decoded_len, &off, lst[i]))
			goto parse_error;
	}

	// public key
	ft_memcpy(&pub->n, &key->n, sizeof(BigInt));
	ft_memcpy(&pub->e, &key->e, sizeof(BigInt));

	free(b64_decoded);
	return (0);

invalid_key:
	close(fd);
	log("DER: Invalid DER ASN.1 certificate: '%s'\n", LOG_ERROR, filename);
	return -1;

parse_error:
	free(b64_decoded);
	log("DER: Invalid ASN.1 RSA private key\n", LOG_ERROR);
	return (-1);
}

int	rsa_private_key_require(char *filename, t_RSA_private_key *prv, t_RSA_public_key *pub)
{
	if (file_exists(filename))
		return (rsa_private_key_read_der(filename, prv, pub) >= 0);
	else
	{
		if (!rsa_generate_keypair(2048, prv, pub))
		{
			log("Failed to generate a new RSA keypair\n", LOG_ERROR);
			return 0;
		}
		else
		{
			if (rsa_private_key_write_der(filename, prv) != 0)
			{
				log("RSA: DER: Could not save your private key to '%s'\n", LOG_ERROR, filename);
				return 0;
			}
			else
			{
				log("RSA: DER: Saved your private key to '%s'!\n", LOG_SUCCESS, filename);
				return 1;
			}
		}
	}
	return 0;
}