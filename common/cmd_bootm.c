/*
 * (C) Copyright 2001
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * 
 * (C) Copyright 2000, 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Boot support
 */
#include "armboot.h"
#include "command.h"
#include "cmd_boot.h"
#include "image.h"
#include "malloc.h"
#include "zlib.h"

#undef DEBUG

int  gunzip (void *, int, unsigned char *, int *);

static void *zalloc(void *, unsigned, unsigned);
static void zfree(void *, void *, unsigned);

#if (CONFIG_COMMANDS & CFG_CMD_IMI)
static int image_info (unsigned long addr);
#endif
static void print_type (image_header_t *hdr);

ulong load_addr = CFG_LOAD_ADDR;		/* Default Load Address */
image_header_t header;

extern void boot_linux(cmd_tbl_t *cmdtp, 
		       bd_t *bd, int flag,
		       int argc, char *argv[],
		       ulong addr,
		       ulong *len_ptr,
		       int   verify);

int do_bootm (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	ulong	iflag;
	ulong	addr, ram_addr;
	ulong	data, len, checksum;
	ulong  *len_ptr;
	int	i, verify;
	char	*name, *s;
	int	(*appl)(cmd_tbl_t *, bd_t *, int, int, char *[]);
	image_header_t *hdr = &header;

	s = getenv (bd, "verify");
	verify = (s && (*s == 'n')) ? 0 : 1;

	if (argc < 2) {
		addr = load_addr;
	} else {
		addr = simple_strtoul(argv[1], NULL, 16);
	}

	printf ("## Booting image at %08lx ...\n", addr);

	/* Copy header so we can blank CRC field for re-calculation */
	memcpy (&header, (char *)addr, sizeof(image_header_t));

	if (SWAP32(hdr->ih_magic)  != IH_MAGIC) {
		printf ("Bad Magic Number\n");
		return 1;
	}

	data = (ulong)&header;
	len  = sizeof(image_header_t);

	checksum = SWAP32(hdr->ih_hcrc);
	hdr->ih_hcrc = 0;

	if (crc32 (0, (char *)data, len) != checksum) {
		printf ("Bad Header Checksum\n");
		return 1;
	}

	/************************************************************/
	/* BIG FAT WARNING:                                         */
	/* SOME PARTS OF THIS CODE WILL OVERWRITE THE IMAGE         */
	/* IF THE IMAGE RESIDES IN FLASH, FLASH WILL BE OVERWRITTEN */
	/* SO WE TRANSFER THE IMAGE TO RAM FIRST                    */
	/************************************************************/

	if (addr2info(addr) != NULL)
	{
		ram_addr = load_addr; /* should be a env-var ... */
		printf ("## Copy image from flash %08lx to ram %08lx ...\n", addr, ram_addr);

		/* we copy the whole to ram (load_addr) */
		memcpy ((char *)ram_addr, (char *)addr, SWAP32(hdr->ih_size) + sizeof(image_header_t));
		addr = ram_addr;
	}

	/* for multi-file images we need the data part, too */
	print_image_hdr ((image_header_t *)addr);

	data = addr + sizeof(image_header_t);
	len  = SWAP32(hdr->ih_size);

	if (verify) {
		printf ("   Verifying Checksum ... ");
		if (crc32 (0, (char *)data, len) != SWAP32(hdr->ih_dcrc)) {
			printf ("Bad Data CRC\n");
			return 1;
		}
		printf ("OK\n");
	}

	len_ptr = (ulong *)data;

	if (hdr->ih_arch != IH_CPU_ARM) {
		printf ("Unsupported Architecture\n");
		return 1;
	}

	switch (hdr->ih_type) {
	case IH_TYPE_STANDALONE:	name = "Standalone Application";
					break;
	case IH_TYPE_KERNEL:		name = "Kernel Image";
					break;
	case IH_TYPE_MULTI:		name = "Multi-File Image";
					len  = SWAP32(len_ptr[0]);
					/* OS kernel is always the first image */
					data += 8; /* kernel_len + terminator */
					for (i=1; len_ptr[i]; ++i)
						data += 4;
					break;
	default: printf ("Wrong Image Type for %s command\n", cmdtp->name);
		 return 1;
	}

	/*
	 * We have reached the point of no return: we are going to
	 * overwrite all exception vector code, so we cannot easily
	 * recover from any failures any more...
	 */

	iflag = disable_interrupts();

	switch (hdr->ih_comp) {
	case IH_COMP_NONE:
		printf ("   Loading %s ... ", name);
		memcpy ((void *) SWAP32(hdr->ih_load), (uchar *)data, len);
		break;
	case IH_COMP_GZIP:
		printf ("   Uncompressing %s ... ", name);
		if (gunzip ((void *)SWAP32(hdr->ih_load), 0x400000,
			    (uchar *)data, (int *)&len) != 0) {
			printf ("GUNZIP ERROR - must RESET board to recover\n");
			do_reset (cmdtp, bd, flag, argc, argv);
		}
		break;
	default:
		if (iflag)
			enable_interrupts();
		printf ("Unimplemented compression type %d\n", hdr->ih_comp);
		return 1;
	}
	printf ("OK\n");

	switch (hdr->ih_type) {
	case IH_TYPE_STANDALONE:
		appl = (int (*)(cmd_tbl_t *, bd_t *, int, int, char *[]))SWAP32(hdr->ih_ep);

		/* flush caches before invoking (potentially downloaded) code */
		flush_all_caches();
		(*appl)(cmdtp, bd, flag, argc-1, &argv[1]);
		/* just in case we return */
		if (iflag)
			enable_interrupts();
		break;
	case IH_TYPE_KERNEL:
	case IH_TYPE_MULTI:
		/* handled below */
		break;
	default:
		if (iflag)
			enable_interrupts();
		printf ("Can't boot image type %d\n", hdr->ih_type);
		return 1;
	}

	switch (hdr->ih_os)
	{
	default:			/* handled by (original) Linux case */
	case IH_OS_LINUX:
	    boot_linux(cmdtp, bd, flag, argc, argv,
			     addr, len_ptr, verify);
	    break;
	}

	printf ("\n## Control returned to monitor\n");
#ifdef DEBUG
	printf ("\nResetting Hardware ... \n");
	do_reset (cmdtp, bd, flag, argc, argv);
#endif
	return 0;
}


#if (CONFIG_COMMANDS & CFG_CMD_BOOTD)
int do_bootd (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	return run_command (getenv (bd, "bootcmd"), bd, flag);
}
#endif


#if (CONFIG_COMMANDS & CFG_CMD_IMI)
int do_iminfo (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	int	arg;
	ulong	addr;
	int rc = 0;

	if (argc < 2) {
		rc = image_info (load_addr);
	}
	else {
		for (arg = 1; arg < argc && rc == 0; ++arg) {
			addr = simple_strtoul(argv[arg], NULL, 16);
			rc = image_info (addr);
		}
	}
	return rc;
}

static int image_info (ulong addr)
{
	ulong	data, len, checksum;
	image_header_t *hdr = &header;

	printf ("\n## Checking Image at %08lx ...\n", addr);

	/* Copy header so we can blank CRC field for re-calculation */
	memcpy (&header, (char *)addr, sizeof(image_header_t));

	if (SWAP32(hdr->ih_magic)  != IH_MAGIC) {
		printf ("   Bad Magic Number\n");
		return 1;
	}

	data = (ulong)&header;
	len  = sizeof(image_header_t);

	checksum = SWAP32(hdr->ih_hcrc);
	hdr->ih_hcrc = 0;

	if (crc32 (0, (char *)data, len) != checksum) {
		printf ("   Bad Header Checksum\n");
		return 1;
	}

	/* for multi-file images we need the data part, too */
	print_image_hdr ((image_header_t *)addr);

	data = addr + sizeof(image_header_t);
	len  = SWAP32(hdr->ih_size);

	printf ("   Verifying Checksum ... ");
	if (crc32 (0, (char *)data, len) != SWAP32(hdr->ih_dcrc)) {
		printf ("   Bad Data CRC\n");
		return 1;
	}
	printf ("OK\n");
	return 0;
}
#endif	/* CFG_CMD_IMI */

void
print_image_hdr (image_header_t *hdr)
{
	printf ("   Image Name:   %.*s\n", IH_NMLEN, hdr->ih_name);
	printf ("   Image Type:   "); print_type(hdr); printf ("\n");
	printf ("   Data Size:    %d Bytes = %d kB = %d MB\n",
		SWAP32(hdr->ih_size), 
		SWAP32(hdr->ih_size) >> 10, 
		SWAP32(hdr->ih_size) >> 20);
	printf ("   Load Address: %08x\n", SWAP32(hdr->ih_load));
	printf ("   Entry Point:  %08x\n", SWAP32(hdr->ih_ep));

	if (hdr->ih_type == IH_TYPE_MULTI) {
		int i;
	    	ulong len;
		ulong *len_ptr = (ulong *)((ulong)hdr + sizeof(image_header_t));

		printf ("   Contents:\n");
		for (i=0; (len = SWAP32(*len_ptr)); ++i, ++len_ptr) {
			printf ("   Image %d: %8ld Bytes = %ld kB = %ld MB\n",
				i, len, len >> 10, len >> 20);
		}
	}
}


static void
print_type (image_header_t *hdr)
{
	char *os, *arch, *type, *comp;

	switch (hdr->ih_os) {
	case IH_OS_INVALID:	os = "Invalid OS";		break;
	case IH_OS_NETBSD:	os = "NetBSD";			break;
	case IH_OS_LINUX:	os = "Linux";			break;
	case IH_OS_PPCBOOT:	os = "ARMBoot";			break;
	default:		os = "Unknown OS";		break;
	}

	switch (hdr->ih_arch) {
	case IH_CPU_INVALID:	arch = "Invalid CPU";		break;
	case IH_CPU_ALPHA:	arch = "Alpha";			break;
	case IH_CPU_ARM:	arch = "ARM";			break;
	case IH_CPU_I386:	arch = "Intel x86";		break;
	case IH_CPU_IA64:	arch = "IA64";			break;
	case IH_CPU_MIPS:	arch = "MIPS";			break;
	case IH_CPU_MIPS64:	arch = "MIPS 64 Bit";		break;
	case IH_CPU_PPC:	arch = "PowerPC";		break;
	case IH_CPU_S390:	arch = "IBM S390";		break;
	case IH_CPU_SH:		arch = "SuperH";		break;
	case IH_CPU_SPARC:	arch = "SPARC";			break;
	case IH_CPU_SPARC64:	arch = "SPARC 64 Bit";		break;
	default:		arch = "Unknown Architecture";	break;
	}

	switch (hdr->ih_type) {
	case IH_TYPE_INVALID:	type = "Invalid Image";		break;
	case IH_TYPE_STANDALONE:type = "Standalone Program";	break;
	case IH_TYPE_KERNEL:	type = "Kernel Image";		break;
	case IH_TYPE_RAMDISK:	type = "RAMDisk Image";		break;
	case IH_TYPE_MULTI:	type = "Multi-File Image";	break;
	case IH_TYPE_FIRMWARE:	type = "Firmware";			break;
	default:		type = "Unknown Image";		break;
	}

	switch (hdr->ih_comp) {
	case IH_COMP_NONE:	comp = "uncompressed";		break;
	case IH_COMP_GZIP:	comp = "gzip compressed";	break;
	case IH_COMP_BZIP2:	comp = "bzip2 compressed";	break;
	default:		comp = "unknown compression";	break;
	}

	printf ("%s %s %s (%s)", arch, os, type, comp);
}

#define	ZALLOC_ALIGNMENT	16

static void *zalloc(void *x, unsigned items, unsigned size)
{
	void *p;

	size *= items;
	size = (size + ZALLOC_ALIGNMENT - 1) & ~(ZALLOC_ALIGNMENT - 1);

	p = malloc (size);

	return (p);
}

static void zfree(void *x, void *addr, unsigned nb)
{
	free (addr);
}

#define HEAD_CRC	2
#define EXTRA_FIELD	4
#define ORIG_NAME	8
#define COMMENT		0x10
#define RESERVED	0xe0

#define DEFLATED	8

int gunzip(void *dst, int dstlen, unsigned char *src, int *lenp)
{
	z_stream s;
	int r, i, flags;

	/* skip header */
	i = 10;
	flags = src[3];
	if (src[2] != DEFLATED || (flags & RESERVED) != 0) {
		printf ("Error: Bad gzipped data\n");
		return (-1);
	}
	if ((flags & EXTRA_FIELD) != 0)
		i = 12 + src[10] + (src[11] << 8);
	if ((flags & ORIG_NAME) != 0)
		while (src[i++] != 0)
			;
	if ((flags & COMMENT) != 0)
		while (src[i++] != 0)
			;
	if ((flags & HEAD_CRC) != 0)
		i += 2;
	if (i >= *lenp) {
		printf ("Error: gunzip out of data in header\n");
		return (-1);
	}

	s.zalloc = zalloc;
	s.zfree = zfree;
#if defined(CONFIG_WATCHDOG)
	s.outcb = (cb_func)watchdog_reset;
#else
	s.outcb = Z_NULL;
#endif	/* CONFIG_WATCHDOG */

	r = inflateInit2(&s, -MAX_WBITS);
	if (r != Z_OK) {
		printf ("Error: inflateInit2() returned %d\n", r);
		return (-1);
	}
	s.next_in = src + i;
	s.avail_in = *lenp - i;
	s.next_out = dst;
	s.avail_out = dstlen;
	r = inflate(&s, Z_FINISH);
	if (r != Z_OK && r != Z_STREAM_END) {
		printf ("Error: inflate() returned %d\n", r);
		return (-1);
	}
	*lenp = s.next_out - (unsigned char *) dst;
	inflateEnd(&s);

	return (0);
}
