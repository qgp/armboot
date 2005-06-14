/*
 * Copyright (c) 2000-2002 Altera Corporation, San Jose, California, USA.  
 * All rights reserved.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
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

#include "armboot.h"
#include "config.h"

ulong myflush(void);

typedef volatile unsigned short fltype;

#define FLASH_BANK_SIZE 0x400000
#define MAIN_SECT_SIZE  0x10000
#define PARAM_SECT_SIZE 0x2000

#define PUZZLE_FROM_FLASH(x)	(x)
#define PUZZLE_TO_FLASH(x)	(x)

extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];

#define CMD_READ_ARRAY		0x00FF
#define CMD_IDENTIFY	        0x0090
#define CMD_ERASE_SETUP		0x0020
#define CMD_ERASE_CONFIRM	0x00D0
#define CMD_PROGRAM		0x0040
#define CMD_RESUME		0x00D0
#define CMD_SUSPEND		0x00B0
#define CMD_STATUS_READ		0x0070
#define CMD_STATUS_RESET	0x0050
#define CMD_CONFIG_SETUP        0x0060
#define CMD_LOCK_BLOCK          0x0001
#define CMD_UNLOCK_BLOCK        0x00D0

#define BIT_BUSY		0x0080
#define BIT_ERASE_SUSPEND	0x0040
#define BIT_ERASE_ERROR		0x0020
#define BIT_PROGRAM_ERROR	0x0010
#define BIT_VPP_RANGE_ERROR	0x0008
#define BIT_PROGRAM_SUSPEND	0x0004
#define BIT_PROTECT_ERROR	0x0002
#define BIT_UNDEFINED		0x0001

#define BIT_SEQUENCE_ERROR	0x0030
#define BIT_TIMEOUT		0x8000

#define MASK_VENDOR_ID          0xFF00FF00
#define MASK_FLASHTYPE          0x00FF00FF

/*-----------------------------------------------------------------------
 */

ulong int_flash_init(bd_t *bd)
{
    int i, j;
    ulong size = 0;

    for (i = 0; i < CFG_MAX_FLASH_BANKS; i++)
    {
	ulong flashbase = 0;

	flash_info[i].flash_id =
	  (INTEL_MANUFACT & FLASH_VENDMASK) |
	  (INTEL_ID_28F320C3B & FLASH_TYPEMASK);
	flash_info[i].size = FLASH_BANK_SIZE;
	flash_info[i].sector_count = CFG_MAX_FLASH_SECT;
	memset(flash_info[i].protect, 0, CFG_MAX_FLASH_SECT);
	if (i == 0)
	  flashbase = PHYS_FLASH_1;
// only one FLASH on DCS board
//	else if (i == 1)
//	  flashbase = PHYS_FLASH_2;

//	else
//	  panic("configured to0 many flash banks!\n");



	for (j = 0; j < flash_info[i].sector_count; j++)
	{
	    if (j <= 7)
	    {
		flash_info[i].start[j] = flashbase + j * PARAM_SECT_SIZE;
	    }
	    else
	    {
		flash_info[i].start[j] = flashbase + (j - 7)*MAIN_SECT_SIZE;
	    }
	}
	size += flash_info[i].size;
    }

    /* Protect monitor and environment sectors
     */
    flash_protect(FLAG_PROTECT_SET,
		  CFG_FLASH_BASE,
		  CFG_FLASH_BASE + _armboot_end - _armboot_start,
		  &flash_info[0]);

    flash_protect(FLAG_PROTECT_SET,
		  CFG_ENV_ADDR,
		  CFG_ENV_ADDR + CFG_ENV_SIZE - 1,
		  &flash_info[0]);

    return size;
}

/*-----------------------------------------------------------------------
 */
void int_flash_print_info  (flash_info_t *info)
{
	int i;

	switch (info->flash_id & FLASH_VENDMASK)
	{
	case (INTEL_MANUFACT & FLASH_VENDMASK):
		printf("Intel: ");
		break;
	case (AMD_MANUFACT & FLASH_VENDMASK):
		printf("AMD: ");
		break;
	default:
		printf("Unknown Vendor ");
		break;
	}

	switch (info->flash_id & FLASH_TYPEMASK)
	{
	case (INTEL_ID_28F160F3B & FLASH_TYPEMASK):
		printf("2x 28F160F3B (16Mbit)\n");
		break;
	case (INTEL_ID_28F320C3B & FLASH_TYPEMASK):
		printf("1x 28F320C3B (32Mbit)\n");
		break;
	case(AMD_ID_DL322B):
		printf("1x 29DL322B ID (32 M, bottom boot sect)\n");
		break;
	default:
		printf("Unknown Chip Type\n");
		printf("flash id %#x\n",(int) info->flash_id);
		goto Done;
		break;
	}

	printf("  Size: %ld MB in %d Sectors\n",
	       info->size >> 20, info->sector_count);

	printf("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; i++)
	{
		if ((i % 5) == 0)
		{
			printf ("\n   ");
		}
		printf (" %08lX%s", info->start[i],
			info->protect[i] ? " (RO)" : "     ");
	}
	printf ("\n");

 Done:
}

/*-----------------------------------------------------------------------
 */

static int flash_error (ulong code)
{
	/* Check bit patterns */
	/* SR.7=0 is busy, SR.7=1 is ready */
	/* all other flags indicate error on 1 */
	/* SR.0 is undefined */
	/* Timeout is our faked flag */

	/* sequence is described in Intel 290644-005 document */

	/* check Timeout */
	if (code & BIT_TIMEOUT)
	{
		printf ("Timeout\n");
		return ERR_TIMOUT;
	}

	/* check Busy, SR.7 */
	if (~code & BIT_BUSY)
	{
		printf ("Busy\n");
		return ERR_PROG_ERROR;
	}

	/* check Vpp low, SR.3 */
	if (code & BIT_VPP_RANGE_ERROR)
	{
		printf ("Vpp range error\n");
		return ERR_PROG_ERROR;
	}

	/* check Device Protect Error, SR.1 */
	if (code & BIT_PROTECT_ERROR)
	{
		printf ("Device protect error\n");
		return ERR_PROG_ERROR;
	}

	/* check Command Seq Error, SR.4 & SR.5 */
	if (code & BIT_SEQUENCE_ERROR)
	{
		printf ("Command seqence error\n");
		return ERR_PROG_ERROR;
	}

	/* check Block Erase Error, SR.5 */
	if (code & BIT_ERASE_ERROR)
	{
		printf ("Block erase error\n");
		return ERR_PROG_ERROR;
	}

	/* check Program Error, SR.4 */
	if (code & BIT_PROGRAM_ERROR)
	{
		printf ("Program error\n");
		return ERR_PROG_ERROR;
	}

	/* check Block Erase Suspended, SR.6 */
	if (code & BIT_ERASE_SUSPEND)
	{
		printf ("Block erase suspended\n");
		return ERR_PROG_ERROR;
	}

	/* check Program Suspended, SR.2 */
	if (code & BIT_PROGRAM_SUSPEND)
	{
		printf ("Program suspended\n");
		return ERR_PROG_ERROR;
	}

	/* OK, no error */
	return ERR_OK;
}

/*-----------------------------------------------------------------------
 */

int	int_flash_erase (flash_info_t *info, int s_first, int s_last)
{
    ulong result;
    int iflag, cflag, prot, sect;
    int rc = ERR_OK;

    /* first look for protection bits */

    if (info->flash_id == FLASH_UNKNOWN)
	return ERR_UNKNOWN_FLASH_TYPE;

    if ((s_first < 0) || (s_first > s_last)) {
	return ERR_INVAL;
    }

    if ((info->flash_id & FLASH_VENDMASK) !=
	(INTEL_MANUFACT & FLASH_VENDMASK)) {
	return ERR_UNKNOWN_FLASH_VENDOR;
    }

    prot = 0;
    for (sect=s_first; sect<=s_last; ++sect) {
	if (info->protect[sect]) {
	    prot++;
	}
    }
    if (prot)
	return ERR_PROTECTED;

    /*
     * Disable interrupts which might cause a timeout
     * here. Remember that our exception vectors are
     * at address 0 in the flash, and we don't want a
     * (ticker) exception to happen while the flash
     * chip is in programming mode.
     */
    cflag = icache_status();
    icache_disable();
    iflag = disable_interrupts();

    /* Start erase on unprotected sectors */
    for (sect = s_first; sect<=s_last && !ctrlc(); sect++)
    {
	printf("Erasing sector %2d ... ", sect);

	/* arm simple, non interrupt dependent timer */
	reset_timer_masked();

	if (info->protect[sect] == 0)
	{	/* not protected */
	    fltype *addr = (fltype *)(info->start[sect]);

	    *addr = PUZZLE_TO_FLASH(CMD_CONFIG_SETUP);
	    *addr = PUZZLE_TO_FLASH(CMD_UNLOCK_BLOCK);
	    *addr = PUZZLE_TO_FLASH(CMD_STATUS_RESET);
	    *addr = PUZZLE_TO_FLASH(CMD_ERASE_SETUP);
	    *addr = PUZZLE_TO_FLASH(CMD_ERASE_CONFIRM);

	    /* wait until flash is ready */
	    do
	    {
		/* check timeout */
		if (get_timer_masked() > CFG_FLASH_ERASE_TOUT)
		{
		    *addr = PUZZLE_TO_FLASH(CMD_SUSPEND);
		    result = BIT_TIMEOUT;
		    break;
		}

		result = PUZZLE_FROM_FLASH(*addr);
	    }  while (~result & BIT_BUSY);

            *addr = PUZZLE_TO_FLASH(CMD_CONFIG_SETUP);
            *addr = PUZZLE_TO_FLASH(CMD_UNLOCK_BLOCK);
            *addr = PUZZLE_TO_FLASH(CMD_READ_ARRAY);    

	    if ((rc = flash_error(result)) != ERR_OK)
	    	goto outahere;

	    printf("ok.\n");
	}
	else /* it was protected */
	{
	    printf("protected!\n");
	}
    }

    if (ctrlc())
      printf("User Interrupt!\n");

outahere:

    /* allow flash to settle - wait 10 ms */
    udelay_masked(10000);

    if (iflag)
      enable_interrupts();

    if (cflag)
      icache_enable();

    return rc;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash
 */

volatile static int write_word (flash_info_t *info, ulong dest, ulong data)
{
    fltype *addr = (fltype *)dest;
    ulong result;
    int rc = ERR_OK;
    int cflag, iflag;

    /* Check if Flash is (sufficiently) erased
     */
    result = PUZZLE_FROM_FLASH(*addr);
    if ((result & data) != data)
        return ERR_NOT_ERASED;

    /*
     * Disable interrupts which might cause a timeout
     * here. Remember that our exception vectors are
     * at address 0 in the flash, and we don't want a
     * (ticker) exception to happen while the flash
     * chip is in programming mode.
     */
    cflag = icache_status();
    icache_disable();
    iflag = disable_interrupts();

    *addr = PUZZLE_TO_FLASH(CMD_STATUS_RESET);
    *addr = PUZZLE_TO_FLASH(CMD_PROGRAM);
    *addr = data;

    /* arm simple, non interrupt dependent timer */
    reset_timer_masked();

    /* wait until flash is ready */
    do
    {
	/* check timeout */
	if (get_timer_masked() > CFG_FLASH_ERASE_TOUT)
	{
	    *addr = PUZZLE_TO_FLASH(CMD_SUSPEND);
	    result = BIT_TIMEOUT;
	    break;
	}

	result = PUZZLE_FROM_FLASH(*addr);
    }  while (~result & BIT_BUSY);

    *addr = PUZZLE_TO_FLASH(CMD_READ_ARRAY);

    rc = flash_error(result);

    if (iflag)
      enable_interrupts();

    if (cflag)
      icache_enable();

    return rc;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash.
 */

int int_write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
    ulong wp, data, ep;
    int l;
    int rc;
    int sec = 0;
    int lsec;    

    wp = (addr & ~1);	/* get lower half word aligned address */  
    ep = addr + cnt;
    
    /* unlock all sectors being accessed */

    while ((info->start[sec] < addr) && (++sec < info->sector_count));  
    sec--;

    lsec = sec;

    while ((sec < info->sector_count) && (info->start[sec] < ep)){
        *((uchar *) info->start[sec]) = PUZZLE_TO_FLASH(CMD_CONFIG_SETUP);
        *((uchar *) info->start[sec]) = PUZZLE_TO_FLASH(CMD_UNLOCK_BLOCK);
        *((uchar *) info->start[sec++]) = PUZZLE_TO_FLASH(CMD_READ_ARRAY);    
    }    
    
    /*
     * handle unaligned start bytes
     */

    if ((l = addr - wp) != 0) {
	data = *(uchar *)wp | (*src++ << 8);
	--cnt;

	if ((rc = write_word(info, wp, data)) != 0) {
	    goto finish;
	}
	wp += 2;
    }

    /*
     * handle word aligned part
     */
    while (cnt >= 2) {
	data = *((fltype*)src);
	if ((rc = write_word(info, wp, data)) != 0) {
	    goto finish;
	}
	src += 2;
	wp  += 2;
	cnt -= 2;
    }

    if (cnt == 0) {
	rc =  ERR_OK;
        goto finish;
    }

    /*
     * handle unaligned tail bytes
     */

    data = ((*(uchar *)(wp + 1)) << 8) | *src;

    rc = write_word(info, wp, data);

    /* lock all sectors being accessed */

finish:

    while ((lsec < info->sector_count) && (info->start[lsec] < ep)){
        *((uchar *) info->start[lsec]) = PUZZLE_TO_FLASH(CMD_CONFIG_SETUP);
        *((uchar *) info->start[lsec]) = PUZZLE_TO_FLASH(CMD_UNLOCK_BLOCK);
        *((uchar *) info->start[lsec++]) = PUZZLE_TO_FLASH(CMD_READ_ARRAY);    
    }
    return rc;    
}

/*-----------------------------------------------------------------------
 * Read user data.
 */

int int_flash_read_user(int bank, int offset){
	fltype* addr;
	int val;

	if (bank == 1){
	    addr = (fltype*) PHYS_FLASH_1;
	}
	else if (bank == 2){
	    addr = (fltype*) PHYS_FLASH_2;
	}
	else{
	    return -1;
	}
	*addr = PUZZLE_TO_FLASH(CMD_IDENTIFY);
	val   = *(addr + 0x85 + (offset >> 1));
	if (offset & 0x1){
		val >>= 8;
	}
	val &= 0xff;
	*addr = PUZZLE_TO_FLASH(CMD_READ_ARRAY);
	return val;
} 

