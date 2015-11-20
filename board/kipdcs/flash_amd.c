/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
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

ulong myflush(void);


#define FLASH_BANK_SIZE 0x800000	/* 8 MB */
#define MAIN_SECT_SIZE  0x10000
#define PARAM_SECT_SIZE 0x1000

extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];

//unsigned int    flashbase[CFG_MAX_FLASH_BANKS]={
//	PHYS_FLASH_1,
//	PHYS_FLASH_2
//};
unsigned int    flashbase[CFG_MAX_FLASH_BANKS]={
	PHYS_FLASH_1,
};

#define CMD_READ_ARRAY		0x000000F0
#define CMD_UNLOCK1		0x000000AA
#define CMD_UNLOCK2		0x00000055
#define CMD_ERASE_SETUP		0x00000080
#define CMD_ERASE_CONFIRM	0x00000030
#define CMD_PROGRAM		0x000000A0
#define CMD_UNLOCK_BYPASS	0x00000020
#define CMD_ENTER_SECSI	        0x00000088
#define CMD_EXIT_SECSI	        0x00000090


#define MEM_FLASH_ADDR1		(*(volatile u16 *)(CFG_FLASH_BASE + (0x00000555 << 1)))
#define MEM_FLASH_ADDR2		(*(volatile u16 *)(CFG_FLASH_BASE + (0x000002AA << 1)))
//#define MEM_FLASH_ADDR1(base)	(*(volatile u16 *)(base + (0x00000555<<1))
//#define MEM_FLASH_ADDR2(base)	(*(volatile u16 *)(base + (0x000002AA<<1))

typedef volatile u16 fltype;

#define BIT_ERASE_DONE		0x00000080
#define BIT_RDY_MASK		0x00000080
#define BIT_PROGRAM_ERROR	0x00000020
#define BIT_TIMEOUT		0x80000000 /* our flag */

#define READY 1
#define ERR   2
#define TMO   4

/*-----------------------------------------------------------------------
 */

ulong amd_flash_init(bd_t *bd)
{
    int i, j;
    ulong size = 0;

    for (i = 0; i < CFG_MAX_FLASH_BANKS; i++)
    {
	flash_info[i].flash_id =
	  (AMD_MANUFACT & FLASH_VENDMASK) |
	  //	  (AMD_ID_DL322B & FLASH_TYPEMASK);
	  (AMD_ID_LV320B & FLASH_TYPEMASK);
	flash_info[i].size = FLASH_BANK_SIZE;
	flash_info[i].sector_count = CFG_MAX_FLASH_SECT;
	memset(flash_info[i].protect, 0, CFG_MAX_FLASH_SECT);
	for (j = 0; j < flash_info[i].sector_count; j++)
	{
	    if (j <= 7)
	    {
		flash_info[i].start[j] = flashbase[i] + j * PARAM_SECT_SIZE;
	    }
	    else
	    {
		flash_info[i].start[j] = flashbase[i] + (j - 7)*MAIN_SECT_SIZE;
	    }
	}
	size += flash_info[i].size;
    }

    /*
     * Protect monitor and environment sectors
     * Inferno is complicated, it's hardware locked
     */
#ifdef CONFIG_INFERNO
    /* first one, 0x00000 to 0x07fff */
    flash_protect(FLAG_PROTECT_SET,
		  CFG_FLASH_BASE + 0x00000,
		  CFG_FLASH_BASE + 0x08000 - 1,
		  &flash_info[0]);

    /* third to 10th, 0x0c000 - 0xdffff */
    flash_protect(FLAG_PROTECT_SET,
		  CFG_FLASH_BASE + 0x0c000,
		  CFG_FLASH_BASE + 0xe0000 - 1,
		  &flash_info[0]);
#else
    flash_protect(FLAG_PROTECT_SET,
		  CFG_FLASH_BASE,
		  CFG_FLASH_BASE + _armboot_end - _armboot_start,
		  &flash_info[0]);

    flash_protect(FLAG_PROTECT_SET,
		  CFG_ENV_ADDR,
		  CFG_ENV_ADDR + CFG_ENV_SIZE - 1,
		  &flash_info[0]);
#endif
    return size;
}

/*-----------------------------------------------------------------------
 */
void amd_flash_print_info  (flash_info_t *info)
{
	int i;

	switch (info->flash_id & FLASH_VENDMASK)
	{
	case (AMD_MANUFACT & FLASH_VENDMASK):
		printf("AMD: ");
		break;
	default:
		printf("Unknown Vendor ");
		break;
	}

	switch (info->flash_id & FLASH_TYPEMASK)
	{
	case (AMD_ID_LV160B & FLASH_TYPEMASK):
		printf("2x Amd29F160BB (16Mbit)\n");
		break;
	case (AMD_ID_DL322B	&FLASH_TYPEMASK):
		printf("2x 29DL322B (32mBit)\n");
		break;
	case (AMD_ID_LV320B	&FLASH_TYPEMASK):
		printf("1x 29LV320B (32mBit)\n");
		break;
	default:
		printf("Unknown Chip Type\n");
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
	;
}

/*-----------------------------------------------------------------------
 */

int	amd_flash_erase (flash_info_t *info, int s_first, int s_last)
{
    ushort result;
    int iflag, cflag, prot, sect;
    int rc = ERR_OK;
    int chip;

    /* first look for protection bits */

    if (info->flash_id == FLASH_UNKNOWN)
	return ERR_UNKNOWN_FLASH_TYPE;

    if ((s_first < 0) || (s_first > s_last)) {
	return ERR_INVAL;
    }

    if ((info->flash_id & FLASH_VENDMASK) !=
	(AMD_MANUFACT & FLASH_VENDMASK)) {
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
	    vushort *addr = (vushort *)(info->start[sect]);

	    MEM_FLASH_ADDR1 = CMD_UNLOCK1;
	    MEM_FLASH_ADDR2 = CMD_UNLOCK2;
	    MEM_FLASH_ADDR1 = CMD_ERASE_SETUP;

	    MEM_FLASH_ADDR1 = CMD_UNLOCK1;
	    MEM_FLASH_ADDR2 = CMD_UNLOCK2;
	    *addr = CMD_ERASE_CONFIRM;

	    /* wait until flash is ready */
	    chip = 0;

	    do
	    {
		result = *addr;

		/* check timeout */
		if (get_timer_masked() > CFG_FLASH_ERASE_TOUT)
		{
		    MEM_FLASH_ADDR1 = CMD_READ_ARRAY;
		    chip = TMO;
		    break;
		}

		if (!chip && (result & 0xFFFF) & BIT_ERASE_DONE)
			chip = READY;

		if (!chip && (result & 0xFFFF) & BIT_PROGRAM_ERROR)
			chip = ERR;

	    }  while (!chip);

	    MEM_FLASH_ADDR1 = CMD_READ_ARRAY;

	    if (chip == ERR)
	    {
		rc = ERR_PROG_ERROR;
		goto outahere;
	    }
	    if (chip == TMO)
	    {
		rc = ERR_TIMOUT;
		goto outahere;
	    }

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

volatile static int write_hword (flash_info_t *info, ulong dest, ushort data)
{
    vushort *addr = (vushort *)dest;
    ushort result;
    int rc = ERR_OK;
    int cflag, iflag;
    int chip;

    /*
     * Check if Flash is (sufficiently) erased
     */
    result = *addr;
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

    MEM_FLASH_ADDR1 = CMD_UNLOCK1;
    MEM_FLASH_ADDR2 = CMD_UNLOCK2;
//    MEM_FLASH_ADDR1 = CMD_UNLOCK_BYPASS;
    MEM_FLASH_ADDR1 = CMD_PROGRAM;
    *addr = data;

    /* arm simple, non interrupt dependent timer */
    reset_timer_masked();

    /* wait until flash is ready */
    chip = 0;
    do
    {
	result = *addr;

	/* check timeout */
	if (get_timer_masked() > CFG_FLASH_ERASE_TOUT)
	{
	    chip = ERR | TMO;
	    break;
	}
	if (!chip && ((result & 0x80) == (data & 0x80)))
		chip = READY;

	if (!chip && ((result & 0xFFFF) & BIT_PROGRAM_ERROR))
	{
		result = *addr;

		if ((result & 0x80) == (data & 0x80))
			chip = READY;
		else
			chip = ERR;
	}

    }  while (!chip);


    *addr = CMD_READ_ARRAY;

    if (chip == ERR || *addr != data)
        rc = ERR_PROG_ERROR;

    if (iflag)
      enable_interrupts();

    if (cflag)
      icache_enable();

    return rc;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash.
 */

int amd_write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
    ulong cp, wp;
    int l;
    int i, rc;
    ushort data;

    wp = (addr & ~1);	/* get lower word aligned address */

    /*
     * handle unaligned start bytes
     */
    if ((l = addr - wp) != 0) {
	data = 0;
	for (i=0, cp=wp; i<l; ++i, ++cp) {
	    data = (data >> 8) | (*(uchar *)cp << 8);
	}
	for (; i<2 && cnt>0; ++i) {
	    data = (data >> 8) | (*src++ << 8);
	    --cnt;
	    ++cp;
	}
	for (; cnt==0 && i<2; ++i, ++cp) {
	    data = (data >> 8) | (*(uchar *)cp << 8);
	}

	if ((rc = write_hword(info, wp, data)) != 0) {
	    return (rc);
	}
	wp += 2;
    }

    /*
     * handle word aligned part
     */
    while (cnt >= 2) {
	data = *((vushort*)src);
	if ((rc = write_hword(info, wp, data)) != 0) {
	    return (rc);
	}
	src += 2;
	wp  += 2;
	cnt -= 2;
    }

    if (cnt == 0) {
	return ERR_OK;
    }

    /*
     * handle unaligned tail bytes
     */
    data = 0;
    for (i=0, cp=wp; i<2 && cnt>0; ++i, ++cp) {
	data = (data >> 8) | (*src++ << 8);
	--cnt;
    }
    for (; i<2; ++i, ++cp) {
	data = (data >> 8) | (*(uchar *)cp << 8);
    }

    return write_hword(info, wp, data);
}

int amd_flash_read_user(int bank, int offset){
    
	fltype* addr;
	int val;

	if (bank == 1){
	    addr = (fltype*) PHYS_FLASH_1;
	}
	//	else if (bank == 2){
	//	    addr = (fltype*) PHYS_FLASH_2;
	//	}
	else{
	    return -1;
	}
	MEM_FLASH_ADDR1 = CMD_UNLOCK1;
	MEM_FLASH_ADDR2 = CMD_UNLOCK2;
	MEM_FLASH_ADDR1 = CMD_ENTER_SECSI;

	val   = *(addr + (offset >> 1));
	if (offset & 0x1){
		val >>= 8;
	}
	val &= 0xff;

	MEM_FLASH_ADDR1 = CMD_UNLOCK1;
	MEM_FLASH_ADDR2 = CMD_UNLOCK2;
	MEM_FLASH_ADDR1 = CMD_EXIT_SECSI;
	MEM_FLASH_ADDR1 = 0;
	return val;

   
}
