/*
 * (C) Copyright 2000
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

#include "version.h"
#include "armboot.h"

const char version_string[] = 
	ARMBOOT_VERSION " (" __DATE__ " - " __TIME__ ")";

void display_banner(bd_t *bd)
{
    printf ("\n\n%s\n\n", version_string);
    printf("ARMboot code: %08lx -> %08lx\n", _armboot_start, _armboot_end);
#ifdef CONFIG_USE_IRQ
    printf("IRQ Stack: %08lx\n", IRQ_STACK_START);
    printf("FIQ Stack: %08lx\n", FIQ_STACK_START);
#endif
}

static void pretty_print_size(ulong size)
{
    if (size > 0x100000)
      printf("%ld MB", size / 0x100000);
    else
      printf("%ld KB\n", size / 0x400);
}

void display_dram_config(bd_t *bd)
{
    int i;
   
    printf("DRAM Configuration:\n");
   
    for(i=0; i<CONFIG_NR_DRAM_BANKS; i++)
    {
	printf("Bank #%d: %08lx ", i, bd->bi_dram[i].start);
	pretty_print_size(bd->bi_dram[i].size);
	printf("\n");
    }
}

void display_flash_config(bd_t *bd, ulong size)
{
    printf("Flash: ");
    pretty_print_size(size);
    printf("\n");
}
