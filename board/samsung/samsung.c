/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
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
#include "s3c2400.h"

/* ------------------------------------------------------------------------- */


/*
 * Miscellaneous platform dependent initialisations
 */

int board_init(bd_t *bd)
{
    /* memory and cpu-speed are setup before relocation */
    /* change the clock to be 50 MHz 1:1:1 */
    rMPLLCON = 0x5c042;
    rCLKDIVN = 0;
    /* set up the I/O ports */
    rPACON = 0x3ffff;
    rPBCON=0xaaaaaaaa;
    rPBUP=0xffff;
    rPECON=0x0;
    rPEUP=0x0;
    /*INPUT INPUT INPUT INPUT TXD[0] INPUT RXD[0] */
    /*   00,   00,   00,   00,    10,   00,    10 */
    rPFCON=0x22;
    rPFUP=0x5;
    rPGCON=0x0;
    rPGUP=0x0;
    rOPENCR=0x0;

    /* arch number of SAMSUNG-Board */
    /* I have no idea what this means, so I just picked an unused value */
    bd->bi_arch_number = 28;

    /* adress of boot parameters */
    bd->bi_boot_params = 0x0c000100;

    return 1;
}

int dram_init(bd_t *bd)
{
    bd->bi_dram[0].start = PHYS_SDRAM_1;
    bd->bi_dram[0].size  = PHYS_SDRAM_1_SIZE;
    return PHYS_SDRAM_1_SIZE;
}
