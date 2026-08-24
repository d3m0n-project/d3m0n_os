#ifndef ELF_H
#define ELF_H

#include "log.h"
#include "memory.h"
#include "types.h"
#include "filesystem.h"
#include "proc.h"

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

typedef enum
{
	ET_NONE=0,        // No file type
	ET_REL=1,         // Relocatable file
	ET_EXEC=2,        // Executable file
	ET_DYN=3,         // Shared object file
	ET_CORE=4,        // Core file
	ET_LOOS=0xfe00,   // Operating system-specific
	ET_HIOS=0xfeff,   // Operating system-specific
	ET_LOPROC=0xff00, // Processor-specific
	ET_HIPROC=0xffff  // Processor-specific
}	e_elf_header_type;

typedef enum
{
	PT_NULL=0,
	PT_LOAD=1,
	PT_DYNAMIC=2,
	PT_INTERP=3,
	PT_NOTE=4,
	PT_SHLIB=5,
	PT_PHDR=6,
	PT_TLS=7,
	PT_LOOS=0x60000000,
	PT_HIOS=0x6fffffff,
	PT_LOPROC=0x70000000,
	PT_HIPROC=0x7fffffff
}	e_elf_program_header_type;

#define SHT_REL				9
#define SHT_RELA			4
#define R_ARM_ABS32			2
#define R_ARM_RELATIVE		23


/* functions */
int					parse_elf_headers(int fd);
struct s_process	*elf_to_proc(char *elf_path);
void				print_elf_file_report(elf_header_32 *header);
void				parse_elf_to_lsb(elf_header_32 *header, elf_program_header_32 *prog_h, elf_section_header_32 *section_h);


/* utils */
uint16_t			u16(const char b[2]);
uint32_t			u32(const char b[4]);

#endif
