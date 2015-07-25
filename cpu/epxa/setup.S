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
	ldr r4, exp_pc
	subs r4, pc, r4
	nop
exp_pc:
	@ initialize registers
	mov r0, #0
	mov r1, #0
	mov r2, #0
	mov r3, #0

	@ map flash on EBI0 to memory, jump there, and adjust pc offset
	ldr r5, =MMAP_EBI0_ADDR
	ldr r6, =MMAP_EBI0_VALUE
	str r6, [r5]
	add pc, pc, #MMAP_EBI0_BASE
	nop
	add r4, r4, #MMAP_EBI0_BASE

	@ set boot control register
	ldr r8, =BOOT_CR_ADDR
	ldr r9, =BOOT_CR_VALUE
	str r9, [r8]

	@ map registers to memory
	ldr sl, =MMAP_REGISTERS_ADDR
	ldr fp, =MMAP_REGISTERS_VALUE
	str fp, [sl]

	@ set no-prefetch bit for flash
	orr r6, r6, #2
	str r6, [r5]

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

	@ prepare stack at offset 0x4000 of SRAM
	mov sl, #MMAP_DPSRAM0_BASE
	add sp, sl, #0x4000

	@ store registers on stack (downwards)
	stmdb sp!, {r0, r1, r2, r3}

	@ finish CPU setup (incl. SDRAM setup)

	@ restore values from stack
	ldmia sp!, {r0, r1, r2, r3}

	@ prepare scratch region and use for stack

	stmdb sp!, {r0, r1, r2, r3}

	@ finish setup and program PLD (if wanted)

	ldmia sp!, {r0, r1, r2, r3}

	@ cleanup registers
	mov r7, #0
	mov r8, #0
	mov r9, #0
	mov sl, #0
	mov fp, #0
	mov ip, #0

	@ disable bits 12 and 14 again
	mrc 15, 0, sp, cr1, cr0, 0
	bic sp, sp, #0x5000
#ifdef BIG_ENDIAN
	orr sp, sp, #0x80
#endif
	mcr 15, 0, sp, cr1, cr0, 0
	mcr 15, 0, sp, cr7, cr5, 0

	@ done, now branch to ___altera_user_start
	b =___altera_user_start
.text
