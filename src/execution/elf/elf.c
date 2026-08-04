#include "elf.h"

static const char *get_file_type(char file_type[2])
{
	char *types[5] = {"Unknown", "Relocatable file", "Executable file", "Shared object", "Core file"};

	uint16_t	v = u16(file_type);

	if (v > 4)
		return types[0];
	return types[v];
}
const char *get_instruction_set_name(char p[2])
{
    unsigned int v = u16(p);

    switch (v)
    {
        case 0x00: return "No specific instruction set";
        case 0x01: return "AT&T WE 32100";
        case 0x02: return "SPARC";
        case 0x03: return "x86";
        case 0x04: return "Motorola 68000 (M68k)";
        case 0x05: return "Motorola 88000 (M88k)";
        case 0x06: return "Intel MCU";
        case 0x07: return "Intel 80860";
        case 0x08: return "MIPS";
        case 0x09: return "IBM System/370";
        case 0x0A: return "MIPS RS3000 Little-endian";
        case 0x0F: return "Hewlett-Packard PA-RISC";
        case 0x13: return "Intel 80960";
        case 0x14: return "PowerPC";
        case 0x15: return "PowerPC (64-bit)";
        case 0x16: return "S390 / S390x";
        case 0x17: return "IBM SPU/SPC";
        case 0x24: return "NEC V800";
        case 0x25: return "Fujitsu FR20";
        case 0x26: return "TRW RH-32";
        case 0x27: return "Motorola RCE";
        case 0x28: return "ARM (AArch32)";
        case 0x29: return "Digital Alpha";
        case 0x2A: return "SuperH";
        case 0x2B: return "SPARC v9";
        case 0x2C: return "Siemens TriCore";
        case 0x2D: return "Argonaut RISC Core";
        case 0x2E: return "Hitachi H8/300";
        case 0x2F: return "Hitachi H8/300H";
        case 0x30: return "Hitachi H8S";
        case 0x31: return "Hitachi H8/500";
        case 0x32: return "IA-64";
        case 0x33: return "Stanford MIPS-X";
        case 0x34: return "Motorola ColdFire";
        case 0x35: return "Motorola M68HC12";
        case 0x36: return "Fujitsu MMA";
        case 0x37: return "Siemens PCP";
        case 0x38: return "Sony nCPU";
        case 0x39: return "Denso NDR1";
        case 0x3A: return "Motorola Star*Core";
        case 0x3B: return "Toyota ME16";
        case 0x3C: return "STMicroelectronics ST100";
        case 0x3D: return "ALC TinyJ";
        case 0x3E: return "AMD x86-64";
        case 0x3F: return "Sony DSP";
        case 0x40: return "DEC PDP-10";
        case 0x41: return "DEC PDP-11";
        case 0x42: return "Siemens FX66";
        case 0x43: return "STMicroelectronics ST9+";
        case 0x44: return "STMicroelectronics ST7";
        case 0x45: return "Motorola MC68HC16";
        case 0x46: return "Motorola MC68HC11";
        case 0x47: return "Motorola MC68HC08";
        case 0x48: return "Motorola MC68HC05";
        case 0x49: return "SGI SVx";
        case 0x4A: return "STMicroelectronics ST19";
        case 0x4B: return "Digital VAX";
        case 0x4C: return "Axis Communications 32-bit";
        case 0x4D: return "Infineon 32-bit";
        case 0x4E: return "Element 14 64-bit DSP";
        case 0x4F: return "LSI Logic 16-bit DSP";
        case 0x8C: return "TMS320C6000";
        case 0xAF: return "MCST Elbrus e2k";
        case 0xB7: return "ARM AArch64";
        case 0xDC: return "Zilog Z80";
        case 0xF3: return "RISC-V";
        case 0xF7: return "Berkeley Packet Filter";
        case 0x101: return "WDC 65C816";
        case 0x102: return "LoongArch";
        default: return "Unknown / Reserved";
    }
}
static const char	*get_abi_name(int value)
{
	switch (value)
	{
		case 0: return "UNIX System V";
		case 1: return "HP-UX";
		case 2: return "NetBSD";
		case 3: return "Linux";
		case 6: return "Sun Solaris";
		case 7: return "IBM AIX";
		case 8: return "SGI Irix";
		case 9: return "FreeBSD";
		case 10: return "Compaq TRU64";
		case 11: return "Novell Modesto";
		case 12: return "OpenBSD";
		case 64: return "ARM EABI";
		case 97: return "ARM";
		case 255: return "Standalone";
		default: return "Unknown";
	}
}

uint16_t	u16(const char b[2])
{
    return (uint16_t)(uint8_t)b[0] | ((uint16_t)(uint8_t)b[1] << 8);
}

uint32_t	u32(const char b[4])
{
    return (uint32_t)(uint8_t)b[0] | ((uint32_t)(uint8_t)b[1] << 8) | ((uint32_t)(uint8_t)b[2] << 16) | ((uint32_t)(uint8_t)b[3] << 24);
}

void	print_elf_file_report(elf_header_32 *header)
{
	log("ELF file:\n", 0);
	log("\tBITS:			%s\n", 0, (header->IDENTIFICATION.BITS_ARCH == 2)?"64":"32");
	log("\tENDIANNESS:		%s\n", 0, (header->IDENTIFICATION.ENDIANNESS == 2)?"MSB":"LSB");
	log("\tHEADER VERSION:		%i\n", 0, header->IDENTIFICATION.VERSION);
	log("\tABI:			%s\n", 0, get_abi_name(header->IDENTIFICATION.ABI));
	log("\tABI VERSION:		%i\n", 0, header->IDENTIFICATION.ABI_VERSION);

	log("\tFILE TYPE:		%s\n", 0, get_file_type(header->FILE_TYPE));
	log("\tINSTRUCTION SET:	%s\n", 0, get_instruction_set_name(header->TARGET_SYSTEM));
	log("=================================================\n\n", 0);
}


void	reverse_bytes(char *str, size_t len)
{
	size_t	i = 0;
	char	tmp;
	while (i < len / 2)
	{
		tmp = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = tmp;
		i++;
	}
}
void	parse_elf_to_lsb(elf_header_32 *header, elf_program_header_32 *prog_h, elf_section_header_32 *section_h)
{
	// parse bytes to lsb
	if (header)
	{
		reverse_bytes(header->FILE_TYPE, 2);
		reverse_bytes(header->TARGET_SYSTEM, 2);
		reverse_bytes(header->VERSION, 4);
		reverse_bytes(header->ENTRY_POINT, 4);
		reverse_bytes(header->PROGRAM_HEADER_OFFSET, 4);
		reverse_bytes(header->SECTIONS_TABLE_OFFSET, 4);
		reverse_bytes(header->CPU_FLAGS, 4);
		reverse_bytes(header->PROGRAM_HEADER_SIZE, 2);
		reverse_bytes(header->PROGRAM_HEADER_ENTRY_SIZE, 2);
		reverse_bytes(header->PROGRAM_HEADER_ENTRY_COUNT, 2);
		reverse_bytes(header->SECTION_TABLE_ENTRY_SIZE, 2);
		reverse_bytes(header->SECTION_TABLE_ENTRY_COUNT, 2);
		reverse_bytes(header->SECTION_TABLE_INDEX, 2);
	}
	if (prog_h)
	{
		reverse_bytes(prog_h->TYPE, 4);
		reverse_bytes(prog_h->OFFSET, 4);
		reverse_bytes(prog_h->VIRT_ADDR, 4);
		reverse_bytes(prog_h->PHY_ADDR, 4);
		reverse_bytes(prog_h->FILESIZE, 4);
		reverse_bytes(prog_h->MEMORYSIZE, 4);
		reverse_bytes(prog_h->FLAGS, 4);
		reverse_bytes(prog_h->ALIGN, 4);
	}
	if (section_h)
	{
		reverse_bytes(section_h->NAME, 4);
		reverse_bytes(section_h->TYPE, 4);
		reverse_bytes(section_h->FLAGS, 4);
		reverse_bytes(section_h->ADDR, 4);
		reverse_bytes(section_h->OFFSET, 4);
		reverse_bytes(section_h->SIZE, 4);
		reverse_bytes(section_h->LINK, 4);
		reverse_bytes(section_h->INFO, 4);
		reverse_bytes(section_h->ADDRALIGN, 4);
		reverse_bytes(section_h->ENTSIZE, 4);
	}
}