#ifndef __CLOCK_CTRL00_H
#define __CLOCK_CTRL00_H

/*
 * Register definitions for the Clock Control Logic
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
 
#define CLK_PLL1_NCNT(base_addr) (CLOCK_CTRL00_TYPE (base_addr  ))
#define CLK_PLL1_NCNT_LSB_MSK (0x7)
#define CLK_PLL1_NCNT_LSB_OFST (0)
#define CLK_PLL1_NCNT_MSB_MSK (0xF00)
#define CLK_PLL1_NCNT_MSB_OFST (8)
#define CLK_PLL1_NCNT_CT_MSK (0x70000)
#define CLK_PLL1_NCNT_CT_OFST (16)

#define CLK_PLL1_MCNT(base_addr) (CLOCK_CTRL00_TYPE (base_addr  + 0x4 ))
#define CLK_PLL1_MCNT_LSB_MSK (0x7)
#define CLK_PLL1_MCNT_LSB_OFST (0)
#define CLK_PLL1_MCNT_MSB_MSK (0xF00)
#define CLK_PLL1_MCNT_MSB_OFST (8)
#define CLK_PLL1_MCNT_CT_MSK (0x70000)
#define CLK_PLL1_MCNT_CT_OFST (16)

#define CLK_PLL1_KCNT(base_addr) (CLOCK_CTRL00_TYPE (base_addr  + 0x8 ))
#define CLK_PLL1_KCNT_LSB_MSK (0x7)
#define CLK_PLL1_KCNT_LSB_OFST (0)
#define CLK_PLL1_KCNT_MSB_MSK (0xF00)
#define CLK_PLL1_KCNT_MSB_OFST (8)
#define CLK_PLL1_KCNT_CT_MSK (0x70000)
#define CLK_PLL1_KCNT_CT_OFST (16)

#define CLK_PLL1_CTRL(base_addr) (CLOCK_CTRL00_TYPE (base_addr  + 0xC ))
#define CLK_PLL1_CTRL_P_MSK  (0x1)
#define CLK_PLL1_CTRL_P_OFST (0)
#define CLK_PLL1_CTRL_CTRL_MSK (0x7FFE)
#define CLK_PLL1_CTRL_CTRL_OFST (1)

#define CLK_PLL2_NCNT(base_addr) (CLOCK_CTRL00_TYPE (base_addr  + 0x10 ))
#define CLK_PLL2_NCNT_LSB_MSK (0x7)
#define CLK_PLL2_NCNT_LSB_OFST (0)
#define CLK_PLL2_NCNT_MSB_MSK (0xF00)
#define CLK_PLL2_NCNT_MSB_OFST (8)
#define CLK_PLL2_NCNT_CT_MSK (0x70000)
#define CLK_PLL2_NCNT_CT_OFST (16)

#define CLK_PLL2_MCNT(base_addr) (CLOCK_CTRL00_TYPE (base_addr  + 0x14 ))
#define CLK_PLL2_MCNT_LSB_MSK (0x7)
#define CLK_PLL2_MCNT_LSB_OFST (0)
#define CLK_PLL2_MCNT_MSB_MSK (0xF00)
#define CLK_PLL2_MCNT_MSB_OFST (8)
#define CLK_PLL2_MCNT_CT_MSK (0x70000)
#define CLK_PLL2_MCNT_CT_OFST (16)

#define CLK_PLL2_KCNT(base_addr) (CLOCK_CTRL00_TYPE (base_addr  + 0x18 ))
#define CLK_PLL2_KCNT_LSB_MSK (0x7)
#define CLK_PLL2_KCNT_LSB_OFST (0)
#define CLK_PLL2_KCNT_MSB_MSK (0xF00)
#define CLK_PLL2_KCNT_MSB_OFST (8)
#define CLK_PLL2_KCNT_CT_MSK (0x70000)
#define CLK_PLL2_KCNT_CT_OFST (16)

#define CLK_PLL2_CTRL(base_addr) (CLOCK_CTRL00_TYPE (base_addr  + 0x1C ))
#define CLK_PLL2_CTRL_P_MSK  (0x1)
#define CLK_PLL2_CTRL_P_OFST (0)
#define CLK_PLL2_CTRL_CTRL_MSK (0x7FFE)
#define CLK_PLL2_CTRL_CTRL_OFST (0)

#define CLK_DERIVE(base_addr) (CLOCK_CTRL00_TYPE (base_addr  + 0x20 ))
#define CLK_DERIVE_DF2_MSK (0x30)
#define CLK_DERIVE_DF2_OFST (4)
#define CLK_DERIVE_BP1_MSK (0x1000)
#define CLK_DERIVE_BP1_OFST (12)
#define CLK_DERIVE_BP2_MSK (0x2000)
#define CLK_DERIVE_BP2_OFST (13)

#define CLK_STATUS(base_addr) (CLOCK_CTRL00_TYPE (base_addr  + 0x24 ))
#define CLK_STATUS_L1_MSK (0x1)
#define CLK_STATUS_L1_OFST (0)
#define CLK_STATUS_L2_MSK (0x2)
#define CLK_STATUS_L2_OFST (1)
#define CLK_STATUS_C1_MSK (0x4)
#define CLK_STATUS_C1_OFST (2)
#define CLK_STATUS_C2_MSK (0x8)
#define CLK_STATUS_C2_OFST (3)
#define CLK_STATUS_P1_MSK (0x10)
#define CLK_STATUS_P1_OFST (4)
#define CLK_STATUS_P2_MSK (0x20)
#define CLK_STATUS_P2_OFST (5)

#define CLK_AHB1_COUNT(base_addr) (CLOCK_CTRL00_TYPE (base_addr  + 0x28 ))
#define CLK_AHB1_COUNT_MSK (0xFFFFFFFF)
#define CLK_AHB1_COUNT_OFST (0)

#endif /* __CLOCK_CTRL00_H */
