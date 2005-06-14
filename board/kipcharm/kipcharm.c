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
#include "epxa/excalibur.h"

extern void epxa1db_set_mac_addr(void);

/* ------------------------------------------------------------------------- */

/*
 * Miscelaneous platform dependent initialisations
 */

int board_init(bd_t *bd)
{
    /* memory and cpu-speed are setup before relocation */
    /* so nothing to do in that respect */

    /* arch number of epxa1 Development Board */
    bd->bi_arch_number = 0x62;

    /* adress of boot parameters */
    bd->bi_boot_params = 0x100;

    return 1;
}

int dram_init(bd_t *bd)
{
    bd->bi_dram[0].start = EXC_SDRAM_BLOCK0_BASE;
    bd->bi_dram[0].size  = EXC_SDRAM_BLOCK0_SIZE;
    return EXC_SDRAM_BLOCK0_SIZE;
}







