#include "filesystem.h"
#include "fat32.h"

#define FAT32_EOC					0x0FFFFFF8U
#define FAT32_DIR_ENTRY_SIZE		32U
#define FAT32_MAX_CLUSTER_SECTORS	128U
#define FAT32_MAX_CLUSTER_BYTES		(SECTOR_SIZE * FAT32_MAX_CLUSTER_SECTORS)

#define FAT32_BOOT_SIG_OFF			(SECTOR_SIZE - 2)
#define FAT32_PART_ENTRY0_OFF		446U
#define FAT32_PART_LBA_OFF			8U

#define BPB_BYTSPERSEC_OFF			11U
#define BPB_SECPERCLUS_OFF			13U
#define BPB_RSVDSECCNT_OFF			14U
#define BPB_NUMFATS_OFF				16U
#define BPB_ROOTENTCNT_OFF			17U
#define BPB_TOTSEC16_OFF			19U
#define BPB_FATSZ16_OFF				22U
#define BPB_TOTSEC32_OFF			32U
#define BPB_FATSZ32_OFF				36U
#define BPB_ROOTCLUS_OFF			44U

#define DIR_NAME_OFF				0U
#define DIR_ATTR_OFF				11U
#define DIR_FSTCLUSHI_OFF			20U
#define DIR_FSTCLUSLO_OFF			26U
#define DIR_FILESIZE_OFF			28U

#define FAT32_ATTR_DIRECTORY		0x10U
#define FAT32_ATTR_LFN				0x0FU
#define FAT32_LFN_SEQ_LAST			0x40U
#define FAT32_LFN_MAX_ENTRIES		20U
#define FAT32_LFN_MAX_LEN			(FAT32_LFN_MAX_ENTRIES * 13U + 1U)

static const uint8_t g_lfn_char_off[13] = {1,3,5,7,9,14,16,18,20,22,24,28,30};

typedef struct s_fat32_info
{
	uint32_t	fat_start;
	uint32_t	data_start;
	uint32_t	root_cluster;
	uint32_t	fat_size;
	uint8_t		num_fats;
	uint8_t		sectors_per_cluster;
}	FAT32_Info;

static FAT32_Info	fat32;
static uint8_t		g_cluster_buf[FAT32_MAX_CLUSTER_BYTES];

static uint16_t mem_read16(const uint8_t *p)
{
    return p[0] | (p[1] << 8);
}

static void mem_write16(uint8_t *p, uint16_t v)
{
	p[0] = (uint8_t)(v & 0xFF);
	p[1] = (uint8_t)((v >> 8) & 0xFF);
}

static void mem_write32(uint8_t *p, uint32_t v)
{
	p[0] = (uint8_t)(v & 0xFF);
	p[1] = (uint8_t)((v >> 8) & 0xFF);
	p[2] = (uint8_t)((v >> 16) & 0xFF);
	p[3] = (uint8_t)((v >> 24) & 0xFF);
}

static uint32_t mem_read32(const uint8_t *p)
{
    return p[0] |
           (p[1] << 8) |
           (p[2] << 16) |
           (p[3] << 24);
}

static int	is_pow2_u32(uint32_t value)
{
	return (value != 0 && (value & (value - 1U)) == 0);
}

static uint32_t	cluster_bytes(void)
{
	return (uint32_t)fat32.sectors_per_cluster * SECTOR_SIZE;
}

static int	fat32_ready(void)
{
	if (fat32.sectors_per_cluster == 0)
		return 0;
	if (fat32.sectors_per_cluster > FAT32_MAX_CLUSTER_SECTORS)
		return 0;
	if (fat32.root_cluster < 2)
		return 0;
	return 1;
}

static uint32_t	fat32_max_clusters(void)
{
	if (!fat32_ready() || fat32.fat_size == 0)
		return 0;
	return (fat32.fat_size * SECTOR_SIZE) / 4U;
}

uint32_t	fat32_cluster_to_lba(uint32_t cluster)
{
	if (!fat32_ready() || cluster < 2)
		return 0;
	return fat32.data_start + (cluster - 2) * fat32.sectors_per_cluster;
}

static char	to_upper_char(char c)
{
	if (c >= 'a' && c <= 'z')
		return (char)(c - ('a' - 'A'));
	return c;
}

static void mem_copy(uint8_t *dst, const uint8_t *src, uint32_t n)
{
	uint32_t i;

	for (i = 0; i < n; i++)
		dst[i] = src[i];
}

static void mem_zero(uint8_t *dst, uint32_t n)
{
	uint32_t i;

	for (i = 0; i < n; i++)
		dst[i] = 0;
}

static int parse_bpb(uint32_t lba)
{
	uint8_t		boot_sector[SECTOR_SIZE];
	uint32_t	rsvd_sec_cnt;
	uint32_t	num_fats;
	uint32_t	root_ent_cnt;
	uint32_t	fat_sz16;
	uint32_t	fat_sz32;
	uint32_t	root_clus;
	uint32_t	bytes_per_sec;
	uint32_t	sec_per_clus;
	uint32_t	tot_sec16;
	uint32_t	tot_sec32;
	uint32_t	total_sectors;
	uint32_t	data_sectors;
	uint32_t	cluster_count;

	if (block_read(lba, boot_sector) != 0)
		return -1;
	if (boot_sector[FAT32_BOOT_SIG_OFF] != 0x55
		|| boot_sector[FAT32_BOOT_SIG_OFF + 1] != 0xAA)
		return -1;

	bytes_per_sec = mem_read16(&boot_sector[BPB_BYTSPERSEC_OFF]);
	sec_per_clus = boot_sector[BPB_SECPERCLUS_OFF];
	rsvd_sec_cnt = mem_read16(&boot_sector[BPB_RSVDSECCNT_OFF]);
	num_fats = boot_sector[BPB_NUMFATS_OFF];
	root_ent_cnt = mem_read16(&boot_sector[BPB_ROOTENTCNT_OFF]);
	fat_sz16 = mem_read16(&boot_sector[BPB_FATSZ16_OFF]);
	tot_sec16 = mem_read16(&boot_sector[BPB_TOTSEC16_OFF]);
	tot_sec32 = mem_read32(&boot_sector[BPB_TOTSEC32_OFF]);
	fat_sz32 = mem_read32(&boot_sector[BPB_FATSZ32_OFF]);
	root_clus = mem_read32(&boot_sector[BPB_ROOTCLUS_OFF]);

	if (bytes_per_sec != SECTOR_SIZE || !is_pow2_u32(sec_per_clus)
		|| sec_per_clus > FAT32_MAX_CLUSTER_SECTORS
		|| rsvd_sec_cnt == 0 || num_fats == 0
		|| root_ent_cnt != 0 || fat_sz16 != 0
		|| fat_sz32 == 0 || root_clus < 2)
		return -1;

	total_sectors = (tot_sec16 != 0) ? tot_sec16 : tot_sec32;
	if (total_sectors == 0)
		return -1;

	if (rsvd_sec_cnt + (num_fats * fat_sz32) >= total_sectors)
		return -1;

	data_sectors = total_sectors - (rsvd_sec_cnt + (num_fats * fat_sz32));
	cluster_count = data_sectors / sec_per_clus;
	if (cluster_count < 65525U)
		return -1;
	if (root_clus >= cluster_count + 2U)
		return -1;

	fat32.sectors_per_cluster = (uint8_t)sec_per_clus;
	fat32.num_fats = (uint8_t)num_fats;
	fat32.fat_size = fat_sz32;
	fat32.fat_start = lba + rsvd_sec_cnt;
	fat32.root_cluster = root_clus;
	fat32.data_start = fat32.fat_start + (num_fats * fat_sz32);

	return 0;
}

static int fat32_write_cluster(uint32_t cluster, const uint8_t *buffer)
{
	uint32_t lba;
	uint32_t i;

	if (!buffer || !fat32_ready() || cluster < 2)
		return -1;

	lba = fat32_cluster_to_lba(cluster);
	if (lba == 0)
		return -1;

	for (i = 0; i < fat32.sectors_per_cluster; i++)
	{
		if (block_write(lba + i, buffer + i * SECTOR_SIZE) != 0)
			return -1;
	}
	return 0;
}

static int fat32_set_fat_entry(uint32_t cluster, uint32_t value)
{
	uint32_t fat_offset;
	uint32_t sector_off;
	uint32_t byte_off;
	uint32_t fat_index;
	uint32_t lba;
	uint32_t v;

	if (!fat32_ready() || cluster < 2)
		return -1;

	fat_offset = cluster * 4U;
	sector_off = fat_offset / SECTOR_SIZE;
	byte_off = fat_offset % SECTOR_SIZE;
	v = value & 0x0FFFFFFFU;

	for (fat_index = 0; fat_index < fat32.num_fats; fat_index++)
	{
		lba = fat32.fat_start + fat_index * fat32.fat_size + sector_off;
		if (block_read(lba, g_cluster_buf) != 0)
			return -1;
		mem_write32(g_cluster_buf + byte_off, v);
		if (block_write(lba, g_cluster_buf) != 0)
			return -1;
	}
	return 0;
}

static int fat32_find_free_cluster(uint32_t *cluster_out)
{
	uint32_t max_entries;
	uint32_t cluster;
	uint32_t fat_sector;
	uint32_t byte_off;
	uint32_t value;

	if (!cluster_out || !fat32_ready())
		return -1;

	max_entries = (fat32.fat_size * SECTOR_SIZE) / 4U;
	for (cluster = 2; cluster < max_entries; cluster++)
	{
		fat_sector = fat32.fat_start + (cluster * 4U) / SECTOR_SIZE;
		byte_off = (cluster * 4U) % SECTOR_SIZE;
		if (block_read(fat_sector, g_cluster_buf) != 0)
			return -1;
		value = mem_read32(g_cluster_buf + byte_off) & 0x0FFFFFFFU;
		if (value == 0)
		{
			*cluster_out = cluster;
			return 0;
		}
	}
	return -1;
}

int	fat32_read_cluster(uint32_t cluster, uint8_t *buffer)
{
	uint32_t	lba;
	uint32_t	i;

	if (!buffer || !fat32_ready() || cluster < 2)
		return -1;

	lba = fat32_cluster_to_lba(cluster);
	if (lba == 0)
		return -1;

	for (i = 0; i < fat32.sectors_per_cluster; i++)
	{
		if (block_read(lba + i, buffer + i * SECTOR_SIZE) != 0)
			return -1;
	}
	return 0;
}

int	fat32_get_next_cluster(uint32_t cluster, uint32_t *next)
{
	uint32_t fat_sector;
	uint32_t offset;
	uint8_t sector[SECTOR_SIZE];
	uint32_t value;
	uint32_t max_clusters;

	if (!next || !fat32_ready() || cluster < 2)
		return -1;

	max_clusters = fat32_max_clusters();
	if (cluster >= max_clusters)
		return -1;
	
	fat_sector = fat32.fat_start + (cluster * 4U) / SECTOR_SIZE;
	offset = (cluster * 4U) % SECTOR_SIZE;

	if (block_read(fat_sector, sector) != 0)
		return -1;

	value = mem_read32(&sector[offset]) & 0x0FFFFFFF;

	if (value >= FAT32_EOC)
		value = 0;

	if (value != 0 && (value < 2 || value >= max_clusters))
		return -1;

	*next = value;
	return 0;
}



static int fat32_create_dir_entry(uint32_t dir_cluster, const uint8_t name83[11], uint32_t first_cluster)
{
	uint32_t cluster;
	uint32_t entries;
	uint32_t i;
	uint8_t *entry;
	uint32_t hops;
	uint32_t max_clusters;

	if (!name83 || first_cluster < 2 || !fat32_ready() || dir_cluster < 2)
		return -1;

	cluster = dir_cluster;
	hops = 0;
	max_clusters = fat32_max_clusters();
	while (cluster)
	{
		if (max_clusters == 0 || hops++ >= max_clusters)
			return -1;
		if (fat32_read_cluster(cluster, g_cluster_buf) != 0)
			return -1;

		entries = cluster_bytes() / FAT32_DIR_ENTRY_SIZE;
		for (i = 0; i < entries; i++)
		{
			entry = &g_cluster_buf[i * FAT32_DIR_ENTRY_SIZE];

			if (entry[DIR_NAME_OFF] == 0x00 || entry[DIR_NAME_OFF] == 0xE5)
			{
				mem_zero(entry, FAT32_DIR_ENTRY_SIZE);
				mem_copy(entry + DIR_NAME_OFF, name83, 11);
				entry[DIR_ATTR_OFF] = 0x20;
				mem_write16(entry + DIR_FSTCLUSHI_OFF, (uint16_t)(first_cluster >> 16));
				mem_write16(entry + DIR_FSTCLUSLO_OFF, (uint16_t)(first_cluster & 0xFFFF));
				mem_write32(entry + DIR_FILESIZE_OFF, 0);
				return fat32_write_cluster(cluster, g_cluster_buf);
			}
		}

		if (fat32_get_next_cluster(cluster, &cluster) != 0)
			return -1;
	}

	return -1;
}

static void lfn_fill(const uint8_t *lfn_entry, char *buf, uint32_t seq_1based)
{
	uint32_t	base;
	uint32_t	k;
	uint8_t		lo;

	base = (seq_1based - 1U) * 13U;
	for (k = 0; k < 13U; k++)
	{
		lo = lfn_entry[g_lfn_char_off[k]];
		if (lo == 0x00U || lo == 0xFFU)
			buf[base + k] = '\0';
		else
			buf[base + k] = (char)lo;
	}
}

static int lfn_match(const char *lfn_buf, const char *comp)
{
	uint32_t i = 0;

	while (lfn_buf[i] && comp[i])
	{
		if (to_upper_char(lfn_buf[i]) != to_upper_char(comp[i]))
			return 0;
		i++;
	}
	return (lfn_buf[i] == '\0' && comp[i] == '\0');
}

static int	fat32_name_to_83(const char *name, uint8_t out[11])
{
	uint32_t	i;
	uint32_t	base_len;
	uint32_t	ext_len;
	uint32_t	pos;

	if (!name)
		return -1;

	if (name[0] == '/')
		name++;

	for (i = 0; i < 11; i++)
		out[i] = ' ';

	base_len = 0;
	ext_len = 0;
	pos = 0;

	while (name[pos] && name[pos] != '.' && name[pos] != '/')
	{
		if (base_len >= 8)
			return -1;
		out[base_len++] = (uint8_t)to_upper_char(name[pos]);
		pos++;
	}

	if (name[pos] == '/')
		return -1;

	if (name[pos] == '.')
	{
		pos++;
		while (name[pos] && name[pos] != '/')
		{
			if (ext_len >= 3)
				return -1;
			out[8 + ext_len] = (uint8_t)to_upper_char(name[pos]);
			ext_len++;
			pos++;
		}
		if (name[pos] == '/')
			return -1;
	}

	return 0;
}

static int	fat32_name_match(const uint8_t entry_name[11], const char *filename)
{
	uint8_t		name83[11];
	uint32_t	i;

	if (fat32_name_to_83(filename, name83) < 0)
		return 0;
	for (i = 0; i < 11; i++)
	{
		if (entry_name[i] != name83[i])
			return 0;
	}
	return 1;
}

static int fat32_search_dir(uint32_t start_cluster, const char *name, FAT32_File *out)
{
	uint32_t	cluster;
	uint32_t	hops;
	uint32_t	max_clusters;
	uint32_t	entries;
	uint32_t	i;
	uint8_t		*entry;
	uint8_t		attr;
	uint8_t		seq;
	char		lfn_buf[FAT32_LFN_MAX_LEN];
	int			lfn_valid;

	if (!name || !out || !fat32_ready())
		return 0;

	mem_zero((uint8_t *)lfn_buf, FAT32_LFN_MAX_LEN);
	lfn_valid = 0;
	cluster = start_cluster;
	hops = 0;
	max_clusters = fat32_max_clusters();

	while (cluster)
	{
		if (max_clusters == 0 || hops++ >= max_clusters)
			return 0;
		if (fat32_read_cluster(cluster, g_cluster_buf) != 0)
			return 0;

		entries = cluster_bytes() / FAT32_DIR_ENTRY_SIZE;
		for (i = 0; i < entries; i++)
		{
			entry = &g_cluster_buf[i * FAT32_DIR_ENTRY_SIZE];

			if (entry[DIR_NAME_OFF] == 0x00)
				return 0;
			if (entry[DIR_NAME_OFF] == 0xE5)
			{
				lfn_valid = 0;
				continue;
			}

			attr = entry[DIR_ATTR_OFF];

			if (attr == FAT32_ATTR_LFN)
			{
				seq = entry[0];
				if (seq & FAT32_LFN_SEQ_LAST)
				{
					mem_zero((uint8_t *)lfn_buf, FAT32_LFN_MAX_LEN);
					lfn_valid = 1;
				}
				if (lfn_valid)
				{
					uint8_t seq_num = seq & 0x3FU;
					if (seq_num >= 1U && seq_num <= FAT32_LFN_MAX_ENTRIES)
						lfn_fill(entry, lfn_buf, seq_num);
				}
				continue;
			}

			if (attr & 0x08U)
			{
				lfn_valid = 0;
				continue;
			}

			{
				int matched = 0;
				if (lfn_valid && lfn_buf[0] != '\0')
					matched = lfn_match(lfn_buf, name);
				if (!matched)
					matched = fat32_name_match(entry + DIR_NAME_OFF, name);

				if (matched)
				{
					out->first_cluster =
						((uint32_t)mem_read16(entry + DIR_FSTCLUSHI_OFF) << 16)
						| mem_read16(entry + DIR_FSTCLUSLO_OFF);
					if (out->first_cluster == 0 && (attr & FAT32_ATTR_DIRECTORY))
						out->first_cluster = fat32.root_cluster;
					if (!(attr & FAT32_ATTR_DIRECTORY))
					{
						if (out->first_cluster < 2 || out->first_cluster >= fat32_max_clusters())
						{
							out->first_cluster = 0;
							return 0;
						}
					}
					out->size        = mem_read32(entry + DIR_FILESIZE_OFF);
					out->pos         = 0;
					out->dir_cluster = cluster;
					out->dir_index   = i;
					out->is_dir      = (attr & FAT32_ATTR_DIRECTORY) ? 1U : 0U;
					return 1;
				}
			}
			lfn_valid = 0;
		}
		if (fat32_get_next_cluster(cluster, &cluster) != 0)
			return 0;
	}
	return 0;
}

static int	fat32_update_dir_entry(FAT32_File *file)
{
	uint32_t	entries;
	uint8_t		*entry;

	if (!file || file->dir_cluster < 2 || !fat32_ready())
		return -1;
	if (fat32_read_cluster(file->dir_cluster, g_cluster_buf) != 0)
		return -1;
	entries = cluster_bytes() / FAT32_DIR_ENTRY_SIZE;
	if (file->dir_index >= entries)
		return -1;
	entry = &g_cluster_buf[file->dir_index * FAT32_DIR_ENTRY_SIZE];
	if (entry[DIR_NAME_OFF] == 0x00 || entry[DIR_NAME_OFF] == 0xE5)
		return -1;
	mem_write16(entry + DIR_FSTCLUSHI_OFF, (uint16_t)(file->first_cluster >> 16));
	mem_write16(entry + DIR_FSTCLUSLO_OFF, (uint16_t)(file->first_cluster & 0xFFFF));
	mem_write32(entry + DIR_FILESIZE_OFF, file->size);
	return fat32_write_cluster(file->dir_cluster, g_cluster_buf);
}

int	fat32_read_root_file(const char *filename, uint8_t *buffer)
{
	FAT32_File	file;

	file = fat32_open(filename);
	if (file.first_cluster == 0)
		return -1;
	return fat32_read(&file, buffer, file.size);
}

FAT32_File	fat32_open(const char *path)
{
	FAT32_File	file;
	FAT32_File	found;
	uint32_t	cur_cluster;
	char		comp[256];
	uint32_t	comp_len;

	file = (FAT32_File){0};
	if (!path || !fat32_ready())
		return file;
	if (*path == '/')
		path++;

	cur_cluster = fat32.root_cluster;

	while (1)
	{
		comp_len = 0;
		while (path[comp_len] && path[comp_len] != '/' && comp_len < 255U)
		{
			comp[comp_len] = path[comp_len];
			comp_len++;
		}
		comp[comp_len] = '\0';

		if (comp_len == 0)
			return file;

		found = (FAT32_File){0};
		if (!fat32_search_dir(cur_cluster, comp, &found))
			return file;
		if (path[comp_len] == '\0')
			return found;
		if (!found.is_dir)
			return file;

		cur_cluster = found.first_cluster;
		path += comp_len + 1U;
	}
}

FAT32_File fat32_create(const char *path)
{
	FAT32_File	file;
	FAT32_File	found;
	uint32_t	cur_cluster;
	uint32_t	new_cluster;
	uint8_t		name83[11];
	char		comp[256];
	uint32_t	comp_len;
	const char	*remaining;

	file = (FAT32_File){0};
	if (!path || !fat32_ready())
		return file;
	if (*path == '/')
		path++;

	cur_cluster = fat32.root_cluster;
	remaining = path;

	while (1)
	{
		comp_len = 0;
		while (remaining[comp_len] && remaining[comp_len] != '/' && comp_len < 255U)
		{
			comp[comp_len] = remaining[comp_len];
			comp_len++;
		}
		comp[comp_len] = '\0';

		if (comp_len == 0)
			return file;

		if (remaining[comp_len] == '\0')
			break;

		found = (FAT32_File){0};
		if (!fat32_search_dir(cur_cluster, comp, &found) || !found.is_dir)
			return file;
		cur_cluster = found.first_cluster;
		remaining += comp_len + 1U;
	}

	if (fat32_name_to_83(comp, name83) < 0)
		return file;

	if (fat32_search_dir(cur_cluster, comp, &found))
		return file;

	if (fat32_find_free_cluster(&new_cluster) != 0)
		return file;
	if (new_cluster < 2 || new_cluster >= fat32_max_clusters())
		return file;

	mem_zero(g_cluster_buf, cluster_bytes());
	if (fat32_write_cluster(new_cluster, g_cluster_buf) != 0)
		return file;
	if (fat32_set_fat_entry(new_cluster, FAT32_EOC) != 0)
		return file;
	if (fat32_create_dir_entry(cur_cluster, name83, new_cluster) != 0)
		return file;

	found = fat32_open(path);
	if (found.first_cluster == new_cluster)
		return found;
	file.first_cluster = new_cluster;
	return file;
}


int	fat32_read(FAT32_File *file, uint8_t *buffer, uint32_t len)
{
	uint32_t	remaining;
	uint32_t	read_total;
	uint32_t	cluster;
	uint32_t	offset;
	uint32_t	cbytes;
	uint32_t	copy_len;
	uint32_t	hops;
	uint32_t	max_clusters;

	if (!file || !buffer || !fat32_ready())
		return -1;
	if (file->first_cluster < 2)
		return -1;
	if (file->pos >= file->size)
		return 0;

	remaining = file->size - file->pos;
	if (len < remaining)
		remaining = len;

	cluster = file->first_cluster;
	offset = file->pos;
	cbytes = cluster_bytes();
	hops = 0;
	max_clusters = fat32_max_clusters();

	while (offset >= cbytes)
	{
		if (max_clusters == 0 || hops++ >= max_clusters)
			return -1;
		if (fat32_get_next_cluster(cluster, &cluster) != 0)
			return -1;
		if (cluster == 0)
			return -1;
		offset -= cbytes;
	}

	read_total = 0;
	while (remaining > 0 && cluster)
	{
		if (max_clusters == 0 || hops++ >= max_clusters)
			return -1;
		if (fat32_read_cluster(cluster, g_cluster_buf) != 0)
			return -1;

		copy_len = cbytes - offset;
		if (copy_len > remaining)
			copy_len = remaining;

		mem_copy(buffer + read_total, g_cluster_buf + offset, copy_len);

		read_total += copy_len;
		remaining -= copy_len;
		offset = 0;

		if (remaining > 0)
		{
			if (fat32_get_next_cluster(cluster, &cluster) != 0)
				return -1;
		}
	}

	file->pos += read_total;
	return (int)read_total;
}

int	fat32_write(FAT32_File *file, const uint8_t *buffer, uint32_t len)
{
	uint32_t	remaining;
	uint32_t	write_total;
	uint32_t	cluster;
	uint32_t	offset;
	uint32_t	cbytes;
	uint32_t	copy_len;
	uint32_t	i;
	uint32_t	lba;
	uint32_t	hops;
	uint32_t	max_clusters;

	if (!file || !buffer || !fat32_ready())
		return -1;
	if (file->first_cluster < 2)
		return -1;
	if (len == 0)
		return 0;

	remaining = len;
	cluster = file->first_cluster;
	offset = file->pos;
	cbytes = cluster_bytes();
	hops = 0;
	max_clusters = fat32_max_clusters();

	while (offset >= cbytes)
	{
		if (max_clusters == 0 || hops++ >= max_clusters)
			return -1;
		if (fat32_get_next_cluster(cluster, &cluster) != 0)
			return -1;
		if (cluster == 0)
			return 0;
		offset -= cbytes;
	}

	write_total = 0;
	while (remaining > 0 && cluster)
	{
		if (max_clusters == 0 || hops++ >= max_clusters)
			return -1;
		if (fat32_read_cluster(cluster, g_cluster_buf) != 0)
			return -1;

		copy_len = cbytes - offset;
		if (copy_len > remaining)
			copy_len = remaining;

		mem_copy(g_cluster_buf + offset, buffer + write_total, copy_len);

		lba = fat32_cluster_to_lba(cluster);
		if (lba == 0)
			return -1;
		for (i = 0; i < fat32.sectors_per_cluster; i++)
		{
			if (block_write(lba + i, g_cluster_buf + i * SECTOR_SIZE) != 0)
				return -1;
		}

		write_total += copy_len;
		remaining -= copy_len;
		offset = 0;

		if (remaining > 0)
		{
			if (fat32_get_next_cluster(cluster, &cluster) != 0)
				return -1;
		}
	}
	file->pos += write_total;
	if (file->pos > file->size)
	{
		file->size = file->pos;
		if (fat32_update_dir_entry(file) != 0)
			return -1;
	}

	return (int)write_total;
}

void	fat32_close(FAT32_File *file)
{
	if (!file)
		return;
	if (file->first_cluster >= 2 && file->dir_cluster >= 2)
		fat32_update_dir_entry(file);
	file->first_cluster = 0;
	file->size = 0;
	file->pos = 0;
	file->dir_cluster = 0;
	file->dir_index = 0;
}

void	fat32_list_dir(const char *path, void (*print)(const char *name, uint32_t size))
{
	uint32_t	cluster;
	uint32_t	entries;
	uint32_t	i;
	uint8_t		*entry;
	uint32_t	hops;
	uint32_t	max_clusters;
	char		name[13];
	char		lfn_buf[FAT32_LFN_MAX_LEN];
	int			lfn_valid;
	uint32_t	j;
	FAT32_File	dir;

	if (!print || !fat32_ready())
		return;

	if (!path || path[0] == '\0' || (path[0] == '/' && path[1] == '\0'))
	{
		cluster = fat32.root_cluster;
	}
	else
	{
		dir = fat32_open(path);
		if (!dir.is_dir || dir.first_cluster < 2)
			return;
		cluster = dir.first_cluster;
	}
	hops = 0;
	max_clusters = fat32_max_clusters();
	mem_zero((uint8_t *)lfn_buf, FAT32_LFN_MAX_LEN);
	lfn_valid = 0;

	while (cluster)
	{
		if (max_clusters == 0 || hops++ >= max_clusters)
			return;
		if (fat32_read_cluster(cluster, g_cluster_buf) != 0)
			return;

		entries = cluster_bytes() / FAT32_DIR_ENTRY_SIZE;
		for (i = 0; i < entries; i++)
		{
			entry = &g_cluster_buf[i * FAT32_DIR_ENTRY_SIZE];

			if (entry[DIR_NAME_OFF] == 0x00)
				return;
			if (entry[DIR_NAME_OFF] == 0xE5)
			{
				lfn_valid = 0;
				continue;
			}

			if (entry[DIR_ATTR_OFF] == FAT32_ATTR_LFN)
			{
				uint8_t seq = entry[0];
				if (seq & FAT32_LFN_SEQ_LAST)
				{
					mem_zero((uint8_t *)lfn_buf, FAT32_LFN_MAX_LEN);
					lfn_valid = 1;
				}
				if (lfn_valid)
				{
					uint8_t seq_num = seq & 0x3FU;
					if (seq_num >= 1U && seq_num <= FAT32_LFN_MAX_ENTRIES)
						lfn_fill(entry, lfn_buf, seq_num);
				}
				continue;
			}
			if (entry[DIR_ATTR_OFF] & 0x08U)
			{
				lfn_valid = 0;
				continue;
			}

			if (lfn_valid && lfn_buf[0] != '\0')
			{
				print(lfn_buf, mem_read32(entry + DIR_FILESIZE_OFF));
			}
			else
			{
				j = 0;
				while (j < 8 && entry[j] != ' ')
				{
					name[j] = entry[j];
					j++;
				}
				if (entry[8] != ' ')
				{
					name[j++] = '.';
					name[j++] = entry[8];
					if (entry[9]  != ' ') name[j++] = entry[9];
					if (entry[10] != ' ') name[j++] = entry[10];
				}
				name[j] = '\0';
				print(name, mem_read32(entry + DIR_FILESIZE_OFF));
			}
			lfn_valid = 0;
		}

		if (fat32_get_next_cluster(cluster, &cluster) != 0)
			return;
	}
}

int	fat32_mount(void)
{
    uint8_t mbr[SECTOR_SIZE];
    uint32_t part_lba;

    if (block_read(0, mbr) != 0)
        return -1;

    if (mbr[510] == 0x55 && mbr[511] == 0xAA)
    {
        part_lba = mem_read32(&mbr[446 + 8]);

        if (part_lba != 0)
        {
            if (parse_bpb(part_lba) == 0)
                return 0;
        }
    }
    return parse_bpb(0);
}
