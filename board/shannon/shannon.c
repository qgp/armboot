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
#include "command.h"
#include <SA-1100.h>

/* ------------------------------------------------------------------------- */


/*
 * Miscelaneous platform dependent initialisations
 */

struct clocksetup
{
	int           clockspeed;	/* frequency in MHz */
	unsigned long ppcr;			/* value for core clock config */
	unsigned long mdcas0;		/* CAS timing ... */
	unsigned long mdcas1;
	unsigned long mdcas2;
	unsigned long mdcnfg;
	unsigned long msc0;
	unsigned long mecr;			/* PCMCIA timing */
	unsigned long lccr3;		/* pixel clock divider */
};


const static struct clocksetup speeds[] =
{
	{
		103,
		0x00000003,
		0xcccccccf,
		0xfffffffc,
		0xffffffff,
		0x0326991f,
		0x42304230,
		0x00c600c6,
		0x00000008
	},
	{
		190,
		0x00000009,
		0xc71c703f,
		0xffc71c71,
		0xffffffff,
		0x01b4b21f,
		0x44584458,
		0x016b016b,
		0x0000000f
	},
	{
		206,
		0x0000000a,
		0xc71c703f,
		0xffc71c71,
		0xffffffff,
		0x0194b21f,
		0x84c084c0,
		0x018c018c,
		0x00000010
	}
};

int board_init(bd_t *bd)
{
    /* memory and cpu-speed are setup before relocation */
    /* but if we use InfernoLoader, we must do some inits here */

    #ifdef CONFIG_INFERNO
    {
        unsigned long temp;
        __asm__ __volatile__(/* disable MMU, enable icache */
                             "mrc p15, 0, %0, c1, c0\n"
                             "bic %0, %0, #0x00002000\n"
                             "bic %0, %0, #0x0000000f\n"
                             "orr %0, %0, #0x00001000\n"
                             "orr %0, %0, #0x00000002\n"
                             "mcr p15, 0, %0, c1, c0\n"
                             /* flush caches */
                             "mov %0, #0\n"
                             "mcr p15, 0, %0, c7, c7, 0\n"
                             "mcr p15, 0, %0, c8, c7, 0\n"
                             : "=r" (temp)
                             :
                             : "memory");
        /* setup PCMCIA timing */
        temp = 0xa0000018;
        *(unsigned long *)temp = 0x00060006;

    }
    #endif /* CONFIG_INFERNO */

    /* arch number for shannon */
    bd->bi_arch_number = 97;

    /* adress of boot parameters */
    bd->bi_boot_params = 0xc0000100;

    return 1;
}

int dram_init(bd_t *bd)
{
    #ifdef PHYS_SDRAM_1
    bd->bi_dram[0].start = PHYS_SDRAM_1;
    bd->bi_dram[0].size  = PHYS_SDRAM_1_SIZE;
    #endif

    #ifdef PHYS_SDRAM_2
    bd->bi_dram[1].start = PHYS_SDRAM_2;
    bd->bi_dram[1].size  = PHYS_SDRAM_2_SIZE;
    #endif

    #ifdef PHYS_SDRAM_3
    bd->bi_dram[2].start = PHYS_SDRAM_3;
    bd->bi_dram[2].size  = PHYS_SDRAM_3_SIZE;
    #endif

    #ifdef PHYS_SDRAM_4
    bd->bi_dram[3].start = PHYS_SDRAM_4;
    bd->bi_dram[3].size  = PHYS_SDRAM_4_SIZE;
    #endif

    return PHYS_SDRAM_1_SIZE + PHYS_SDRAM_2_SIZE
    #ifdef PHYS_SDRAM_4
           + PHYS_SDRAM_3_SIZE + PHYS_SDRAM_4_SIZE
    #endif
    ;
}

#ifdef CONFIG_CMD_SETCLOCK
int set_clock(int clockspeed)
{
	int i;

	for(i = 0; i < sizeof(speeds) / sizeof(speeds[0]); i++)
	{
		if(speeds[i].clockspeed == clockspeed)
		{
			PPCR     = speeds[i].ppcr;
			MSC0     = speeds[i].msc0;
			MDCAS0   = speeds[i].mdcas0;
			MDCAS1   = speeds[i].mdcas1;
			MDCAS2   = speeds[i].mdcas2;
			MDCNFG   = speeds[i].mdcnfg;
			MECR     = speeds[i].mecr;
			LCCR3    = speeds[i].lccr3;
			return(0);
		}
	}
	printf("Available clockspeeds are:\n");
	for(i = 0; i < sizeof(speeds) / sizeof(speeds[0]); i++)
		printf("   %d\n", speeds[i].clockspeed);
	return(-1);
}


int do_setclock (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
    if (argc != 2) {
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

    return(set_clock(simple_strtoul(argv[1], NULL, 10)));
}


#endif			
