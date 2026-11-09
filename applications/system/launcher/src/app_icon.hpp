#ifndef APP_ICON_HPP
#define APP_ICON_HPP

#include "app.hpp"

static uint32_t app_icon_read_u32(const unsigned char *data)
{
	return (uint32_t)data[0] | ((uint32_t)data[1] << 8) | ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

static uint16_t app_icon_read_u16(const unsigned char *data)
{
	return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static bool	app_icon_section(const unsigned char *elf, size_t file_size, uint32_t section_offset, uint16_t section_size, uint16_t section_count, uint32_t string_offset, uint32_t string_size, const char *wanted, uint32_t *out_offset, uint32_t *out_size, uint32_t *out_addr)
{
	if (!section_size || section_size < 40 || section_offset > file_size || (uint64_t)section_size * section_count > file_size - section_offset || string_offset > file_size || string_size > file_size - string_offset)
		return false;
	
	const char *strings = (const char *)elf + string_offset;
	for (uint16_t i = 0; i < section_count; ++i)
	{
		const unsigned char *section = elf + section_offset + (uint32_t)i * section_size;
		uint32_t name_offset = app_icon_read_u32(section);
		uint32_t offset = app_icon_read_u32(section + 16);
		uint32_t size = app_icon_read_u32(section + 20);
		if (name_offset >= string_size || offset > file_size || size > file_size - offset)
			continue;
		
		const char *name = strings + name_offset;
		size_t remaining = string_size - name_offset;
		if (!memchr(name, '\0', remaining) || strcmp(name, wanted))
			continue;
		
		*out_offset = offset;
		*out_size = size;
		*out_addr = app_icon_read_u32(section + 12);
		return true;
	}
	return false;
}

static bool	load_app_icon(const char *path, Image *image, char *app_name, size_t app_name_size)
{
	int fd = open(path, O_READ);
	if (fd < 0)
		return false;
	int length = lseek(fd, 0, SEEK_END);
	if (length < 52 || length > 8388608 || lseek(fd, 0, SEEK_SET) < 0)
	{
		close(fd);
		return false;
	}
	unsigned char *elf = (unsigned char *)malloc((size_t)length);
	if (!elf)
	{
		close(fd);
		return false;
	}
	int read_size = 0;
	while (read_size < length)
	{
		int amount = read(fd, (char *)elf + read_size, length - read_size);
		if (amount <= 0)
			break;
		read_size += amount;
	}
	close(fd);
	bool loaded = false;
	if (read_size == length && !memcmp(elf, "\x7f" "ELF", 4) && elf[4] == 1 && elf[5] == 1)
	{
		uint32_t section_offset = app_icon_read_u32(elf + 32);
		uint16_t section_size = app_icon_read_u16(elf + 46);
		uint16_t section_count = app_icon_read_u16(elf + 48);
		uint16_t string_index = app_icon_read_u16(elf + 50);
		if (section_size >= 40 && string_index < section_count && section_offset <= (uint32_t)length &&
			(uint64_t)section_size * section_count <= (uint32_t)length - section_offset)
		{
			const unsigned char *strings_section = elf + section_offset + (uint32_t)string_index * section_size;
			uint32_t strings_offset = app_icon_read_u32(strings_section + 16);
			uint32_t strings_size = app_icon_read_u32(strings_section + 20);
			uint32_t metadata_offset = 0, metadata_size = 0, metadata_addr = 0;
			uint32_t icon_offset = 0, icon_section_size = 0, icon_addr = 0;
			if (app_icon_section(elf, length, section_offset, section_size, section_count, strings_offset, strings_size, ".appmeta", &metadata_offset, &metadata_size, &metadata_addr) &&
				app_icon_section(elf, length, section_offset, section_size, section_count, strings_offset, strings_size, ".appicon", &icon_offset, &icon_section_size, &icon_addr) && metadata_size >= sizeof(AppMetadata))
			{
				(void)metadata_addr;
				AppMetadata metadata;
				memcpy(&metadata, elf + metadata_offset, sizeof(metadata));
				uint32_t name_address = (uint32_t)(uintptr_t)metadata.name;
				for (uint16_t i = 0; app_name && app_name_size && i < section_count; ++i)
				{
					const unsigned char *section = elf + section_offset + (uint32_t)i * section_size;
					uint32_t address = app_icon_read_u32(section + 12);
					uint32_t offset = app_icon_read_u32(section + 16);
					uint32_t size = app_icon_read_u32(section + 20);
					if (name_address < address || name_address - address >= size || offset > (uint32_t)length || size > (uint32_t)length - offset)
						continue;
					
					const char *name = (const char *)elf + offset + (name_address - address);
					size_t available = size - (name_address - address);
					if (memchr(name, '\0', available))
						strlcpy(app_name, name, min(MAX_APP_NAME_WIDTH + 1, app_name_size));
					break;
				}
				uint32_t icon_address = (uint32_t)(uintptr_t)metadata.icon;
				if (metadata.magic == APP_MANIFEST_MAGIC && metadata.end_magic == APP_MANIFEST_MAGIC && metadata.icon_size &&
					icon_address >= icon_addr && icon_address - icon_addr <= icon_section_size &&
					metadata.icon_size <= icon_section_size - (icon_address - icon_addr))
					loaded = image->load_image_from_buf((char *)elf + icon_offset + (icon_address - icon_addr), metadata.icon_size) == 0;
			}
		}
	}
	free(elf);
	return loaded;
}

typedef struct
{
	Rect	touch_area;
	Image	icon;
	Text	name;
}	t_app_icon;

#endif
