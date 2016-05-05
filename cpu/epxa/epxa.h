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

#define BOOT_CR_ADDR    (0x7fffc000)
#define BOOT_CR_VALUE   (0x00000001)

#define MMAP_EBI0_ADDR        (0x7fffc0c0)
#define MMAP_EBI0_VALUE       (0x40000b01)
#define MMAP_EBI0_BASE        (0x40000000)

#define MMAP_REGISTERS_ADDR   (0x7fffc080)
#define MMAP_REGISTERS_VALUE  (0x7fffc683)
#define MMAP_REGISTERS_BASE   (0x7fffc000)

#define MMAP_DPSRAM0_ADDR     (0x7fffc0a0)
#define MMAP_DPSRAM0_VALUE    (0x20000681)
#define MMAP_DPSRAM0_BASE     (0x20000000)

#define SCRATCH_BASE     (0x20000000)
#define SCRATCH_SIZE     (0x00004000)

/* this is generic */
#define CLK_PLL1_NCNT_ADDR (0x7fffc300)
#define CLK_PLL1_MCNT_ADDR (0x7fffc304)
#define CLK_PLL1_KCNT_ADDR (0x7fffc308)
#define CLK_PLL1_CTRL_ADDR (0x7fffc30c)
#define CLK_PLL2_NCNT_ADDR (0x7fffc310)
#define CLK_PLL2_MCNT_ADDR (0x7fffc314)
#define CLK_PLL2_KCNT_ADDR (0x7fffc318)
#define CLK_PLL2_CTRL_ADDR (0x7fffc31c)
#define CLK_DERIVE_ADDR    (0x7fffc320)

#define CLK_PLL1_NCNT_VALUE (0x00040000)
#define CLK_PLL1_MCNT_VALUE (0x00020404)
#define CLK_PLL1_KCNT_VALUE (0x00040000)
#define CLK_PLL1_CTRL_VALUE (0x00001055)
#define CLK_PLL2_NCNT_VALUE (0x00040000)
#define CLK_PLL2_MCNT_VALUE (0x00020606)
#define CLK_PLL2_KCNT_VALUE (0x00020101)
#define CLK_PLL2_CTRL_VALUE (0x00001055)
#define CLK_DERIVE_VALUE    (0x00000010)

#define IOCR_SDRAM_ADDR   (0x7fffc040)
#define IOCR_SDRAM_VALUE  (0x00000007)
#define IOCR_EBI_ADDR     (0x7fffc044)
#define IOCR_EBI_VALUE    (0x00000003)
#define IOCR_UART_ADDR    (0x7fffc048)
#define IOCR_UART_VALUE   (0x00000003)
#define IOCR_TRACE_ADDR   (0x7fffc04c)

#define MMAP_SDRAM_ADDR   (0x7fffc0b0)
#define MMAP_SDRAM_VALUE  (0x00000c01)
#define MMAP_PLD_ADDR     (0x7fffc0d0)
#define MMAP_PLD_VALUE    (0x80000f03)

#define DPSRAM0_SR_ADDR   (0x7fffc034)
#define DPSRAM0_SR_VALUE  (0x00000000)

#define INT_MODE_ADDR     (0x7fffcc18)
#define INT_MODE_VALUE    (0x00000003)

#define EBI_BLOCK0_ADDR   (0x7fffc390)
#define EBI_BLOCK0_VALUE  (0x00000014)

#define EBI_CR_ADDR       (0x7fffc380)
#define EBI_CR_VALUE      (0x00080000)

#define CLK_STATUS_ADDR   (0x7fffc324)
#define CLK_STATUS_VALUE  (0x0000000c)

#define SDRAM_WIDTH_ADDR  (0x7fffc07c)
#define SDRAM_WIDTH_VALUE (0x00000000)

#define SDRAM_TIMING1_ADDR  (0x7fffc400)
#define SDRAM_TIMING1_VALUE (0x00004c92)
#define SDRAM_TIMING2_ADDR  (0x7fffc404)
#define SDRAM_TIMING2_VALUE (0x000087c0)
#define SDRAM_CONFIG_ADDR   (0x7fffc408)
#define SDRAM_CONFIG_VALUE  (0x00000000)
#define SDRAM_REFRESH_ADDR  (0x7fffc40c)
#define SDRAM_REFRESH_VALUE (0x00000383)
#define SDRAM_ADDR_ADDR     (0x7fffc410)
#define SDRAM_ADDR_VALUE    (0x0000d980)

#define SDRAM_MODE0_ADDR    (0x7fffc420)
#define SDRAM_MODE0_VALUE   (0x00000023)
#define SDRAM_INIT_ADDR     (0x7fffc41c)
#define SDRAM_INIT_VALUE1   (0x00008000)
#define SDRAM_INIT_VALUE2   (0x0000c000)
#define SDRAM_INIT_VALUE3   (0x00008800)
#define SDRAM_INIT_VALUE4   (0x00008800)
#define SDRAM_INIT_VALUE5   (0x0000a000)
