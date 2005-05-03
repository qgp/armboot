#ifndef __MODE_CTRL00_H
#define __MODE_CTRL00_H

/*
 * Register definitions for the reset and mode control
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

#define BOOT_CR(base_addr) (MODE_CTRL00_TYPE (base_addr  ))
#define BOOT_CR_BF_MSK (0x1)
#define BOOT_CR_BF_OFST (0)
#define BOOT_CR_HM_MSK (0x2)
#define BOOT_CR_HM_OFST (1)
#define BOOT_CR_RE_MSK (0x4)
#define BOOT_CR_RE_OFST (2)

#define RESET_SR(base_addr) (MODE_CTRL00_TYPE (base_addr  + 0x4 ))
#define RESET_SR_WR_MSK (0x1)
#define RESET_SR_WR_OFST (0)
#define RESET_SR_CR_MSK (0x2)
#define RESET_SR_CR_OFST (1)
#define RESET_SR_JT_MSK (0x4)
#define RESET_SR_JT_OFST (2)
#define RESET_SR_ER_MSK (0x8)
#define RESET_SR_ER_OFST (3)

#define ID_CODE(base_addr) (MODE_CTRL00_TYPE (base_addr  + 0x08 ))

#define SRAM0_SR(base_addr) (MODE_CTRL00_TYPE (base_addr  + 0x20 ))
#define SRAM0_SR_SIZE_MSK (0xFFFFF000)
#define SRAM0_SR_SIZE_OFST (12)

#define SRAM1_SR(base_addr) (MODE_CTRL00_TYPE (base_addr  + 0x24 ))
#define SRAM1_SR_SIZE_MSK (0xFFFFF000)
#define SRAM1_SR_SIZE_OFST (12)

#define DPSRAM0_SR(base_addr) (MODE_CTRL00_TYPE (base_addr  + 0x30 ))

#define DPSRAM0_SR_MODE_MSK (0xF)
#define DPSRAM0_SR_MODE_OFST (0)
#define DPSRAM0_SR_GLBL_MSK (0x30)
#define DPSRAM0_SR_SIZE_MSK (0xFFFFF000)
#define DPSRAM0_SR_SIZE_OFST (12)

#define DPSRAM0_LCR(base_addr) (MODE_CTRL00_TYPE (base_addr  + 0x34 ))
#define DPSRAM0_LCR_LCKADDR_MSK (0x1FFE0)
#define DPSRAM0_LCR_LCKADDR_OFST (4)

#define DPSRAM1_SR(base_addr) (MODE_CTRL00_TYPE (base_addr  + 0x38 ))
#define DPSRAM1_SR_MODE_MSK (0xF)
#define DPSRAM1_SR_MODE_OFST (0)
#define DPSRAM1_SR_GLBL_MSK (0x30)
#define DPSRAM1_SR_GLBL_OFST (4)
#define DPSRAM1_SR_SIZE_MSK (0xFFFFF000)
#define DPSRAM1_SR_SIZE_OFST (12)

#define DPSRAM1_LCR(base_addr) (MODE_CTRL00_TYPE (base_addr  + 0x3C ))
#define DPSRAM1_LCR_LCKADDR_MSK (0x1FFE0)
#define DPSRAM1_LCR_LCKADDR_OFST (4)

#endif /* __MODE_CTRL00_H */
