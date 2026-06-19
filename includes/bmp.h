#ifndef BMP_H
#define BMP_H

#ifndef SHOW_IMAGE_STATUS
#define SHOW_IMAGE_STATUS 0
#endif

typedef struct BmpTexture
{
	int				width;
	int				height;
	int				bytes_per_pixel;
	unsigned int	*pixels;
}	BmpTexture;


int	bmp_load_image(BmpTexture *out, const char *path);
int	free_bmp_texture(BmpTexture *texture);

#endif