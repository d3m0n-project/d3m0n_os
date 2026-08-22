#include "app_info.h"
#include "../../../compiler/sdk/lib/app/app_manifest.h"
#include "display.h"
#include "ico.h"

static char	*get_relative_ptr(const char *address, char *buffer, uint32_t rodata_addr, uint32_t rodata_offset)
{
	uint32_t offset = (uint32_t)address - rodata_addr;
	return buffer + rodata_offset + offset;
}

static int	display_app_manifest(AppMetadata *metadata, char *buffer, uint32_t rodata_addr, uint32_t rodata_offset)
{
	int version_major = (int)metadata->version;
	int version_minor = (int)((metadata->version - version_major) * 100);

	log("\tmagic:			%s\n", 0, (metadata->end_magic == metadata->magic && metadata->magic == APP_MANIFEST_MAGIC)?"valid":"invalid");
	log("\tversion:		%i.%-2i\n", 0, version_major, version_minor);
	log("\tname:			%s\n", 0, get_relative_ptr(metadata->name, buffer, rodata_addr, rodata_offset));
	log("\tauthor:			%s\n", 0, get_relative_ptr(metadata->author, buffer, rodata_addr, rodata_offset));
	log("\tdescription:		%s\n", 0, get_relative_ptr(metadata->description, buffer, rodata_addr, rodata_offset));
	log("\ticon_size:		%lu\n", 0, metadata->icon_size);
	log("\ticon_offset:		%p\n", 0, metadata->icon);

	uint8_t		*icon = (uint8_t *)get_relative_ptr((const char *)metadata->icon, buffer, rodata_addr, rodata_offset);
	ICO_icon	ico = {0};
	if (load_ico_from_buff((char *)icon, metadata->icon_size, &ico))
	{
		log("APP: Could not load app icon\n", LOG_ERROR);
		return 1;
	}
	for (uint32_t y=0; y<ico.height; y++)
		for (uint32_t x=0; x<ico.width; x++)
			put_pixel(x, y, ((uint32_t *)(uintptr_t)ico.pixels)[y * ico.width + x]);
	return 0;
}

int		parse_app_info(elf_header_32 *header, char *buffer)
{
	uint32_t shoff	 = u32(header->SECTIONS_TABLE_OFFSET);
	uint16_t shentsize = u16(header->SECTION_TABLE_ENTRY_SIZE);
	uint16_t shnum	 = u16(header->SECTION_TABLE_ENTRY_COUNT);
	uint16_t shstrndx  = u16(header->SECTION_TABLE_INDEX);

	elf_header_identification	*identification_header = &header->IDENTIFICATION;
	AppMetadata					metadata = {0};
	int							appmeta_found = 0;
	uint32_t					rodata_offset = 0;
	uint32_t					rodata_addr = 0;

	elf_section_header_32 *sh_table = (elf_section_header_32 *)(buffer + shoff);	
	elf_section_header_32 *shstr = (elf_section_header_32 *)((char *)sh_table + shstrndx * shentsize);
	if (identification_header->ENDIANNESS == 2)
	{
		parse_elf_to_lsb(0, 0, shstr);
		parse_elf_to_lsb(0, 0, sh_table);
	}

	char *shstrtab = buffer + u32(shstr->OFFSET);
	for (uint16_t i = 0; i < shnum; i++)
	{
		elf_section_header_32 *sh = (elf_section_header_32 *)((char *)sh_table + i * shentsize);
		if (identification_header->ENDIANNESS == 2)
			parse_elf_to_lsb(0, 0, sh);

		uint32_t name_offset = u32(sh->NAME);
		char *name = shstrtab + name_offset;
		if (ft_strcmp(name, ".appmeta") == 0)
		{
			// get app metadata
			unsigned char	*sh_bytes = (unsigned char *)buffer + u32(sh->OFFSET);
			ft_memcpy(&metadata, sh_bytes, u32(sh->SIZE));
			appmeta_found = 1;
		}
		else if (ft_strcmp(name, ".rodata") == 0)
		{
			// app metadata is stored inside read only data
			rodata_addr = u32(sh->ADDR);
			rodata_offset = u32(sh->OFFSET);
		}
	}
	if (appmeta_found && rodata_offset > 0)
		return display_app_manifest(&metadata, buffer, rodata_addr, rodata_offset);
	log("APP: ELF is not a valid d3m0n application, please recompile with d3c!\n", LOG_ERROR);
	return 1;
}