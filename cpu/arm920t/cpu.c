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

/*
 * CPU specific code
 */

#include "armboot.h"
#include "command.h"
#include "arm920t.h"

/* it makes no sense to use the caches if the MMU also isn't used */
#ifdef USE_920T_MMU
/* read co-processor 15, register #1 (control register) */
static unsigned long read_p15_c1(void)
{
    unsigned long value;

    __asm__ __volatile__(
	"mrc     p15, 0, %0, c1, c0, 0   @ read control reg\n"
	: "=r" (value)
	:
	: "memory");
#ifdef MMU_DEBUG
    printf("p15/c1 is = %08lx\n", value);
#endif
    return value;
}

/* write to co-processor 15, register #1 (control register) */
static void write_p15_c1(unsigned long value)
{
#ifdef MMU_DEBUG
    printf("write %08lx to p15/c1\n", value);
#endif
    __asm__ __volatile__(
        "mcr     p15, 0, %0, c1, c0, 0   @ write it back\n"
	:
	: "r" (value)
	: "memory");
    read_p15_c1();
}

static void cp_delay(void)
{
    volatile int i;

    /* copro seems to need some delay between reading and writing */
    for (i=0; i<100; i++);
}

/* See also ARM Ref. Man. */
#define C1_MMU		(1<<0)	/* mmu off/on */
#define C1_ALIGN	(1<<1)	/* alignment faults off/on */
#define C1_DC		(1<<2)	/* dcache off/on */
#define C1_BIG_ENDIAN	(1<<7)	/* big endian off/on */
#define C1_SYS_PROT	(1<<8)	/* system protection */
#define C1_ROM_PROT	(1<<9)	/* ROM protection */
#define C1_IC		(1<<12)	/* icache off/on */
#define C1_HIGH_VECTORS	(1<<13)	/* location of vectors: low/high addresses */
#define RESERVED_1	(0xf << 3) /* must be 111b for R/W */
#endif

void cpu_init(bd_t *bd)
{
    /*
     * setup up stack if necessary
     */
#ifdef CONFIG_USE_IRQ
    IRQ_STACK_START = _armboot_end +
      			CONFIG_STACKSIZE + CONFIG_STACKSIZE_IRQ - 4;
    FIQ_STACK_START = IRQ_STACK_START + CONFIG_STACKSIZE_FIQ;
    _armboot_real_end = FIQ_STACK_START + 4;
#else
    _armboot_real_end = _armboot_end + CONFIG_STACKSIZE;
#endif
}

void cleanup_before_linux(bd_t *bd)
{
    /*
     * this function is called just before we call linux
     * it prepares the processor for linux
     *
     * we turn off caches etc ...
     */

#ifdef USE_920T_MMU
    unsigned long i;
#endif

    disable_interrupts();

#ifdef USE_920T_MMU
    /* turn off I/D-cache */
    asm ("mrc p15, 0, %0, c1, c0, 0": "=r" (i));
    i &= ~(C1_DC|C1_IC);
    asm ("mcr p15, 0, %0, c1, c0, 0": : "r" (i));

    /* flush I/D-cache */
    i = 0;
    asm ("mcr p15, 0, %0, c7, c7, 0": : "r" (i));
#endif

}

void do_reset (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
    extern void reset_cpu(ulong addr);

    disable_interrupts();
    reset_cpu(0);
    /*NOTREACHED*/
}

#ifdef USE_920T_MMU
void icache_enable(void)
{
    ulong reg;
    reg = read_p15_c1();
    cp_delay();
    write_p15_c1(reg | C1_IC);
}

void icache_disable(void)
{
    ulong reg;
    reg = read_p15_c1();
    cp_delay();
    write_p15_c1(reg & ~C1_IC);
}

int icache_status(void)
{
    return (read_p15_c1() & C1_IC) != 0;
}

void dcache_enable(void)
{
    ulong reg;
    reg = read_p15_c1();
    cp_delay();
    write_p15_c1(reg | C1_DC);
}

void dcache_disable(void)
{
    ulong reg;
    reg = read_p15_c1();
    cp_delay();
    reg &= ~C1_DC;
    write_p15_c1(reg);
}

int dcache_status(void)
{
    return (read_p15_c1() & C1_DC) != 0;
}
#else
/* I don't know if this is the right solution for
 . the samsung board.
 . Otherwise one would have to #ifdef the "flush_all_caches()"
 . function in common/cmd_boot.c
 */
void icache_enable(void) {}
void icache_disable(void) {}
int icache_status(void) {}
void dcache_enable(void) {}
void dcache_disable(void) {}
int dcache_status(void) {}

#endif /* USE_920T_MMU */
