#include "elf.h"

char	elf_get_binary_code(int fd)
{
	char	*buffer;
	size_t	buffer_size = 5;
	size_t	bytes_read;
	size_t	total_read = 0;

	buffer = malloc(buffer_size);
	if (!buffer)
		return 1;

	while ((bytes_read = read(fd, buffer + total_read, buffer_size - total_read)) > 0)
	{
		total_read += bytes_read;
		if (total_read == buffer_size)
		{
			size_t new_size = buffer_size * 2;
			char *new_buffer = ft_realloc(buffer, total_read, new_size);
			if (!new_buffer)
			{
				free(buffer);
				return 1;
			}
			buffer = new_buffer;
			buffer_size = new_size;
		}
	}
	if (bytes_read == (size_t)-1)
	{
		free(buffer);
		return 1;
	}

	if (ft_memcmp(buffer, ELF_SIG, 4) == 0)
	{
		log("Format:	\033[35mELF\033[0m\n", 0);
		elf_header_identification	identification_header;
		// read identification header
		if (total_read < sizeof(elf_header_identification))
		{
			free(buffer);
			return 1;
		}
		ft_memcpy(&identification_header, buffer, sizeof(elf_header_identification));
		if (identification_header.BITS_ARCH == 1) // 32 bits
		{
			elf_header_32	header;
			if (total_read < sizeof(elf_header_32))
			{
				free(buffer);
				return 1;
			}
			ft_memcpy(&header, buffer, sizeof(elf_header_32));

			if (identification_header.ENDIANNESS == 2)
				parse_elf_to_lsb(&header, 0, 0);
			print_elf_file_report(&header);

			// display program headers
			elf_program_header_32	*program_headers = (elf_program_header_32 *)(buffer + u32(header.PROGRAM_HEADER_OFFSET));
			uint32_t	base_offset = -1;
			for (int i=0; i<u16(header.PROGRAM_HEADER_ENTRY_COUNT); i++)
			{
				log("Program Header [%i]:\n", 0, i);
				log("\t- OFFSET:		%x\n", 0, u32(program_headers[i].OFFSET));
				log("\t- PHYSICAL_ADDR:	%p\n", 0, (uint64_t)u32(program_headers[i].PHY_ADDR));
				log("\n", 0);

				if (u32(program_headers[i].OFFSET) == 0)
					base_offset = u32(program_headers[i].PHY_ADDR);
			}
			if (base_offset == (unsigned int)-1)
			{
				free(buffer);
				return 1;
			}
			


			// display sections
			log("=================================================\n", 0);
			log("Program's sections:\n", 0);
			uint32_t shoff     = u32(header.SECTIONS_TABLE_OFFSET);
			uint16_t shentsize = u16(header.SECTION_TABLE_ENTRY_SIZE);
			uint16_t shnum     = u16(header.SECTION_TABLE_ENTRY_COUNT);
			uint16_t shstrndx  = u16(header.SECTION_TABLE_INDEX);

			elf_section_header_32 *sh_table = (elf_section_header_32 *)(buffer + shoff);	
			elf_section_header_32 *shstr = (elf_section_header_32 *)((char *)sh_table + shstrndx * shentsize);
			if (identification_header.ENDIANNESS == 2)
			{
				parse_elf_to_lsb(0, 0, shstr);
				parse_elf_to_lsb(0, 0, sh_table);
			}

			char *shstrtab = buffer + u32(shstr->OFFSET);
			for (uint16_t i = 0; i < shnum; i++)
			{
				elf_section_header_32 *sh = (elf_section_header_32 *)((char *)sh_table + i * shentsize);
				if (identification_header.ENDIANNESS == 2)
					parse_elf_to_lsb(0, 0, sh);

				uint32_t name_offset = u32(sh->NAME);
				char *name = shstrtab + name_offset;

				log("[%u] %s %u\n", 0, i, name, u32(sh->OFFSET));
				if (ft_strcmp(name, ".text") == 0)
				{
					// display entry point code
					unsigned char	*sh_bytes = (unsigned char *)buffer + u32(sh->OFFSET);
					for (uint32_t i=0; i<u32(sh->SIZE); i++)
						log("%x ", 0, sh_bytes[i]);
					//x86_disasm(sh_bytes, u32(sh->SIZE));
					//for (int i=0; i < u32(sh->SIZE); i++)
					//	log("%.2x ", sh_bytes[i]);
					log("\n", 0);
				}
			}
		}
		else if (identification_header.BITS_ARCH == 2) // 64 bits
		{
			log("64 bit elf are not handled yet.\n", 0);
			free(buffer);
			return 1;
		}
		else // unknown
		{
			log("Unknown elf bit format.\n", 0);
			free(buffer);
			return 1;
		}
	}
	else
	{
		// other binary files are not handled yet
		free(buffer);
		return 1; 
	}
	free(buffer);
	return 0;
}