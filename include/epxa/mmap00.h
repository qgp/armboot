#ifndef __MMAP00_H
#define __MMAP00_H

/*
 * Register definitions to configure the memory map
 */

/*
 * Copyright (c) 2000-2002 Altera Corporation, San Jose, California, USA.  
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State 
 * of California and by the laws of the United States of America.
 */
 
#ifndef MMAP00_TYPE
#define MMAP00_TYPE
#warning MMAP00_TYPE not defined 
#endif

#define MMAP_REGISTERS(base_addr) (MMAP00_TYPE (base_addr  ))
#define MMAP_REGISTERS_EN_MSK (0x1)
#define MMAP_REGISTERS_EN_OFST (0x0)
#define MMAP_REGISTERS_EN_ENABLE (0x1)
#define MMAP_REGISTERS_NP_MSK (0x2)
#define MMAP_REGISTERS_NP_OFST (0x1)
#define MMAP_REGISTERS_NP_NO_PREFETCH (0x2)
#define MMAP_REGISTERS_SIZE_MSK (0xF80)
#define MMAP_REGISTERS_SIZE_OFST (0x7)
#define MMAP_REGISTERS_BASE_MSK (0xFFFFC000)
#define MMAP_REGISTERS_BASE_OFST (0xE)

#define MMAP_SRAM0(base_addr) (MMAP00_TYPE (base_addr  + 0x10 ))
#define MMAP_SRAM0_EN_MSK (0x1)
#define MMAP_SRAM0_EN_OFST (0x0)
#define MMAP_SRAM0_EN_ENABLE (0x1)
#define MMAP_SRAM0_NP_MSK (0x2)
#define MMAP_SRAM0_NP_OFST (0x1)
#define MMAP_SRAM0_NP_NO_PREFETCH (0x2)
#define MMAP_SRAM0_SIZE_MSK (0xF80)
#define MMAP_SRAM0_SIZE_OFST (0x7)
#define MMAP_SRAM0_BASE_MSK (0xFFFFC000)
#define MMAP_SRAM0_BASE_OFST (0xE)

#define MMAP_SRAM1(base_addr) (MMAP00_TYPE (base_addr  + 0x14 ))
#define MMAP_SRAM1_EN_MSK (0x1)
#define MMAP_SRAM1_EN_OFST (0x0)
#define MMAP_SRAM1_EN_ENABLE (0x1)
#define MMAP_SRAM1_NP_MSK (0x2)
#define MMAP_SRAM1_NP_OFST (0x1)
#define MMAP_SRAM1_NP_NO_PREFETCH (0x2)
#define MMAP_SRAM1_SIZE_MSK (0xF80)
#define MMAP_SRAM1_SIZE_OFST (0x7)
#define MMAP_SRAM1_BASE_MSK (0xFFFFC000)
#define MMAP_SRAM1_BASE_OFST (0xE)

#define MMAP_DPSRAM0(base_addr) (MMAP00_TYPE (base_addr  + 0x20 ))
#define MMAP_DPSRAM0_EN_MSK (0x1)
#define MMAP_DPSRAM0_EN_OFST (0x0)
#define MMAP_DPSRAM0_EN_ENABLE (0x1)
#define MMAP_DPSRAM0_NP_MSK (0x2)
#define MMAP_DPSRAM0_NP_OFST (0x1)
#define MMAP_DPSRAM0_NP_NO_PREFETCH (0x2)
#define MMAP_DPSRAM0_SIZE_MSK (0xF80)
#define MMAP_DPSRAM0_SIZE_OFST (0x7)
#define MMAP_DPSRAM0_BASE_MSK (0xFFFFC000)
#define MMAP_DPSRAM0_BASE_OFST (0xE)

#define MMAP_DPSRAM1(base_addr) (MMAP00_TYPE (base_addr  + 0x24 ))
#define MMAP_DPSRAM1_EN_MSK (0x1)
#define MMAP_DPSRAM1_EN_OFST (0x0)
#define MMAP_DPSRAM1_EN_ENABLE (0x1)
#define MMAP_DPSRAM1_NP_MSK (0x2)
#define MMAP_DPSRAM1_NP_OFST (0x1)
#define MMAP_DPSRAM1_NP_NO_PREFETCH (0x2)
#define MMAP_DPSRAM1_SIZE_MSK (0xF80)
#define MMAP_DPSRAM1_SIZE_OFST (0x7)
#define MMAP_DPSRAM1_BASE_MSK (0xFFFFC000)
#define MMAP_DPSRAM1_BASE_OFST (0xE)

#define MMAP_SDRAM0(base_addr) (MMAP00_TYPE (base_addr  + 0x30 ))
#define MMAP_SDRAM0_EN_MSK (0x1)
#define MMAP_SDRAM0_EN_OFST (0x0)
#define MMAP_SDRAM0_EN_ENABLE (0x1)
#define MMAP_SDRAM0_NP_MSK (0x2)
#define MMAP_SDRAM0_NP_OFST (0x1)
#define MMAP_SDRAM0_NP_NO_PREFETCH (0x2)
#define MMAP_SDRAM0_SIZE_MSK (0xF80)
#define MMAP_SDRAM0_SIZE_OFST (0x7)
#define MMAP_SDRAM0_BASE_MSK (0xFFFFC000)
#define MMAP_SDRAM0_BASE_OFST (0xE)

#define MMAP_SDRAM1(base_addr) (MMAP00_TYPE (base_addr  + 0x34 ))
#define MMAP_SDRAM1_EN_MSK (0x1)
#define MMAP_SDRAM1_EN_OFST (0x0)
#define MMAP_SDRAM1_EN_ENABLE (0x1)
#define MMAP_SDRAM1_NP_MSK (0x2)
#define MMAP_SDRAM1_NP_OFST (0x1)
#define MMAP_SDRAM1_NP_NO_PREFETCH (0x2)
#define MMAP_SDRAM1_SIZE_MSK (0xF80)
#define MMAP_SDRAM1_SIZE_OFST (0x7)
#define MMAP_SDRAM1_BASE_MSK (0xFFFFC000)
#define MMAP_SDRAM1_BASE_OFST (0xE)

#define MMAP_EBI0(base_addr) (MMAP00_TYPE (base_addr  + 0x40 ))
#define MMAP_EBI0_EN_MSK (0x1)
#define MMAP_EBI0_EN_OFST (0x0)
#define MMAP_EBI0_EN_ENABLE (0x1)
#define MMAP_EBI0_NP_MSK (0x2)
#define MMAP_EBI0_NP_OFST (0x1)
#define MMAP_EBI0_NP_NO_PREFETCH (0x2)
#define MMAP_EBI0_SIZE_MSK (0xF80)
#define MMAP_EBI0_SIZE_OFST (0x7)
#define MMAP_EBI0_BASE_MSK (0xFFFFC000)
#define MMAP_EBI0_BASE_OFST (0xE)

#define MMAP_EBI1(base_addr) (MMAP00_TYPE (base_addr  + 0x44 ))
#define MMAP_EBI1_EN_MSK (0x1)
#define MMAP_EBI1_EN_OFST (0x0)
#define MMAP_EBI1_EN_ENABLE (0x1)
#define MMAP_EBI1_NP_MSK (0x2)
#define MMAP_EBI1_NP_OFST (0x1)
#define MMAP_EBI1_NP_NO_PREFETCH (0x2)
#define MMAP_EBI1_SIZE_MSK (0xF80)
#define MMAP_EBI1_SIZE_OFST (0x7)
#define MMAP_EBI1_BASE_MSK (0xFFFFC000)
#define MMAP_EBI1_BASE_OFST (0xE)

#define MMAP_EBI2(base_addr) (MMAP00_TYPE (base_addr  + 0x48 ))
#define MMAP_EBI2_EN_MSK (0x1)
#define MMAP_EBI2_EN_OFST (0x0)
#define MMAP_EBI2_EN_ENABLE (0x1)
#define MMAP_EBI2_NP_MSK (0x2)
#define MMAP_EBI2_NP_OFST (0x1)
#define MMAP_EBI2_NP_NO_PREFETCH (0x2)
#define MMAP_EBI2_SIZE_MSK (0xF80)
#define MMAP_EBI2_SIZE_OFST (0x7)
#define MMAP_EBI2_BASE_MSK (0xFFFFC000)
#define MMAP_EBI2_BASE_OFST (0xE)

#define MMAP_EBI3(base_addr) (MMAP00_TYPE (base_addr  + 0x4C ))
#define MMAP_EBI3_EN_MSK (0x1)
#define MMAP_EBI3_EN_OFST (0x0)
#define MMAP_EBI3_EN_ENABLE (0x1)
#define MMAP_EBI3_NP_MSK (0x2)
#define MMAP_EBI3_NP_OFST (0x1)
#define MMAP_EBI3_NP_NO_PREFETCH (0x2)
#define MMAP_EBI3_SIZE_MSK (0xF80)
#define MMAP_EBI3_SIZE_OFST (0x7)
#define MMAP_EBI3_BASE_MSK (0xFFFFC000)
#define MMAP_EBI3_BASE_OFST (0xE)

#define MMAP_PLD0(base_addr) (MMAP00_TYPE (base_addr  + 0x50 ))
#define MMAP_PLD0_EN_MSK (0x1)
#define MMAP_PLD0_EN_OFST (0x0)
#define MMAP_PLD0_EN_ENABLE (0x1)
#define MMAP_PLD0_NP_MSK (0x2)
#define MMAP_PLD0_NP_OFST (0x1)
#define MMAP_PLD0_NP_NO_PREFETCH (0x2)
#define MMAP_PLD0_SIZE_MSK (0xF80)
#define MMAP_PLD0_SIZE_OFST (0x7)
#define MMAP_PLD0_BASE_MSK (0xFFFFC000)
#define MMAP_PLD0_BASE_OFST (0xE)

#define MMAP_PLD1(base_addr) (MMAP00_TYPE (base_addr  + 0x54 ))
#define MMAP_PLD1_EN_MSK (0x1)
#define MMAP_PLD1_EN_OFST (0x0)
#define MMAP_PLD1_EN_ENABLE (0x1)
#define MMAP_PLD1_NP_MSK (0x2)
#define MMAP_PLD1_NP_OFST (0x1)
#define MMAP_PLD1_NP_NO_PREFETCH (0x2)
#define MMAP_PLD1_SIZE_MSK (0xF80)
#define MMAP_PLD1_SIZE_OFST (0x7)
#define MMAP_PLD1_BASE_MSK (0xFFFFC000)
#define MMAP_PLD1_BASE_OFST (0xE)

#define MMAP_PLD2(base_addr) (MMAP00_TYPE (base_addr  + 0x58 ))
#define MMAP_PLD2_EN_MSK (0x1)
#define MMAP_PLD2_EN_OFST (0x0)
#define MMAP_PLD2_EN_ENABLE (0x1)
#define MMAP_PLD2_NP_MSK (0x2)
#define MMAP_PLD2_NP_OFST (0x1)
#define MMAP_PLD2_NP_NO_PREFETCH (0x2)
#define MMAP_PLD2_SIZE_MSK (0xF80)
#define MMAP_PLD2_SIZE_OFST (0x7)
#define MMAP_PLD2_BASE_MSK (0xFFFFC000)
#define MMAP_PLD2_BASE_OFST (0xE)

#define MMAP_PLD3(base_addr) (MMAP00_TYPE (base_addr  + 0x5C ))
#define MMAP_PLD3_EN_MSK (0x1)
#define MMAP_PLD3_EN_OFST (0x0)
#define MMAP_PLD3_EN_ENABLE (0x1)
#define MMAP_PLD3_NP_MSK (0x2)
#define MMAP_PLD3_NP_OFST (0x1)
#define MMAP_PLD3_NP_NO_PREFETCH (0x2)
#define MMAP_PLD3_SIZE_MSK (0xF80)
#define MMAP_PLD3_SIZE_OFST (0x7)
#define MMAP_PLD3_BASE_MSK (0xFFFFC000)
#define MMAP_PLD3_BASE_OFST (0xE)

#endif /* __MMAP00_H */
