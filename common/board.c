/*
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
#include "command.h"
#include "devices.h"
#include "version.h"

#ifdef CONFIG_DRIVER_CS8900
extern void cs8900_get_enetaddr(uchar *addr);
#endif

/*
 * Begin and End of memory area for malloc(), and current "brk"
 */
static  ulong   mem_malloc_start = 0;
static  ulong   mem_malloc_end   = 0;
static  ulong   mem_malloc_brk   = 0;
 
static void mem_malloc_init (ulong dest_addr)
{
    mem_malloc_start  = dest_addr;
    mem_malloc_end    = dest_addr + CONFIG_MALLOC_SIZE;
    mem_malloc_brk    = mem_malloc_start;
    
    memset ((void *)mem_malloc_start, 0, mem_malloc_end - mem_malloc_start);
}

void *sbrk (ptrdiff_t increment)
{
    ulong old = mem_malloc_brk;
    ulong new = old + increment;
    
    if ((new < mem_malloc_start) ||
	(new > mem_malloc_end) ) {
	return (NULL);
    }
    mem_malloc_brk = new;
    
    return ((void *)old);
}

/*
 * Breath some life into the board...
 *
 * Initialize an SMC for serial comms, and carry out some hardware
 * tests.
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */
void start_armboot(void)
{
    bd_t bd;
    ulong size;
   
    /* set up bd strucuture */
    memset(&bd, 0, sizeof(bd));
    bd.bi_baudrate = CONFIG_BAUDRATE;

    /* basic cpu dependent setup */
    cpu_init(&bd);
   
    /* basic board dependent setup */
    board_init(&bd);

    /* initialize environment */
    env_init(&bd);

    /* serial communications setup */
    serial_init(&bd);

    display_banner(&bd);

    /* set up execptions */
    interrupt_init(&bd);

    /* configure available RAM banks */
    dram_init(&bd);
    display_dram_config(&bd);

    /* configure available FLASH banks */
    size = flash_init(&bd);
    display_flash_config(&bd, size);

    /* armboot_end is defined in the board-specific linker script */
    mem_malloc_init(_armboot_real_end);

    /* initialize environment */
    env_relocate(&bd);

    /* enable exceptions */
    enable_interrupts();
    
#ifdef CONFIG_DRIVER_CS8900
    if (!getenv(&bd,"ethaddr") ) {
	cs8900_get_enetaddr(bd.bi_enetaddr);
    }
#endif    
    /* main_loop() can return to retry autoboot, if so just run it again. */
    for (;;) {
	main_loop(&bd);
    }

    /* NOTREACHED - no way out of command loop except booting */
}

void hang(void)
{
	puts ("### ERROR ### Please RESET the board ###\n");
	for (;;);
}
