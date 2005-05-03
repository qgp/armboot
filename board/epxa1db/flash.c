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
#include "flash_amd.h"
#include "flash_int.h"


/* Default to Intel */
void (*epxa_flash_print_info)(flash_info_t *info)=int_flash_print_info;
int (*epxa_flash_erase) (flash_info_t *info, int s_first, int s_last)=int_flash_erase;
int (*epxa_write_buff) (flash_info_t *info, uchar *src, ulong addr, ulong cnt)=int_write_buff;
int (*epxa_flash_read_user)(int bank, int offset)=int_flash_read_user;

typedef volatile unsigned short fltype;
flash_info_t  flash_info[CFG_MAX_FLASH_BANKS];

#define PUZZLE_FROM_FLASH(x)	(x)
#define PUZZLE_TO_FLASH(x)	(x)

#define CMD_READ_ARRAY		0x00FF
#define CMD_READ_CFI	        0x0098


/*-----------------------------------------------------------------------
 */

ulong flash_init(bd_t *bd)
{

     fltype *addr=(fltype*)(PHYS_FLASH_1+0xaa);
     fltype val;


    /* default is Intel Flash_devices, we'll assume that we've got
     * One type or other and not one of each, so if the the first
     * device proves not to be Intel then we'll assume that they're
     * both AMD. If they're not then we don;t support them and we're 
     * knackered.
     */


     *addr = PUZZLE_TO_FLASH(CMD_READ_CFI);
     val=PUZZLE_FROM_FLASH(*(fltype*)(PHYS_FLASH_1 + 0x26));
     *addr = PUZZLE_TO_FLASH(CMD_READ_ARRAY);

     if(val==3){
	     return int_flash_init(bd);
     }else if(val==2){
     
	     /* its an AMD type thing */
	     
	     epxa_flash_print_info=amd_flash_print_info;
	     epxa_flash_erase=amd_flash_erase;
	     epxa_write_buff=amd_write_buff;
	     epxa_flash_read_user=amd_flash_read_user;
	     return amd_flash_init(bd);
     } else{
	     printf("Warning: unrecognised CFI command set:%d\n",val);
	     printf("Trying command set 3 instead, consider yourself lucky if it works\n");
             return int_flash_init(bd);
     }
}
void flash_print_info(flash_info_t *info){
	epxa_flash_print_info(info);
}

int flash_erase(flash_info_t *info, int s_first, int s_last){
	return epxa_flash_erase(info,s_first,s_last);
}

int write_buff(flash_info_t *info, uchar *src, ulong addr, ulong cnt){
	return epxa_write_buff(info,src,addr,cnt);
}

int flash_read_user(int bank, int offset){
	return epxa_flash_read_user(bank,offset);
}
