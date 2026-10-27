#ifndef PNG_H
#define PNG_H

#include "types.h"
#ifdef __cplusplus
#include "../../stdlib.h"
#else
#include "../../stdlib.h"
#endif

static inline int png_bytes_equal(const unsigned char *a, const unsigned char *b, size_t count)
{
	for (size_t i = 0; i < count; i++)
		if (a[i] != b[i])
			return 0;
	return 1;
}

static inline void png_copy_bytes(unsigned char *destination, const unsigned char *source, size_t count)
{
	for (size_t i = 0; i < count; i++)
		destination[i] = source[i];
}

typedef struct s_png_bits
{
	const unsigned char *data;
	size_t size;
	size_t position;
	unsigned bit_buffer;
	unsigned bit_count;
} t_png_bits;

typedef struct s_png_huffman
{
	unsigned short count[16];
	unsigned short symbol[288];
} t_png_huffman;

static inline unsigned png_read_bits(t_png_bits *bits, unsigned count)
{
	while (bits->bit_count < count)
	{
		if (bits->position >= bits->size)
			return 0xffffffffU;
		bits->bit_buffer |= (unsigned)bits->data[bits->position++] << bits->bit_count;
		bits->bit_count += 8;
	}
	unsigned value = bits->bit_buffer & ((1U << count) - 1);
	bits->bit_buffer >>= count;
	bits->bit_count -= count;
	return value;
}

static inline int png_build_huffman(t_png_huffman *tree, const unsigned char *lengths, unsigned count)
{
	for (unsigned i = 0; i < 16; i++)
		tree->count[i] = 0;
	for (unsigned i = 0; i < count; i++)
	{
		if (lengths[i] > 15)
			return 0;
		tree->count[lengths[i]]++;
	}
	tree->count[0] = 0;
	unsigned offset[16];
	offset[1] = 0;
	for (unsigned i = 1; i < 15; i++)
		offset[i + 1] = offset[i] + tree->count[i];
	for (unsigned i = 0; i < count; i++)
		if (lengths[i])
			tree->symbol[offset[lengths[i]]++] = (unsigned short)i;
	return 1;
}

static inline int png_decode_symbol(t_png_bits *bits, const t_png_huffman *tree)
{
	unsigned code = 0, first = 0, index = 0;
	for (unsigned length = 1; length <= 15; length++)
	{
		unsigned bit = png_read_bits(bits, 1);
		if (bit > 1)
			return -1;
		code |= bit;
		unsigned count = tree->count[length];
		if (code >= first && code - first < count)
			return tree->symbol[index + code - first];
		index += count;
		first = (first + count) << 1;
		code <<= 1;
	}
	return -1;
}

static inline int png_inflate_codes(t_png_bits *bits, const t_png_huffman *literal_tree, const t_png_huffman *distance_tree, unsigned char *output, size_t capacity, size_t *written)
{
	static const unsigned short length_base[29] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
	static const unsigned char length_extra[29] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
	static const unsigned short distance_base[30] = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
	static const unsigned char distance_extra[30] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
	for (;;)
	{
		int symbol = png_decode_symbol(bits, literal_tree);
		if (symbol < 0)
			return 0;
		if (symbol < 256)
		{
			if (*written >= capacity)
				return 0;
			output[(*written)++] = (unsigned char)symbol;
		}
		else if (symbol == 256)
			return 1;
		else if (symbol <= 285)
		{
			unsigned length_index = (unsigned)symbol - 257;
			unsigned length = length_base[length_index] + png_read_bits(bits, length_extra[length_index]);
			int distance_symbol = png_decode_symbol(bits, distance_tree);
			if (distance_symbol < 0 || distance_symbol >= 30)
				return 0;
			unsigned distance = distance_base[distance_symbol] + png_read_bits(bits, distance_extra[distance_symbol]);
			if (distance > *written || length > capacity - *written)
				return 0;
			while (length--)
			{
				output[*written] = output[*written - distance];
				(*written)++;
			}
		}
		else
			return 0;
	}
}

static inline int png_inflate(const unsigned char *input, size_t input_size, unsigned char *output, size_t capacity, size_t *written)
{
	static const unsigned char code_order[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
	static const unsigned char fixed_distance_lengths[32] = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
	t_png_bits bits;
	bits.data = input;
	bits.size = input_size;
	bits.position = 2;
	bits.bit_buffer = 0;
	bits.bit_count = 0;
	*written = 0;
	for (;;)
	{
		unsigned final_block = png_read_bits(&bits, 1);
		unsigned block_type = png_read_bits(&bits, 2);
		if (final_block > 1 || block_type > 2)
			return 0;
		if (block_type == 0)
		{
			bits.bit_buffer = 0;
			bits.bit_count = 0;
			unsigned length = png_read_bits(&bits, 16);
			unsigned inverse = png_read_bits(&bits, 16);
			if (length > capacity - *written || (length ^ 0xffffU) != inverse || bits.position + length > bits.size)
				return 0;
			for (unsigned i = 0; i < length; i++)
				output[(*written)++] = bits.data[bits.position++];
		}
		else
		{
			t_png_huffman literal_tree, distance_tree;
			unsigned char literal_lengths[288];
			if (block_type == 1)
			{
				for (unsigned i = 0; i <= 143; i++) literal_lengths[i] = 8;
				for (unsigned i = 144; i <= 255; i++) literal_lengths[i] = 9;
				for (unsigned i = 256; i <= 279; i++) literal_lengths[i] = 7;
				for (unsigned i = 280; i < 288; i++) literal_lengths[i] = 8;
				if (!png_build_huffman(&literal_tree, literal_lengths, 288) || !png_build_huffman(&distance_tree, fixed_distance_lengths, 32))
					return 0;
			}
			else
			{
				unsigned hlit = png_read_bits(&bits, 5) + 257;
				unsigned hdist = png_read_bits(&bits, 5) + 1;
				unsigned hclen = png_read_bits(&bits, 4) + 4;
				unsigned char code_lengths[19] = {0};
				for (unsigned i = 0; i < hclen; i++) code_lengths[code_order[i]] = (unsigned char)png_read_bits(&bits, 3);
				t_png_huffman code_tree;
				if (hlit > 286 || hdist > 32 || !png_build_huffman(&code_tree, code_lengths, 19))
					return 0;
				unsigned char lengths[318];
				unsigned total = hlit + hdist, used = 0;
				while (used < total)
				{
					int value = png_decode_symbol(&bits, &code_tree);
					if (value < 0)
						return 0;
					if (value < 16)
						lengths[used++] = (unsigned char)value;
					else
					{
						unsigned repeat;
					unsigned char repeated;
					if (value == 16)
					{
						if (!used) return 0;
						repeat = png_read_bits(&bits, 2) + 3;
						repeated = lengths[used - 1];
					}
					else if (value == 17)
					{
						repeat = png_read_bits(&bits, 3) + 3;
						repeated = 0;
					}
					else if (value == 18)
					{
						repeat = png_read_bits(&bits, 7) + 11;
						repeated = 0;
					}
					else return 0;
					if (repeat > total - used) return 0;
					while (repeat--) lengths[used++] = repeated;
					}
				}
				if (!png_build_huffman(&literal_tree, lengths, hlit) || !png_build_huffman(&distance_tree, lengths + hlit, hdist))
					return 0;
			}
			if (!png_inflate_codes(&bits, &literal_tree, &distance_tree, output, capacity, written))
				return 0;
		}
		if (final_block)
			return 1;
	}
}

static inline unsigned png_read_u32(const unsigned char *p)
{
	return ((unsigned)p[0] << 24) | ((unsigned)p[1] << 16) | ((unsigned)p[2] << 8) | p[3];
}

static inline int png_decode(const unsigned char *data, size_t size, uint32_t *width, uint32_t *height, uint32_t **pixels)
{
	static const unsigned char signature[8] = {137,80,78,71,13,10,26,10};
	if (!data || size < 33 || !png_bytes_equal(data, signature, 8))
		return 0;
	uint32_t image_width = 0, image_height = 0;
	unsigned bit_depth = 0, color_type = 0, interlace = 0;
	size_t compressed_size = 0;
	for (size_t pos = 8; pos + 12 <= size;)
	{
		unsigned length = png_read_u32(data + pos);
		if ((size_t)length > size - pos - 12)
			return 0;
		const unsigned char *type = data + pos + 4;
		const unsigned char *chunk = data + pos + 8;
		if (png_bytes_equal(type, (const unsigned char *)"IHDR", 4) && length == 13)
		{
			image_width = png_read_u32(chunk);
			image_height = png_read_u32(chunk + 4);
			bit_depth = chunk[8];
			color_type = chunk[9];
			interlace = chunk[12];
		}
		else if (png_bytes_equal(type, (const unsigned char *)"IDAT", 4))
			compressed_size += length;
		else if (png_bytes_equal(type, (const unsigned char *)"IEND", 4))
			break;
		pos += (size_t)length + 12;
	}
	if (!image_width || !image_height || (size_t)image_width > 1048576U / image_height || bit_depth != 8 || (color_type != 2 && color_type != 6) || interlace != 0 || !compressed_size)
		return 0;
	unsigned channels = color_type == 6 ? 4 : 3;
	if ((size_t)image_width > (size_t)-1 / channels)
		return 0;
	size_t row_size = (size_t)image_width * channels;
	if (row_size + 1 > (size_t)-1 / image_height)
		return 0;
	size_t raw_size = (row_size + 1) * image_height;
	if ((size_t)image_width > (size_t)-1 / image_height / sizeof(uint32_t))
		return 0;
	unsigned char *compressed = (unsigned char *)malloc(compressed_size);
	unsigned char *raw = (unsigned char *)malloc(raw_size);
	unsigned char *scanlines = (unsigned char *)malloc(row_size * image_height);
	uint32_t *output = (uint32_t *)malloc((size_t)image_width * image_height * sizeof(uint32_t));
	if (!compressed || !raw || !scanlines || !output)
	{
		free(compressed); free(raw); free(scanlines); free(output);
		return 0;
	}
	size_t compressed_offset = 0;
	for (size_t pos = 8; pos + 12 <= size;)
	{
		unsigned length = png_read_u32(data + pos);
		if ((size_t)length > size - pos - 12) break;
		if (png_bytes_equal(data + pos + 4, (const unsigned char *)"IDAT", 4))
		{
			png_copy_bytes(compressed + compressed_offset, data + pos + 8, length);
			compressed_offset += length;
		}
		if (png_bytes_equal(data + pos + 4, (const unsigned char *)"IEND", 4)) break;
		pos += (size_t)length + 12;
	}
	size_t raw_written = 0;
	if (compressed_size < 6 || !png_inflate(compressed, compressed_size - 4, raw, raw_size, &raw_written) || raw_written != raw_size)
	{
		free(compressed); free(raw); free(scanlines); free(output);
		return 0;
	}
	for (uint32_t y = 0; y < image_height; y++)
	{
		const unsigned char *source = raw + (row_size + 1) * y + 1;
		unsigned char *row = scanlines + row_size * y;
		unsigned filter = raw[(row_size + 1) * y];
		for (size_t x = 0; x < row_size; x++)
		{
			unsigned left = x >= channels ? row[x - channels] : 0;
			unsigned above = y ? scanlines[row_size * (y - 1) + x] : 0;
			unsigned upper_left = y && x >= channels ? scanlines[row_size * (y - 1) + x - channels] : 0;
			unsigned predictor = 0;
			if (filter == 1) predictor = left;
			else if (filter == 2) predictor = above;
			else if (filter == 3) predictor = (left + above) / 2;
			else if (filter == 4)
			{
				int p = (int)left + (int)above - (int)upper_left;
				int pa = p - (int)left; if (pa < 0) pa = -pa;
				int pb = p - (int)above; if (pb < 0) pb = -pb;
				int pc = p - (int)upper_left; if (pc < 0) pc = -pc;
				predictor = pa <= pb && pa <= pc ? left : (pb <= pc ? above : upper_left);
			}
			else if (filter != 0)
			{
				free(compressed); free(raw); free(scanlines); free(output);
				return 0;
			}
			row[x] = (unsigned char)(source[x] + predictor);
		}
	}
	for (size_t i = 0; i < (size_t)image_width * image_height; i++)
	{
		unsigned char *pixel = scanlines + i * channels;
		unsigned alpha = channels == 4 ? pixel[3] : 255;
		output[i] = (alpha << 24) | ((uint32_t)pixel[2] << 16) | ((uint32_t)pixel[1] << 8) | pixel[0];
	}
	free(compressed); free(raw); free(scanlines);
	*width = image_width;
	*height = image_height;
	*pixels = output;
	return 1;
}

#endif
