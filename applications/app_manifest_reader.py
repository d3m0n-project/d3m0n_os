import sys
import struct

elf = sys.argv[1]

with open(elf, "rb") as f:
	data = f.read()

# ELF identification
if data[:4] != b"\x7fELF":
	print("not an ELF file", file=sys.stderr)
	sys.exit(1)

elf_class = data[4]
endian = "<" if data[5] == 1 else ">"

if elf_class == 2:
	ptr_size = 8
	ehdr = struct.unpack_from(endian + "16sHHIQQQIHHHHHH", data, 0)
	_, _, _, _, _, shoff, _, _, _, _, shentsize, shnum, shstrndx, _ = ehdr
else:
	ptr_size = 4
	ehdr = struct.unpack_from(endian + "16sHHIIIIIHHHHHH", data, 0)
	_, _, _, _, _, shoff, _, _, _, _, shentsize, shnum, shstrndx, _ = ehdr

# Read section headers
sections = []

for i in range(shnum):
	off = shoff + i * shentsize

	if elf_class == 2:
		sh = struct.unpack_from(endian + "IIQQQQIIQQ", data, off)
		name, typ, flags, addr, file_off, size, link, info, align, entsize = sh
	else:
		sh = struct.unpack_from(endian + "IIIIIIIIII", data, off)
		name, typ, flags, addr, file_off, size, link, info, align, entsize = sh

	sections.append({
		"name": name,
		"addr": addr,
		"offset": file_off,
		"size": size
	})

# Resolve section names
strtab = sections[shstrndx]
strings = data[strtab["offset"]:strtab["offset"] + strtab["size"]]

for section in sections:
	off = section["name"]
	end = strings.find(b"\0", off)
	section["name"] = strings[off:end].decode(errors="replace")

def section(name):
	for s in sections:
		if s["name"] == name:
			return s
	return None

appmeta = section(".appmeta")
appicon = section(".appicon")

if not appmeta:
	print("missing .appmeta", file=sys.stderr)
	sys.exit(1)

if not appicon:
	print("missing .appicon", file=sys.stderr)
	sys.exit(1)

meta = data[
	appmeta["offset"]:
	appmeta["offset"] + appmeta["size"]
]

# AppMetadata:
#
# uint32_t magic
# float    version
# pointer  name
# pointer  author
# pointer  description
# uint32_t icon_size
# pointer  icon
# uint32_t end_magic
#
# There is ABI-dependent padding here.
# Use the native compiler ABI layout.

if ptr_size == 8:
	# uint32 + float + 4 bytes padding + 3 pointers +
	# uint32 + 4 bytes padding + pointer + uint32
	magic, version = struct.unpack_from(endian + "If", meta, 0)
	name_ptr, author_ptr, desc_ptr = struct.unpack_from(endian + "QQQ", meta, 8)
	icon_size = struct.unpack_from(endian + "I", meta, 32)[0]
	icon_ptr = struct.unpack_from(endian + "Q", meta, 40)[0]
	end_magic = struct.unpack_from(endian + "I", meta, 48)[0]
else:
	magic, version = struct.unpack_from(endian + "If", meta, 0)
	name_ptr, author_ptr, desc_ptr = struct.unpack_from(endian + "III", meta, 8)
	icon_size = struct.unpack_from(endian + "I", meta, 20)[0]
	icon_ptr = struct.unpack_from(endian + "I", meta, 24)[0]
	end_magic = struct.unpack_from(endian + "I", meta, 28)[0]

def read_at_address(addr):
	for s in sections:
		if s["addr"] <= addr < s["addr"] + s["size"]:
			offset = s["offset"] + (addr - s["addr"])
			return data[offset:]

	raise RuntimeError("cannot resolve address 0x%x" % addr)

def read_string(addr):
	raw = read_at_address(addr)
	return raw[:raw.find(b"\0")].decode(errors="replace")

print("magic=0x%08x" % magic)
print("version=%s" % version)
print("name=%s" % read_string(name_ptr))
print("author=%s" % read_string(author_ptr))
print("description=%s" % read_string(desc_ptr))
print("icon_size=%d" % icon_size)
print("icon_address=0x%x" % icon_ptr)
print("end_magic=0x%08x" % end_magic)