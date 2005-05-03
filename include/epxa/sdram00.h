#ifndef __SDRAM00_H
#define __SDRAM00_H

/*
 * Register definitions for the SDRAM Interface
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
 
#define SDRAM_TIMING1(base_addr) (SDRAM00_TYPE (base_addr  + 0x0 ))
#define SDRAM_TIMING1_WR_MSK (0x7)
#define SDRAM_TIMING1_WR_OFST (0)
#define SDRAM_TIMING1_RP_MSK (0x38)
#define SDRAM_TIMING1_RP_OFST (3)
#define SDRAM_TIMING1_RRD_MSK (0x1C0)
#define SDRAM_TIMING1_RRD_OFST (6)
#define SDRAM_TIMING1_RAS_MSK (0x1E00)
#define SDRAM_TIMING1_RAS_OFST (9)
#define SDRAM_TIMING1_RCD_MSK (0x7000)
#define SDRAM_TIMING1_RCD_OFST (13)

#define SDRAM_TIMING2(base_addr) (SDRAM00_TYPE (base_addr  + 0x4 ))
#define SDRAM_TIMING2_RFC_MSK (0x78)
#define SDRAM_TIMING2_RFC_OFST (3)
#define SDRAM_TIMING2_BL_MSK (0x180)
#define SDRAM_TIMING2_BL_OFST (7)
#define SDRAM_TIMING2_CL_MSK (0xE00)
#define SDRAM_TIMING2_CL_OFST (9)
#define SDRAM_TIMING2_RC_MSK (0xF000)
#define SDRAM_TIMING2_RC_OFST (12)

#define SDRAM_CONFIG(base_addr) (SDRAM00_TYPE (base_addr  + 0x8 ))
#define SDRAM_CONFIG_MT_MSK (0x8000)
#define SDRAM_CONFIG_MT_OFST (15)

#define SDRAM_REFRESH(base_addr) (SDRAM00_TYPE (base_addr  + 0xC ))
#define SDRAM_REFRESH_RFSH_MSK (0xFFFF)
#define SDRAM_REFRESH_RFSH_OFST (0)

#define SDRAM_ADDR(base_addr) (SDRAM00_TYPE (base_addr  + 0x10 ))
#define SDRAM_ADDR_BA_MSK (0xC0)
#define SDRAM_ADDR_BA_OFST (6)
#define SDRAM_ADDR_COL_MSK (0xF00)
#define SDRAM_ADDR_COL_OFST (8)
#define SDRAM_ADDR_ROW_MSK (0xF000)
#define SDRAM_ADDR_ROW_OFST (12)

#define SDRAM_BUS1(base_addr) (SDRAM00_TYPE (base_addr  + 0x14 ))
#define SDRAM_BUS1_BM_MSK (0xF000)
#define SDRAM_BUS1_BM_OFST (15)

#define SDRAM_BUS2(base_addr) (SDRAM00_TYPE (base_addr  + 0x18 ))
#define SDRAM_BUS2_BM_MSK (0xF000)
#define SDRAM_BUS2_BM_OFST (15)

#define SDRAM_INIT(base_addr) (SDRAM00_TYPE (base_addr  + 0x1C ))
#define SDRAM_INIT_SR_MSK (0x200)
#define SDRAM_INIT_SR_OFST (9)
#define SDRAM_INIT_BS_MSK (0x400)
#define SDRAM_INIT_BS_OFST (10)
#define SDRAM_INIT_RF_MSK (0x800)
#define SDRAM_INIT_RF_OFST (11)
#define SDRAM_INIT_LEM_MSK (0x1000)
#define SDRAM_INIT_LEM_OFST (12)
#define SDRAM_INIT_LM_MSK (0x2000)
#define SDRAM_INIT_LM_OFST (13)
#define SDRAM_INIT_PR_MSK (0x4000)
#define SDRAM_INIT_PR_OFST (14)
#define SDRAM_INIT_EN_MSK (0x8000)
#define SDRAM_INIT_EN_OFST (15)

#define SDRAM_MODE0(base_addr) (SDRAM00_TYPE (base_addr  + 0x20 ))
#define SDRAM_MODE0_VAL_MSK (0xFFF)
#define SDRAM_MODE0_VAL_OFST (0)

#define SDRAM_MODE1(base_addr) (SDRAM00_TYPE (base_addr  + 0x24 ))
#define SDRAM_MODE1_VAL_MSK (0xFFF)
#define SDRAM_MODE1_VAL_OFST (0)

#endif /* __SDRAM00_H */
