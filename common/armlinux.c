/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "armboot.h"
#include "command.h"
#include "cmd_boot.h"
#include "image.h"
#include "malloc.h"
#include "zlib.h"

#include <asm-arm/setup.h>
#define tag_size(type)  ((sizeof(struct tag_header) + sizeof(struct type)) >> 2)
#define tag_next(t)     ((struct tag *)((u32 *)(t) + (t)->hdr.size))

static void setup_start_tag(bd_t *bd);
static void setup_memory_tags(bd_t *bd);
static void setup_commandline_tag(bd_t *bd, char *commandline);
#if 0
static void setup_ramdisk_tag(bd_t *bd);
#endif
static void setup_initrd_tag(bd_t *bd, ulong initrd_start, ulong initrd_end);
static void setup_end_tag(bd_t *bd);

extern image_header_t header;           /* from cmd_bootm.c */

#undef DEBUG

static struct tag *params;

void boot_linux(cmd_tbl_t *cmdtp,
		bd_t *bd, int flag,
		int argc, char *argv[],
		ulong addr,
		ulong *len_ptr,
		int   verify)
{
    ulong len = 0, checksum;
    ulong initrd_start, initrd_end;
    ulong data;
    char *commandline = getenv(bd, "bootargs");
    void (*theKernel)(int zero, int arch);
    image_header_t *hdr = &header;

    /*
     * Check if there is an initrd image
     */
    if (argc >= 3) {
	addr = simple_strtoul(argv[2], NULL, 16);
	
	printf ("## Loading Ramdisk Image at %08lx ...\n", addr);
	
	/* Copy header so we can blank CRC field for re-calculation */
	memcpy (&header, (char *)addr, sizeof(image_header_t));
	
	if (SWAP32(hdr->ih_magic) != IH_MAGIC) {
	    printf ("Bad Magic Number\n");
	    do_reset (cmdtp, bd, flag, argc, argv);
	}
	
	data = (ulong)&header;
	len  = sizeof(image_header_t);
	
	checksum = SWAP32(hdr->ih_hcrc);
	hdr->ih_hcrc = 0;
	
	if (crc32 (0, (char *)data, len) != checksum) {
	    printf ("Bad Header Checksum\n");
	    do_reset (cmdtp, bd, flag, argc, argv);
	}
	
	print_image_hdr (hdr);
	
	data = addr + sizeof(image_header_t);
	len  = SWAP32(hdr->ih_size);
	
	if (verify) {
	    ulong csum = 0;

	    printf ("   Verifying Checksum ... ");
	    csum = crc32 (0, (char *)data, len);
	    if (csum != SWAP32(hdr->ih_dcrc)) {
		printf ("Bad Data CRC\n");
		do_reset (cmdtp, bd, flag, argc, argv);
	    }
	    printf ("OK\n");
	}
	
	if ((hdr->ih_os   != IH_OS_LINUX)	||
	    (hdr->ih_arch != IH_CPU_ARM)	||
	    (hdr->ih_type != IH_TYPE_RAMDISK)	) {
	    printf ("No Linux ARM Ramdisk Image\n");
	    do_reset (cmdtp, bd, flag, argc, argv);
	}
	
	/*
	 * Now check if we have a multifile image
	 */
    } else if ((hdr->ih_type==IH_TYPE_MULTI) && (len_ptr[1])) {
	ulong tail    = SWAP32(len_ptr[0]) % 4;
	int i;
	
	/* skip kernel length and terminator */
	data = (ulong)(&len_ptr[2]);
	/* skip any additional image length fields */
	for (i=1; len_ptr[i]; ++i)
	  data += 4;
	/* add kernel length, and align */
	data += SWAP32(len_ptr[0]);
	if (tail) {
	    data += 4 - tail;
	}
	
	len   = SWAP32(len_ptr[1]);
	
    } else {
	/*
	 * no initrd image
	 */
	data = 0;
    }
    
#ifdef	DEBUG
    if (!data) {
	printf ("No initrd\n");
    }
#endif
    
    if (data) {
	initrd_start = data;
	initrd_end   = initrd_start + len;
	printf ("   Loading Ramdisk to %08lx, end %08lx ... ",
		initrd_start, initrd_end);
	memmove ((void *)initrd_start, (void *)data, len);
	printf ("OK\n");
    } else {
	initrd_start = 0;
	initrd_end = 0;
    }
    
    theKernel = (void (*)(int, int))SWAP32(hdr->ih_ep);
   
#ifdef DEBUG
    printf ("## Transferring control to Linux (at address %08lx) ...\n",
	    (ulong)theKernel);
#endif

    setup_start_tag(bd);
    setup_memory_tags(bd);
    setup_commandline_tag(bd, commandline);
    setup_initrd_tag(bd, initrd_start, initrd_end);
#if 0
    setup_ramdisk_tag(bd);
#endif
    setup_end_tag(bd);

    /* we assume that the kernel is in place */
    printf("\nStarting kernel ...\n\n");

    cleanup_before_linux(bd);

    theKernel(0, bd->bi_arch_number);
}


static void setup_start_tag(bd_t *bd)
{
    params = (struct tag *)bd->bi_boot_params;
    
    params->hdr.tag = ATAG_CORE;
    params->hdr.size = tag_size(tag_core);
    
    params->u.core.flags = 0;
    params->u.core.pagesize = 0;
    params->u.core.rootdev = 0;
    
    params = tag_next(params);
}


static void setup_memory_tags(bd_t *bd)
{
    int i;
    
    for(i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
	params->hdr.tag = ATAG_MEM;
	params->hdr.size = tag_size(tag_mem32);
	
	params->u.mem.start = bd->bi_dram[i].start;
	params->u.mem.size = bd->bi_dram[i].size;
	    
	params = tag_next(params);
    }
}


static void setup_commandline_tag(bd_t *bd, char *commandline)
{
    char *p;
    
    /* eat leading white space */
    for(p = commandline; *p == ' '; p++)
      ;
    
    /* skip non-existent command lines so the kernel will still
     * use its default command line.
     */
    if(*p == '\0')
      return;
    
    params->hdr.tag = ATAG_CMDLINE;
    params->hdr.size = (sizeof(struct tag_header) + strlen(p) + 1 + 4) >> 2;
    
    strcpy(params->u.cmdline.cmdline, p);
    
    params = tag_next(params);
}


static void setup_initrd_tag(bd_t *bd, ulong initrd_start, ulong initrd_end)
{
    /* an ATAG_INITRD node tells the kernel where the compressed
     * ramdisk can be found. ATAG_RDIMG is a better name, actually.
     */
    params->hdr.tag = ATAG_INITRD;
    params->hdr.size = tag_size(tag_initrd);
    
    params->u.initrd.start = initrd_start;
    params->u.initrd.size = initrd_end - initrd_start;
    
    params = tag_next(params);
}


#if 0
static void setup_ramdisk_tag(bd_t *bd)
{
    /* an ATAG_RAMDISK node tells the kernel how large the
     * decompressed ramdisk will become.
     */
    params->hdr.tag = ATAG_RAMDISK;
    params->hdr.size = tag_size(tag_ramdisk);
    
    params->u.ramdisk.start = 0;
    //params->u.ramdisk.size = RAMDISK_SIZE;
    params->u.ramdisk.flags = 1;	/* automatically load ramdisk */
    
    params = tag_next(params);
}
#endif

static void setup_end_tag(bd_t *bd)
{
    params->hdr.tag = ATAG_NONE;
    params->hdr.size = 0;
}
