#include "elf.h"
#include "time.h"
#include "proc.h"
#include "app_info.h"
#include "uart.h"

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
	log("=================================================\n", 0);
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

static int read_entire_file(int fd, char **out_buf, uint32_t *out_size)
{
	uint32_t size;
	uint32_t total;
	uint32_t chunk;
	char *buf;

	if (!out_buf || !out_size)
		return (1);
	*out_buf = 0;
	*out_size = 0;
	size = lseek(fd, 0, SEEK_END);
	if (size == (uint32_t)-1 || size == 0)
		return (1);
	if (lseek(fd, 0, SEEK_SET) == (uint32_t)-1)
		return (1);
	buf = kmalloc(size);
	if (!buf)
		return (1);
	total = 0;
	while (total < size)
	{
		chunk = read(fd, buf + total, size - total);
		if (chunk == 0 || chunk == (uint32_t)-1)
		{
			kfree(buf);
			return (1);
		}
		total += chunk;
	}
	*out_buf = buf;
	*out_size = size;
	return (0);
}

static int load_image_bounds(char *file_buf, uint32_t file_size, elf_header_32 *header, char is_msb, uint32_t *out_min_vaddr, uint32_t *out_max_vaddr)
{
	uint16_t ph_count;
	uint16_t ph_entry_size;
	uint32_t ph_offset;
	uint32_t min_vaddr;
	uint32_t max_vaddr;
	uint16_t i;
	elf_program_header_32 ph;

	ph_count = u16(header->PROGRAM_HEADER_ENTRY_COUNT);
	ph_entry_size = u16(header->PROGRAM_HEADER_ENTRY_SIZE);
	ph_offset = u32(header->PROGRAM_HEADER_OFFSET);
	if (ph_count == 0 || ph_entry_size < sizeof(elf_program_header_32))
		return (1);
	if (ph_offset >= file_size)
		return (1);
	if ((uint64_t)ph_offset + (uint64_t)ph_count * (uint64_t)ph_entry_size > (uint64_t)file_size)
		return (1);
	min_vaddr = (uint32_t)-1;
	max_vaddr = 0;
	i = 0;
	while (i < ph_count)
	{
		ft_memcpy(&ph, file_buf + ph_offset + i * ph_entry_size, sizeof(elf_program_header_32));
		if (is_msb)
			parse_elf_to_lsb(0, &ph, 0);
		if (u32(ph.TYPE) == PT_LOAD)
		{
			uint32_t vaddr = u32(ph.VIRT_ADDR);
			uint32_t offset = u32(ph.OFFSET);
			uint32_t filesz = u32(ph.FILESIZE);
			uint32_t memsz = u32(ph.MEMORYSIZE);
			uint64_t end_vaddr;

			if (memsz == 0 || filesz > memsz)
				return (1);
			if ((uint64_t)offset + (uint64_t)filesz > (uint64_t)file_size)
				return (1);
			if (vaddr < min_vaddr)
				min_vaddr = vaddr;
			end_vaddr = (uint64_t)vaddr + (uint64_t)memsz;
			if (end_vaddr > (uint64_t)0xFFFFFFFFU)
				return (1);
			if (end_vaddr > (uint64_t)max_vaddr)
				max_vaddr = (uint32_t)end_vaddr;
		}
		i++;
	}
	if (min_vaddr == (uint32_t)-1 || max_vaddr <= min_vaddr)
		return (1);
	*out_min_vaddr = min_vaddr;
	*out_max_vaddr = max_vaddr;
	return (0);
}

static void	apply_relocations(char *image, char *file_buf, elf_header_32 *header, uint32_t min_vaddr)
{
	uint16_t sh_count = u16(header->SECTION_TABLE_ENTRY_COUNT);
	uint16_t sh_ent_size = u16(header->SECTION_TABLE_ENTRY_SIZE);
	uint32_t sh_off = u32(header->SECTIONS_TABLE_OFFSET);
	int32_t  load_base = (int32_t)((uint32_t)image - min_vaddr);

	log("ELF: applying relocations load_base=0x%x image=0x%x min_vaddr=0x%x\n", 0, (uint32_t)load_base, (uint32_t)image, min_vaddr);
	
	for (uint16_t i = 0; i < sh_count; i++)
	{
		elf_section_header_32 sh;
		ft_memcpy(&sh, file_buf + sh_off + i * sh_ent_size, sizeof(sh));

		uint32_t type = u32(sh.TYPE);
		if (type != SHT_REL && type != SHT_RELA)
			continue;

		uint32_t rel_off  = u32(sh.OFFSET);
		uint32_t rel_size = u32(sh.SIZE);
		uint32_t ent_sz   = u32(sh.ENTSIZE);
		if (ent_sz == 0)
			continue;

		uint32_t count = rel_size / ent_sz;
		for (uint32_t j = 0; j < count; j++)
		{
			char	*entry	 = file_buf + rel_off + j * ent_sz;
			uint32_t rel_vaddr = u32(entry + 0);
			uint32_t info	  = u32(entry + 4);
			uint8_t  rel_type  = (uint8_t)(info & 0xFF);

			uint32_t *patch = (uint32_t *)(image + (rel_vaddr - min_vaddr));
			if (rel_type == R_ARM_ABS32 || rel_type == R_ARM_RELATIVE)
				*patch = (uint32_t)((int32_t)*patch + load_base);
		}
	}
}

static void load_pt_segments(char *image, char *file_buf, elf_header_32 *header, char is_msb, uint32_t min_vaddr)
{
	uint16_t ph_count;
	uint16_t ph_entry_size;
	uint32_t ph_offset;
	uint16_t i;
	elf_program_header_32 ph;

	ph_count = u16(header->PROGRAM_HEADER_ENTRY_COUNT);
	ph_entry_size = u16(header->PROGRAM_HEADER_ENTRY_SIZE);
	ph_offset = u32(header->PROGRAM_HEADER_OFFSET);
	i = 0;
	while (i < ph_count)
	{
		ft_memcpy(&ph, file_buf + ph_offset + i * ph_entry_size, sizeof(elf_program_header_32));
		if (is_msb)
			parse_elf_to_lsb(0, &ph, 0);
		if (u32(ph.TYPE) == PT_LOAD)
		{
			uint32_t vaddr = u32(ph.VIRT_ADDR);
			uint32_t offset = u32(ph.OFFSET);
			uint32_t filesz = u32(ph.FILESIZE);

			ft_memcpy(image + (vaddr - min_vaddr), file_buf + offset, filesz);
		}
		i++;
	}
}

static char *proc_name_from_path(char *path)
{
	char *name;

	if (!path)
		return ((char *)"elf");
	name = ft_strrchr(path, '/');
	if (name && *(name + 1))
		return (name + 1);
	if (*path)
		return (path);
	return ((char *)"elf");
}

t_process	*elf_to_proc(char *elf_path)
{
	int				fd;
	char			*file_buf;
	char			*image;
	char			is_msb;
	uint32_t		file_size;
	uint32_t		min_vaddr;
	uint32_t		max_vaddr;
	uint32_t		entry_vaddr;
	void			(*entry)(void);
	elf_header_32	header;
	t_process		*proc;

	fd = open(elf_path, O_READ);
	if (fd < 0)
	{
		log("ELF: Could not open file '%s'\n", LOG_ERROR, elf_path);
		return (0);
	}
	if (read_entire_file(fd, &file_buf, &file_size) != 0)
	{
		close(fd);
		log("ELF: Failed to read file '%s'\n", LOG_ERROR, elf_path);
		return (0);
	}
	close(fd);
	if (file_size < sizeof(elf_header_32))
	{
		kfree(file_buf);
		log("ELF: File too small '%s'\n", LOG_ERROR, elf_path);
		return (0);
	}
	if (ft_memcmp(file_buf, ELF_SIG, 4) != 0)
	{
		kfree(file_buf);
		log("ELF: Invalid signature '%s'\n", LOG_ERROR, elf_path);
		return (0);
	}
	ft_memcpy(&header, file_buf, sizeof(elf_header_32));
	print_elf_file_report(&header);
	if (header.IDENTIFICATION.BITS_ARCH != 1)
	{
		kfree(file_buf);
		log("ELF: Only 32-bit executables are supported\n", LOG_ERROR);
		return (0);
	}
	if (header.IDENTIFICATION.ENDIANNESS != 1 && header.IDENTIFICATION.ENDIANNESS != 2)
	{
		kfree(file_buf);
		log("ELF: Unsupported endianness\n", LOG_ERROR);
		return (0);
	}
	is_msb = (header.IDENTIFICATION.ENDIANNESS == 2);
	if (is_msb)
		parse_elf_to_lsb(&header, 0, 0);
	if (u16(header.FILE_TYPE) != ET_EXEC)
	{
		kfree(file_buf);
		log("ELF: File is not executable '%s'\n", LOG_ERROR, elf_path);
		return (0);
	}
	if (load_image_bounds(file_buf, file_size, &header, is_msb, &min_vaddr, &max_vaddr) != 0)
	{
		kfree(file_buf);
		log("ELF: Invalid PT_LOAD layout '%s'\n", LOG_ERROR, elf_path);
		return (0);
	}
	// ensure app validity
	if (parse_app_info(&header, file_buf))
	{
		kfree(file_buf);
		log("ELF: Invalid App, please recompile using d3c: '%s'\n", LOG_ERROR, elf_path);
		return 0;
	}

	image = ft_calloc((size_t)(max_vaddr - min_vaddr) + USER_HEAP_RESERVED + 8, 1);
	if (!image)
	{
		kfree(file_buf);
		log("ELF: Could not allocate image memory\n", LOG_ERROR);
		return (0);
	}
	load_pt_segments(image, file_buf, &header, is_msb, min_vaddr);
	apply_relocations(image, file_buf, &header, min_vaddr);
	log("ELF: min_vaddr=0x%x max_vaddr=0x%x image=0x%x\n", 0, min_vaddr, max_vaddr, (uint32_t)image);
	entry_vaddr = u32(header.ENTRY_POINT);
	if (entry_vaddr < min_vaddr || entry_vaddr >= max_vaddr)
	{
		kfree(file_buf);
		kfree(image);
		log("ELF: Entry point outside loaded image\n", LOG_ERROR);
		return (0);
	}
	entry = (void (*)(void))(uintptr_t)(image + (entry_vaddr - min_vaddr));
	proc = process_create(entry, proc_name_from_path(elf_path), 0);
	if (!proc)
	{
		kfree(file_buf);
		kfree(image);
		return (0);
	}
	proc->address_space = image;
	proc->image_vaddr_base = (uint32_t)image	;
	proc->image_size = max_vaddr - min_vaddr;
	proc->heap_start = (uint32_t)(((uint8_t *)image + (max_vaddr - min_vaddr)) + 7) & ~7U;
	proc->heap_end = proc->heap_start;
	kfree(file_buf);
	return (proc);
}