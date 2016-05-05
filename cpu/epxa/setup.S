/*
 * (C) Copyright 2015 Jochen Klein
 * All rights reserved.
 *
 * This program is free software ; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation ; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY ; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program ; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
*/

	.section .init

.globl ___altera_entry

#include "epxa.h"

	@ entry point from start.S
	@ configure the device
___altera_entry:
	@ store address offset w.r.t. link time in r4
	@ to allow for position-independent execution
	ldr r4, =exp_pc
	subs r4, pc, r4
	ldr r8, =BOOT_CR_ADDR

exp_pc:
	ldr r9, =BOOT_CR_VALUE

	bne begin_config
	ldr r6, [r8]
	tst r6, r9
	movne r0, #0
	movne r1, #0
	movne r2, #0
	movne r3, #0

begin_config:
	@ map flash on EBI0 to memory, jump there, and adjust pc offset
	ldr r5, =MMAP_EBI0_ADDR
	ldr r6, =MMAP_EBI0_VALUE
	str r6, [r5]
	add pc, pc, #MMAP_EBI0_BASE
	nop
	add r4, r4, #MMAP_EBI0_BASE

	@ set boot control register
	str r9, [r8]

	@ map registers to memory
	ldr sl, =MMAP_REGISTERS_ADDR
	ldr fp, =MMAP_REGISTERS_VALUE
	str fp, [sl]

	@ set no-prefetch bit for EBI0
	orr r6, r6, #2
	str r6, [r5]

	@ no adjustment of pc offset needed
	ldr fp, =in_final_ebi0
	add pc, fp, r4

in_final_ebi0:
	@ map dual-port SRAM to memory
	ldr r5, =MMAP_DPSRAM0_ADDR
	ldr r6, =MMAP_DPSRAM0_VALUE
	str r6, [r5]

	@ enable cache
	@ read-modify-write control register of coprocessor 15
	@ setting bits 12 (ICache enable) and 14 (Round robin replacement)
	mrc 15, 0, r6, cr1, cr0, 0
	orr r6, r6, #0x5000
	mcr 15, 0, r6, cr1, cr0, 0

	@ prepare stack at offset 0x4000 of DPSRAM0
	ldr sl, =MMAP_DPSRAM0_BASE
	add sp, sl, #0x4000

	@ store registers on stack (downwards)
	stmdb sp!, {r0, r1, r2, r3}

	@ PLL setup
	ldr r1, =CLK_PLL1_NCNT_ADDR
	ldr r2, =CLK_PLL1_NCNT_VALUE
	str r2, [r1]
	ldr r1, =CLK_PLL1_MCNT_ADDR
	ldr r2, =CLK_PLL1_MCNT_VALUE
	str r2, [r1]
	ldr r1, =CLK_PLL1_KCNT_ADDR
	ldr r2, =CLK_PLL1_KCNT_VALUE
	str r2, [r1]
	ldr r1, =CLK_PLL1_CTRL_ADDR
	ldr r2, =CLK_PLL1_CTRL_VALUE
	str r2, [r1]

	ldr r1, =CLK_PLL2_NCNT_ADDR
	ldr r2, =CLK_PLL2_NCNT_VALUE
	str r2, [r1]
	ldr r1, =CLK_PLL2_MCNT_ADDR
	ldr r2, =CLK_PLL2_MCNT_VALUE
	str r2, [r1]
	ldr r1, =CLK_PLL2_KCNT_ADDR
	ldr r2, =CLK_PLL2_KCNT_VALUE
	str r2, [r1]
	ldr r1, =CLK_PLL2_CTRL_ADDR
	ldr r2, =CLK_PLL2_CTRL_VALUE
	str r2, [r1]

	ldr r1, =CLK_DERIVE_ADDR
	ldr r2, =CLK_DERIVE_VALUE
	str r2, [r1]

	@ I/O control
	ldr r1, =IOCR_SDRAM_ADDR
	ldr r2, =IOCR_SDRAM_VALUE
	str r2, [r1]
	ldr r1, =IOCR_EBI_ADDR
	ldr r2, =IOCR_EBI_VALUE
	str r2, [r1]
	ldr r1, =IOCR_UART_ADDR
	ldr r2, =IOCR_UART_VALUE
	str r2, [r1]

	@ memory mapping
	ldr r1, =MMAP_SDRAM_ADDR
	ldr r2, =MMAP_SDRAM_VALUE
	str r2, [r1]
	ldr r1, =MMAP_PLD_ADDR
	ldr r2, =MMAP_PLD_VALUE
	str r2, [r1]

	@ DPSRAM
	ldr r1, =DPSRAM0_SR_ADDR
	ldr r2, =DPSRAM0_SR_VALUE
	str r2, [r1]

	@ interrupt controller
	ldr r1, =INT_MODE_ADDR
	ldr r2, =INT_MODE_VALUE
	str r2, [r1]

	@ EBI setup
	ldr r1, =EBI_BLOCK0_ADDR
	ldr r2, =EBI_BLOCK0_VALUE
	str r2, [r1]

	ldr r1, =EBI_CR_ADDR
	ldr r2, =EBI_CR_VALUE
	str r2, [r1]

	@ wait for PLLs

	@ clock setup
	ldr r1, =CLK_STATUS_ADDR
	ldr r2, =CLK_STATUS_VALUE
	str r2, [r1]

	@ wait for 0x3e80 / 4 = 4000

	@ SDRAM setup
	ldr r1, =SDRAM_WIDTH_ADDR
	ldr r2, =SDRAM_WIDTH_VALUE
	str r2, [r1]
	ldr r1, =SDRAM_TIMING1_ADDR
	ldr r2, =SDRAM_TIMING1_VALUE
	str r2, [r1]
	ldr r1, =SDRAM_TIMING2_ADDR
	ldr r2, =SDRAM_TIMING2_VALUE
	str r2, [r1]
	ldr r1, =SDRAM_CONFIG_ADDR
	ldr r2, =SDRAM_CONFIG_VALUE
	str r2, [r1]
	ldr r1, =SDRAM_REFRESH_ADDR
	ldr r2, =SDRAM_REFRESH_VALUE
	str r2, [r1]
	ldr r1, =SDRAM_ADDR_ADDR
	ldr r2, =SDRAM_ADDR_VALUE
	str r2, [r1]

	ldr r1, =SDRAM_MODE0_ADDR
	ldr r2, =SDRAM_MODE0_VALUE
	str r2, [r1]

	ldr r1, =SDRAM_INIT_ADDR
	ldr r2, =SDRAM_INIT_VALUE1
	str r2, [r1]

	ldr r1, =SDRAM_INIT_ADDR
	ldr r2, =SDRAM_INIT_VALUE2
	str r2, [r1]

	@ wait

	ldr r1, =SDRAM_INIT_ADDR
	ldr r2, =SDRAM_INIT_VALUE3
	str r2, [r1]

	@ wait

	ldr r1, =SDRAM_INIT_ADDR
	ldr r2, =SDRAM_INIT_VALUE4
	str r2, [r1]

	@ wait

	ldr r1, =SDRAM_INIT_ADDR
	ldr r2, =SDRAM_INIT_VALUE5
	str r2, [r1]

	@ restore values from stack
	ldmia sp!, {r0, r1, r2, r3}

	@ cleanup registers
	mov r8, #0
	mov r9, #0
	mov sl, #0
	mov fp, #0
	mov ip, #0
	cmp r7, #0
	mov r7, #0

	@ disable bits 12 and 14 again
	mrc 15, 0, sp, cr1, cr0, 0
	bic sp, sp, #0x5000
#ifdef BIG_ENDIAN
	orr sp, sp, #0x80
#endif
	mcr 15, 0, sp, cr1, cr0, 0
	mcr 15, 0, sp, cr7, cr5, 0

	@ done, now branch to ___altera_user_start
	b ___altera_user_start
.text
