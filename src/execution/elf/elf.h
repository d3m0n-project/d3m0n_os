#ifndef ELF_H
#define ELF_H

#include "log.h"
#include "memory.h"
#include "types.h"
#include "filesystem.h"

#define ELF_SIG "\x7F""ELF"

#pragma pack(push, 1)
typedef struct elf_header_identification
{
	char SIGNATURE[4];
	char BITS_ARCH;
	char ENDIANNESS;
	char VERSION;
	char ABI;
	char ABI_VERSION;
	char pad1[7];
}	elf_header_identification;

typedef struct elf_program_header_32
{
	char	TYPE[4];
	char	OFFSET[4];
	char	VIRT_ADDR[4];
	char	PHY_ADDR[4];
	char	FILESIZE[4];
	char	MEMORYSIZE[4];
	char	FLAGS[4];
	char	ALIGN[4];
}	elf_program_header_32;

typedef struct elf_section_header_32
{
	char	NAME[4];
	char	TYPE[4];
	char	FLAGS[4];
	char	ADDR[4];	 
	char	OFFSET[4];
	char	SIZE[4];
	char	LINK[4];
	char	INFO[4];
	char	ADDRALIGN[4];
	char	ENTSIZE[4];
}	elf_section_header_32;


typedef struct elf_header_32
{
	elf_header_identification	IDENTIFICATION;
	char	FILE_TYPE[2];
	char	TARGET_SYSTEM[2];
	char	VERSION[4];
	char	ENTRY_POINT[4];
	char	PROGRAM_HEADER_OFFSET[4];
	char	SECTIONS_TABLE_OFFSET[4];
	char	CPU_FLAGS[4];
	char	PROGRAM_HEADER_SIZE[2];
	char	PROGRAM_HEADER_ENTRY_SIZE[2];
	char	PROGRAM_HEADER_ENTRY_COUNT[2];
	char	SECTION_TABLE_ENTRY_SIZE[2];
	char	SECTION_TABLE_ENTRY_COUNT[2];
	char	SECTION_TABLE_INDEX[2];
}	elf_header_32;

#pragma pack(pop)


/* functions */
int			detect_type(int fd);
void		print_elf_file_report(elf_header_32 *header);
void		parse_elf_to_lsb(elf_header_32 *header, elf_program_header_32 *prog_h, elf_section_header_32 *section_h);

/* utils */
uint16_t	u16(const char b[2]);
uint32_t	u32(const char b[4]);

#endif
