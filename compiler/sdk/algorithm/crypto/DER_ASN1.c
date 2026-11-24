#include "crypto.h"

#define PRIVATE_KEY_INTEGER_SEQUENCE	{&key->n, &key->e, &key->d, &key->p, &key->q, &key->dp, &key->dq, &key->qInv}

typedef struct
{
	uint8_t	*data;
	size_t	len;
	size_t	cap;
	int		failed;
}	DERBuffer;

static void secure_zero(void *data, size_t len)
{
	volatile uint8_t *p = data;
	while (len--) *p++ = 0;
}

static void secure_free(void *data, size_t len)
{
	if (data) { secure_zero(data, len); kfree(data); }
}

static void	der_append(DERBuffer *b, const void *src, size_t n)
{
	if (b->failed || n > SIZE_MAX - b->len) { b->failed = 1; return; }
	if (b->len + n > b->cap)
	{
		size_t newCap = b->cap ? b->cap * 2 : 512;
		while (newCap < b->len + n && newCap <= SIZE_MAX / 2)
			newCap *= 2;
		if (newCap < b->len + n) { b->failed = 1; return; }

		uint8_t	*new_data = kmalloc(newCap);
		if (!new_data) { b->failed = 1; return; }
		if (b->len) memcpy(new_data, b->data, b->len);
		secure_free(b->data, b->cap);
		b->data = new_data;
		b->cap = newCap;
	}

	if (n) memcpy(b->data + b->len, src, n);
	b->len += n;
}

static int write_all(int fd, const void *data, size_t len)
{
	const char *bytes = data;
	while (len)
	{
		uint32_t chunk = len > UINT32_MAX ? UINT32_MAX : (uint32_t)len;
		int written = write(fd, bytes, chunk);
		if (written <= 0) return -1;
		bytes += written;
		len -= (size_t)written;
	}
	return 0;
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
	if (n == 0 || n > sizeof(size_t) || n > buf_len - *off || buf[*off] == 0)
		return (-1);

	*len = 0;
	while (n--)
	{
		if (*len > (SIZE_MAX >> 8)) return -1;
		*len = (*len << 8) | buf[(*off)++];
	}

	if (*len < 128)
		return (-1);
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

	if (len == 0 || len > buf_len - *off || len > MAX_BIGINT_LIMB * sizeof(uint32_t))
		return -1;

	start = *off;
	value_off = start;
	value_len = len;
	if ((buf[value_off] & 0x80) || (value_len > 1 && buf[value_off] == 0x00
		&& !(buf[value_off + 1] & 0x80)))
		return -1;
	if (value_len > 1 && buf[value_off] == 0x00)
	{
		value_off++;
		value_len--;
	}

	BigInt *tmp = big_int_from_bytes(buf + value_off, value_len);
	if (!tmp)
		return -1;

	memcpy(dst, tmp, sizeof(BigInt));
	secure_free(tmp, sizeof(*tmp));
	*off = start + len;
	return 0;
}

typedef struct s_cursor { const uint8_t *data; size_t len; size_t off; } DERCursor;

static int cursor_u32(DERCursor *c, uint32_t *value)
{
	if (c->off > c->len || c->len - c->off < 4) return -1;
	*value = ((uint32_t)c->data[c->off] << 24) | ((uint32_t)c->data[c->off + 1] << 16)
		| ((uint32_t)c->data[c->off + 2] << 8) | c->data[c->off + 3];
	c->off += 4;
	return 0;
}

static int cursor_string(DERCursor *c, const uint8_t **value, size_t *len)
{
	uint32_t n;
	if (cursor_u32(c, &n) || c->off > c->len || n > c->len - c->off) return -1;
	*value = c->data + c->off;
	*len = n;
	c->off += n;
	return 0;
}

static int cursor_mpint(DERCursor *c, BigInt *dst)
{
	const uint8_t *bytes;
	size_t len;
	if (cursor_string(c, &bytes, &len) || len == 0 || len > MAX_BIGINT_LIMB * sizeof(uint32_t)) return -1;
	/* OpenSSH mpints are signed. RSA values must be positive and minimally encoded. */
	if (bytes[0] & 0x80) return -1;
	if (len > 1 && bytes[0] == 0 && !(bytes[1] & 0x80)) return -1;
	if (bytes[0] == 0) { bytes++; len--; }
	if (!len) return -1;
	BigInt *tmp = big_int_from_bytes(bytes, len);
	if (!tmp || tmp->sign <= 0) { if (tmp) secure_free(tmp, sizeof(*tmp)); return -1; }
	memcpy(dst, tmp, sizeof(*dst));
	secure_free(tmp, sizeof(*tmp));
	return 0;
}

static int key_is_valid(t_RSA_private_key *key)
{
	BigInt one = {.sign = 1, .length = 1, .digits = {1}};
	BigInt three = {.sign = 1, .length = 1, .digits = {3}};
	BigInt *p1 = 0, *q1 = 0, *product = 0, *check = 0, *ed = 0, *remainder = 0;
	int ok = 0;
	if (key->n.sign <= 0 || key->d.sign <= 0 || key->p.sign <= 0 || key->q.sign <= 0
		|| key->dp.sign <= 0 || key->dq.sign <= 0 || key->qInv.sign <= 0
		|| big_int_cmp(&key->p, &three) <= 0 || big_int_cmp(&key->q, &three) <= 0
		|| big_int_cmp(&key->p, &key->q) == 0
		|| big_int_bit_length(&key->n) < 2048 || big_int_bit_length(&key->n) > MAX_BIGINT_LIMB * 32
		|| big_int_cmp(&key->e, &three) < 0 || big_int_cmp(&key->e, &key->n) >= 0
		|| big_int_cmp(&key->d, &key->n) >= 0 || big_int_cmp(&key->qInv, &key->p) >= 0
		|| is_even(&key->e)
		|| is_even(&key->p) || is_even(&key->q))
		return 0;
	p1 = big_int_sub(&key->p, &one); q1 = big_int_sub(&key->q, &one);
	product = big_int_mul(&key->p, &key->q);
	if (!p1 || !q1 || !product || big_int_cmp(product, &key->n) != 0) goto done;
	check = big_int_mod(&key->d, p1);
	if (!check || big_int_cmp(check, &key->dp) != 0) goto done;
	secure_free(check, sizeof(*check)); check = 0;
	check = big_int_mod(&key->d, q1);
	if (!check || big_int_cmp(check, &key->dq) != 0) goto done;
	secure_free(check, sizeof(*check)); check = 0;
	check = big_int_mul(&key->qInv, &key->q);
	if (!check) goto done;
	remainder = big_int_mod(check, &key->p);
	if (!remainder || big_int_cmp(remainder, &one) != 0) goto done;
	secure_free(check, sizeof(*check)); check = 0; secure_free(remainder, sizeof(*remainder)); remainder = 0;
	ed = big_int_mul(&key->e, &key->dp);
	if (!ed) goto done;
	remainder = big_int_mod(ed, p1);
	if (!remainder || big_int_cmp(remainder, &one) != 0) goto done;
	secure_free(ed, sizeof(*ed)); ed = 0; secure_free(remainder, sizeof(*remainder)); remainder = 0;
	ed = big_int_mul(&key->e, &key->dq);
	if (!ed) goto done;
	remainder = big_int_mod(ed, q1);
	if (!remainder || big_int_cmp(remainder, &one) != 0) goto done;
	ok = 1;
done:
	if (p1) secure_free(p1, sizeof(*p1));
	if (q1) secure_free(q1, sizeof(*q1));
	if (product) secure_free(product, sizeof(*product));
	if (check) secure_free(check, sizeof(*check));
	if (ed) secure_free(ed, sizeof(*ed));
	if (remainder) secure_free(remainder, sizeof(*remainder));
	return ok;
}

static int decode_base64_strict(const uint8_t *src, size_t src_len, uint8_t **out, size_t *out_len)
{
	static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	uint8_t *clean = kmalloc(src_len + 1), *decoded;
	size_t n = 0, produced = 0;
	if (!clean) return -1;
	for (size_t i = 0; i < src_len; ++i)
	{
		uint8_t c = src[i];
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			continue;
		if (!(c == '=' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
			|| (c >= '0' && c <= '9') || c == '+' || c == '/')) { secure_free(clean, src_len + 1); return -1; }
		clean[n++] = c;
	}
	if (!n || n % 4 || n > 12000) { secure_free(clean, src_len + 1); return -1; }
	decoded = kmalloc((n / 4) * 3);
	if (!decoded) { secure_free(clean, src_len + 1); return -1; }
	for (size_t i = 0; i < n; i += 4)
	{
		uint32_t v[4] = {0, 0, 0, 0};
		int pad = 0;
		for (size_t j = 0; j < 4; ++j)
		{
			if (clean[i + j] == '=')
			{
				if (j < 2 || i + 4 != n) goto invalid_b64;
				pad++;
			}
			else
			{
				if (pad) goto invalid_b64;
				const char *p = strchr(alphabet, clean[i + j]);
				if (!p) goto invalid_b64;
				v[j] = (uint32_t)(p - alphabet);
			}
		}
		if (pad > 2 || (pad == 1 && (v[2] & 3)) || (pad == 2 && (v[1] & 15))) goto invalid_b64;
		decoded[produced++] = (uint8_t)((v[0] << 2) | (v[1] >> 4));
		if (pad < 2) decoded[produced++] = (uint8_t)((v[1] << 4) | (v[2] >> 2));
		if (pad == 0) decoded[produced++] = (uint8_t)((v[2] << 6) | v[3]);
	}
	secure_free(clean, src_len + 1); *out = decoded; *out_len = produced; return 0;
invalid_b64:
	secure_free(clean, src_len + 1); secure_free(decoded, (n / 4) * 3); return -1;
}

int	rsa_private_key_write_der(char *filename, t_RSA_private_key *key)
{
	DERBuffer	body = {0};
	DERBuffer	out = {0};
	uint8_t		*buf = 0;
	char		*b64_buf = 0;
	size_t		len, b64_len = 0;
	int			fd = -1, result = -1;
	static const char header[] = "-----BEGIN RSA PRIVATE KEY-----\n";
	static const char footer[] = "-----END RSA PRIVATE KEY-----\n";
	if (!filename || !key || !key_is_valid(key)) return -1;

	// version = 0
	der_integer_u32(&body, 0);

	// add private key intergers
	BigInt	*lst[] = PRIVATE_KEY_INTEGER_SEQUENCE;
	for (size_t i=0; i<sizeof(lst)/sizeof(lst[0]); i++)
	{
		len = 0;
		buf = big_int_get_bytes(lst[i], &len);
		if (!buf)
			goto cleanup;
		der_integer(&body, buf, len);
		secure_free(buf, len);
		if (body.failed) goto cleanup;
	}	

	// wrap sequence
	der_byte(&out, 0x30);
	der_length(&out, body.len);
	der_append(&out, body.data, body.len);
	if (body.failed || out.failed) goto cleanup;

	b64_buf = base64_encode(out.data, out.len);
	if (!b64_buf) goto cleanup;
	b64_len = strlen(b64_buf);
	fd = open(filename, O_WRITE | O_CREATE | O_TRUNC);
	if (fd < 0)
	{
		log("DER: Could not write certificate to '%s'\n", LOG_ERROR, filename);
		goto cleanup;
	}
	if (write_all(fd, header, sizeof(header) - 1)) goto cleanup;
	for (size_t i = 0; i < b64_len; i += 64)
	{
		size_t line_len = b64_len - i;
		if (line_len > 64) line_len = 64;
		if (write_all(fd, b64_buf + i, line_len) || write_all(fd, "\n", 1)) goto cleanup;
	}
	if (write_all(fd, footer, sizeof(footer) - 1)) goto cleanup;
	result = 0;
cleanup:
	if (fd >= 0 && close(fd) < 0) result = -1;
	secure_free(body.data, body.cap);
	secure_free(out.data, out.cap);
	secure_free(b64_buf, b64_len + (b64_buf != 0));
	return result;
}

static int parse_pkcs1(const uint8_t *data, size_t len, t_RSA_private_key *key)
{
	size_t off = 0, seq_len, end;
	BigInt version = {0};
	if (len < 2 || data[off++] != 0x30 || der_read_length(data, len, &off, &seq_len)
		|| seq_len != len - off) return -1;
	end = off + seq_len;
	if (der_read_integer(data, end, &off, &version) || !is_zero(&version)) return -1;
	BigInt *values[] = PRIVATE_KEY_INTEGER_SEQUENCE;
	for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
		if (der_read_integer(data, end, &off, values[i])) return -1;
	return off == end ? 0 : -1;
}

static int parse_pkcs8(const uint8_t *data, size_t len, t_RSA_private_key *key)
{
	static const uint8_t rsa_algorithm[] = {0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d,
		0x01, 0x01, 0x01, 0x05, 0x00};
	size_t off = 0, seq_len, end, alg_len, octet_len;
	BigInt version = {0};
	if (len < 2 || data[off++] != 0x30 || der_read_length(data, len, &off, &seq_len)
		|| seq_len != len - off) return -1;
	end = off + seq_len;
	if (der_read_integer(data, end, &off, &version) || !is_zero(&version)
		|| off >= end || data[off++] != 0x30
		|| der_read_length(data, end, &off, &alg_len) || alg_len != sizeof(rsa_algorithm)
		|| alg_len > end - off || memcmp(data + off, rsa_algorithm, sizeof(rsa_algorithm))) return -1;
	off += alg_len;
	if (off >= end || data[off++] != 0x04 || der_read_length(data, end, &off, &octet_len)
		|| octet_len != end - off) return -1;
	return parse_pkcs1(data + off, octet_len, key);
}

static int parse_openssh(const uint8_t *data, size_t len, t_RSA_private_key *key)
{
	static const uint8_t magic[] = "openssh-key-v1\0";
	DERCursor file = {data, len, sizeof(magic) - 1}, pub, priv;
	const uint8_t *cipher, *kdf, *opts, *pub_data, *priv_data, *str;
	size_t cipher_len, kdf_len, opts_len, pub_len, priv_len, str_len;
	uint32_t count, check1, check2;
	BigInt public_n = {0}, public_e = {0};
	if (len < sizeof(magic) - 1 || memcmp(data, magic, sizeof(magic) - 1)) return -1;
	if (cursor_string(&file, &cipher, &cipher_len) || cursor_string(&file, &kdf, &kdf_len)
		|| cursor_string(&file, &opts, &opts_len) || cursor_u32(&file, &count)
		|| cipher_len != 4 || memcmp(cipher, "none", 4) || kdf_len != 4
		|| memcmp(kdf, "none", 4) || opts_len || count != 1
		|| cursor_string(&file, &pub_data, &pub_len) || cursor_string(&file, &priv_data, &priv_len)
		|| file.off != file.len) return -1;
	pub = (DERCursor){pub_data, pub_len, 0};
	if (cursor_string(&pub, &str, &str_len) || str_len != 7 || memcmp(str, "ssh-rsa", 7)
		|| cursor_mpint(&pub, &public_e) || cursor_mpint(&pub, &public_n) || pub.off != pub.len) return -1;
	priv = (DERCursor){priv_data, priv_len, 0};
	if (cursor_u32(&priv, &check1) || cursor_u32(&priv, &check2) || check1 != check2
		|| cursor_string(&priv, &str, &str_len) || str_len != 7 || memcmp(str, "ssh-rsa", 7)
		|| cursor_mpint(&priv, &key->n) || cursor_mpint(&priv, &key->e)
		|| cursor_mpint(&priv, &key->d) || cursor_mpint(&priv, &key->qInv)
		|| cursor_mpint(&priv, &key->p) || cursor_mpint(&priv, &key->q)
		|| cursor_string(&priv, &str, &str_len) || big_int_cmp(&public_n, &key->n)
		|| big_int_cmp(&public_e, &key->e)) return -1;
	/* OpenSSH stores p and q but not the reduced exponents. */
	BigInt one = {.sign = 1, .length = 1, .digits = {1}};
	BigInt *p1 = big_int_sub(&key->p, &one), *q1 = big_int_sub(&key->q, &one);
	BigInt *dp = p1 ? big_int_mod(&key->d, p1) : 0;
	BigInt *dq = q1 ? big_int_mod(&key->d, q1) : 0;
	if (!p1 || !q1 || !dp || !dq) { if (p1) secure_free(p1, sizeof(*p1)); if (q1) secure_free(q1, sizeof(*q1)); if (dp) secure_free(dp, sizeof(*dp)); if (dq) secure_free(dq, sizeof(*dq)); return -1; }
	memcpy(&key->dp, dp, sizeof(BigInt)); memcpy(&key->dq, dq, sizeof(BigInt));
	secure_free(p1, sizeof(*p1)); secure_free(q1, sizeof(*q1)); secure_free(dp, sizeof(*dp)); secure_free(dq, sizeof(*dq));
	/* Remaining bytes must be exactly OpenSSH's incremental padding. */
	if (priv.len - priv.off > 7) return -1;
	for (uint8_t pad = 1; priv.off < priv.len; ++pad, ++priv.off)
		if (priv.data[priv.off] != pad) return -1;
	return 0;
}

int rsa_private_key_read_der(char *filename, t_RSA_private_key *key, t_RSA_public_key *pub)
{
	const size_t max_file = 8192;
	uint8_t *file = 0, *decoded = 0;
	size_t file_len = 0, decoded_len = 0, body_off = 0, body_len = 0;
	const char *begin = 0, *end = 0;
	t_RSA_private_key parsed = {0};
	t_RSA_public_key parsed_pub = {0};
	int fd, result = -1;
	if (!filename || !key || !pub)
		return -1;
	
	fd = open(filename, O_READ);
	if (fd < 0)
	{
		log("RSA: Could not open private key file '%s'\n", LOG_ERROR, filename);
		return -1;
	}
	file = kmalloc(max_file + 1);
	if (!file)
	{
		close(fd);
		return -1;
	}

	while (file_len < max_file)
	{
		int n = read(fd, (char *)file + file_len, (uint32_t)(max_file - file_len));
		if (n < 0)
			goto done;
		if (n == 0)
			break;
		file_len += (size_t)n;
	}
	if (file_len == max_file) { char extra; if (read(fd, &extra, 1) != 0) goto done; }
	file[file_len] = 0;
	if (file_len >= 32 && !memcmp(file, "-----BEGIN RSA PRIVATE KEY-----", 31))
		{ begin = "-----BEGIN RSA PRIVATE KEY-----"; end = "-----END RSA PRIVATE KEY-----"; }
	else if (file_len >= 35 && !memcmp(file, "-----BEGIN OPENSSH PRIVATE KEY-----", 35))
		{ begin = "-----BEGIN OPENSSH PRIVATE KEY-----"; end = "-----END OPENSSH PRIVATE KEY-----"; }
	else if (file_len >= 27 && !memcmp(file, "-----BEGIN PRIVATE KEY-----", 27))
		{ begin = "-----BEGIN PRIVATE KEY-----"; end = "-----END PRIVATE KEY-----"; }
	else
		goto done;

	body_off = strlen(begin);
	if (file[body_off] != '\r' && file[body_off] != '\n')
		goto done;
	
	while (file[body_off] == '\r' || file[body_off] == '\n')
		++body_off;

	char *footer = strstr((char *)file + body_off, end);
	if (!footer)
		goto done;

	body_len = (size_t)(footer - ((char *)file + body_off));
	footer += strlen(end);
	while (*footer == '\r' || *footer == '\n' || *footer == ' ' || *footer == '\t')
		++footer;
	
	if (*footer)
		goto done;
	
	if (decode_base64_strict(file + body_off, body_len, &decoded, &decoded_len))
	{
		log("RSA: Private key file has invalid base64 data\n", LOG_ERROR);
		goto done;
	}

	if (decoded_len >= 15 && !memcmp(decoded, "openssh-key-v1\0", 15))
		result = parse_openssh(decoded, decoded_len, &parsed);
	else if (!memcmp(begin, "-----BEGIN PRIVATE KEY-----", 27))
		result = parse_pkcs8(decoded, decoded_len, &parsed);
	else
		result = parse_pkcs1(decoded, decoded_len, &parsed);
	
	if (result)
	{
		log("RSA: Unsupported, encrypted, or malformed private key\n", LOG_ERROR);
		goto done;
	}
	if (!key_is_valid(&parsed))
	{
		log("RSA: Private key components are inconsistent or too weak\n", LOG_ERROR);
		goto done;
	}
	memcpy(key, &parsed, sizeof(parsed));
	memcpy(&parsed_pub.n, &parsed.n, sizeof(BigInt));
	memcpy(&parsed_pub.e, &parsed.e, sizeof(BigInt));
	memcpy(pub, &parsed_pub, sizeof(parsed_pub));
	result = 0;
done:
	close(fd);
	secure_free(decoded, decoded_len);
	secure_free(file, file_len + (file != 0));
	secure_zero(&parsed, sizeof(parsed));
	return result;
}

int	rsa_private_key_require(char *filename, t_RSA_private_key *prv, t_RSA_public_key *pub)
{
	if (!filename || !prv || !pub)
		return 0;
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
			log("generated!\n", 0);
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
