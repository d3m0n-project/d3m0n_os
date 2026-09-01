#ifndef APP_MANIFEST_H
#define APP_MANIFEST_H

#include "types.h"

#define APP_MANIFEST_MAGIC	0x35455234

#ifdef __cplusplus
extern "C" {
#endif
typedef struct AppMetadata
{
	uint32_t			magic;
	float				version;

	const char			*name;
	const char			*author;
	const char			*description;

	uint32_t			icon_size;
	const uint8_t		*icon;
	uint32_t			end_magic;
}		AppMetadata;
#ifdef __cplusplus
}
#endif

#endif