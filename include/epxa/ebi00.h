#ifndef __EBI00_H
#define __EBI00_H

/*
 * Register definitions for the ExternalBus Interface
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
 
#define EBI_CR(base_addr) (EBI00_TYPE (base_addr  + 0x00 ))
#define EBI_CR_BP_MSK (0x1)
#define EBI_CR_BP_OFST (0)
#define EBI_CR_OP_MSK (0x2)
#define EBI_CR_OP_OFST (1)
#define EBI_CR_WP_MSK (0x4)
#define EBI_CR_WP_OFST (2)
#define EBI_CR_EO_MSK (0x8)
#define EBI_CR_EO_OFST (3)
#define EBI_CR_SP_MSK (0x10)
#define EBI_CR_SP_OFST (4)
#define EBI_CR_CLK_DIV_MSK (0x3C0)
#define EBI_CR_CLK_DIV_OFST (6)
#define EBI_CR_TIMEOUT_MSK (0x3FC00)
#define EBI_CR_TIMEOUT_OFST (10)
#define EBI_CR_TE_MSK (0x40000)
#define EBI_CR_TE_OFST (18)
#define EBI_CR_CE_MSK (0x80000)
#define EBI_CR_CE_OFST (19)
#define EBI_CR_R_MSK (0x80000000)
#define EBI_CR_R_OFST (31)

#define EBI_SR(base_addr) (EBI00_TYPE (base_addr  + 0x00 ))
#define EBI_SR_BP_MSK (0x1)
#define EBI_SR_BP_OFST (0)
#define EBI_SR_OP_MSK (0x2)
#define EBI_SR_OP_OFST (1)
#define EBI_SR_WP_MSK (0x4)
#define EBI_SR_WP_OFST (2)
#define EBI_SR_EO_MSK (0x8)
#define EBI_SR_EO_OFST (3)
#define EBI_SR_SP_MSK (0x10)
#define EBI_SR_SP_OFST (4)
#define EBI_SR_CLK_DIV_MSK (0x3C0)
#define EBI_SR_CLK_DIV_OFST (6)
#define EBI_SR_TIMEOUT_MSK (0x3FC00)
#define EBI_SR_TIMEOUT_OFST (10)
#define EBI_SR_TE_MSK (0x40000)
#define EBI_SR_TE_OFST (18)
#define EBI_SR_CE_MSK (0x80000)
#define EBI_SR_CE_OFST (19)
#define EBI_SR_RE_MSK (0x8000000)
#define EBI_SR_RE_OFST (27)
#define EBI_SR_RF_MSK (0x10000000)
#define EBI_SR_RF_OFST (28)
#define EBI_SR_XE_MSK (0x20000000)
#define EBI_SR_XE_OFST (29)
#define EBI_SR_XF_MSK (0x40000000)
#define EBI_SR_XF_OFST (30)
#define EBI_SR_XP_MSK (0x80000000)
#define EBI_SR_XP_OFST (31)

#define EBI_BLOCK0(base_addr) (EBI00_TYPE (base_addr  + 0x10 ))
#define EBI_BLOCK0_SA_MSK 0x1
#define EBI_BLOCK0_SA_OFST (0)
#define EBI_BLOCK0_WAIT_MSK (0x1E)
#define EBI_BLOCK0_WAIT_OFST (1)
#define EBI_BLOCK0_CP_MSK 0x20
#define EBI_BLOCK0_CP_OFST (5)
#define EBI_BLOCK0_BH_MSK 0x40
#define EBI_BLOCK0_BH_OFST (6)
#define EBI_BLOCK0_BE_MSK 0x80
#define EBI_BLOCK0_BE_OFST (7)

#define EBI_BLOCK1(base_addr) (EBI00_TYPE (base_addr  + 0x14 ))
#define EBI_BLOCK1_SA_MSK 0x1
#define EBI_BLOCK1_SA_OFST (0)
#define EBI_BLOCK1_WAIT_MSK (0x1E)
#define EBI_BLOCK1_WAIT_OFST (1)
#define EBI_BLOCK1_CP_MSK 0x20
#define EBI_BLOCK1_CP_OFST (5)
#define EBI_BLOCK1_BH_MSK 0x40
#define EBI_BLOCK1_BH_OFST (6)
#define EBI_BLOCK1_BE_MSK 0x80
#define EBI_BLOCK1_BE_OFST (7)

#define EBI_BLOCK2(base_addr) (EBI00_TYPE (base_addr  + 0x18 ))
#define EBI_BLOCK2_SA_MSK 0x1
#define EBI_BLOCK2_SA_OFST (0)
#define EBI_BLOCK2_WAIT_MSK (0x1E)
#define EBI_BLOCK2_WAIT_OFST (1)
#define EBI_BLOCK2_CP_MSK 0x20
#define EBI_BLOCK2_CP_OFST (5)
#define EBI_BLOCK2_BH_MSK 0x40
#define EBI_BLOCK2_BH_OFST (6)
#define EBI_BLOCK2_BE_MSK 0x80
#define EBI_BLOCK2_BE_OFST (7)

#define EBI_BLOCK3(base_addr) (EBI00_TYPE (base_addr  + 0x1C ))
#define EBI_BLOCK3_SA_MSK 0x1
#define EBI_BLOCK3_SA_OFST (0)
#define EBI_BLOCK3_WAIT_MSK (0x1E)
#define EBI_BLOCK3_WAIT_OFST (1)
#define EBI_BLOCK3_CP_MSK 0x20
#define EBI_BLOCK3_CP_OFST (5)
#define EBI_BLOCK3_BH_MSK 0x40
#define EBI_BLOCK3_BH_OFST (6)
#define EBI_BLOCK3_BE_MSK 0x80
#define EBI_BLOCK3_BE_OFST (7)

#define EBI_INT_SR(base_addr) (EBI00_TYPE (base_addr  + 0x20 ))
#define EBI_INT_SR_TOI (0x1)
#define EBI_INT_SR_OFST (0)

#define EBI_INT_ADDRSR(base_addr) (EBI00_TYPE (base_addr  + 0x24 ))
#define EBI_INT_ADDRESS_MSK (0xFFFFFF)
#define EBI_INT_ADDRESS_OFST (0)
#define EBI_INT_B0_MSK (0x10000000)
#define EBI_INT_B0_OFST (28)
#define EBI_INT_B1_MSK (0x20000000)
#define EBI_INT_B1_OFST (29)
#define EBI_INT_B2_MSK (0x40000000)
#define EBI_INT_B2_OFST (30)
#define EBI_INT_B3_MSK (0x80000000)
#define EBI_INT_B3_OFST (31)

#endif /* __EBI00_H */
