/*
 *  
 *  This file contains the register definitions for the Excalibur
 *  Interrupt controller INT_CTRL00.
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

#ifndef __INT_CTRL00_H
#define __INT_CTRL00_H

#define INT_MS(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x00 ))
#define INT_MS_FC_MSK (0x10000)
#define INT_MS_FC_OFST (16)
#define INT_MS_CR_MSK (0x8000)
#define INT_MS_CR_OFST (15)
#define INT_MS_CT_MSK (0x4000)
#define INT_MS_CT_OFST (14)
#define INT_MS_AE_MSK (0x2000)
#define INT_MS_AE_OFST (13)
#define INT_MS_PE_MSK (0x1000)
#define INT_MS_PE_OFST (12)
#define INT_MS_EE_MSK (0x0800)
#define INT_MS_EE_OFST (11)
#define INT_MS_PS_MSK (0x0400)
#define INT_MS_PS_OFST (10)
#define INT_MS_T1_MSK (0x0200)
#define INT_MS_T1_OFST (9)
#define INT_MS_T0_MSK (0x0100)
#define INT_MS_T0_OFST (8)
#define INT_MS_UA_MSK (0x0080)
#define INT_MS_UA_OFST (7)
#define INT_MS_IP_MSK (0x0040)
#define INT_MS_IP_OFST (6)
#define INT_MS_P5_MSK (0x0020)
#define INT_MS_P5_OFST (5)
#define INT_MS_P4_MSK (0x0010)
#define INT_MS_P4_OFST (4)
#define INT_MS_P3_MSK (0x0008)
#define INT_MS_P3_OFST (3)
#define INT_MS_P2_MSK (0x0004)
#define INT_MS_P2_OFST (2)
#define INT_MS_P1_MSK (0x0002)
#define INT_MS_P1_OFST (1)
#define INT_MS_P0_MSK (0x0001)
#define INT_MS_P0_OFST (0)

#define INT_MC(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x04 ))
#define INT_MC_FC_MSK (0x10000)
#define INT_MC_FC_OFST (16)
#define INT_MC_CR_MSK (0x8000)
#define INT_MC_CR_OFST (15)
#define INT_MC_CT_MSK (0x4000)
#define INT_MC_CT_OFST (14)
#define INT_MC_AE_MSK (0x2000)
#define INT_MC_AE_OFST (13)
#define INT_MC_PE_MSK (0x1000)
#define INT_MC_PE_OFST (12)
#define INT_MC_EE_MSK (0x0800)
#define INT_MC_EE_OFST (11)
#define INT_MC_PS_MSK (0x0400)
#define INT_MC_PS_OFST (10)
#define INT_MC_T1_MSK (0x0200)
#define INT_MC_T1_OFST (9)
#define INT_MC_T0_MSK (0x0100)
#define INT_MC_T0_OFST (8)
#define INT_MC_UA_MSK (0x0080)
#define INT_MC_UA_OFST (7)
#define INT_MC_IP_MSK (0x0040)
#define INT_MC_IP_OFST (6)
#define INT_MC_P5_MSK (0x0020)
#define INT_MC_P5_OFST (5)
#define INT_MC_P4_MSK (0x0010)
#define INT_MC_P4_OFST (4)
#define INT_MC_P3_MSK (0x0008)
#define INT_MC_P3_OFST (3)
#define INT_MC_P2_MSK (0x0004)
#define INT_MC_P2_OFST (2)
#define INT_MC_P1_MSK (0x0002)
#define INT_MC_P1_OFST (1)
#define INT_MC_P0_MSK (0x0001)
#define INT_MC_P0_OFST (0)

#define INT_SS(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x08 ))
#define INT_SS_FC_MSK (0x8000)
#define INT_SS_FC_OFST (15)
#define INT_SS_CR_MSK (0x8000)
#define INT_SS_CR_OFST (15)
#define INT_SS_CT_MSK (0x4000)
#define INT_SS_CT_OFST (14)
#define INT_SS_AE_MSK (0x2000)
#define INT_SS_AE_OFST (13)
#define INT_SS_PE_MSK (0x1000)
#define INT_SS_PE_OFST (12)
#define INT_SS_EE_MSK (0x0800)
#define INT_SS_EE_OFST (11)
#define INT_SS_PS_MSK (0x0400)
#define INT_SS_PS_OFST (10)
#define INT_SS_T1_MSK (0x0200)
#define INT_SS_T1_OFST (9)
#define INT_SS_T0_MSK (0x0100)
#define INT_SS_T0_OFST (8)
#define INT_SS_UA_MSK (0x0080)
#define INT_SS_UA_OFST (7)
#define INT_SS_IP_MSK (0x0040)
#define INT_SS_IP_OFST (6)
#define INT_SS_P5_MSK (0x0020)
#define INT_SS_P5_OFST (5)
#define INT_SS_P4_MSK (0x0010)
#define INT_SS_P4_OFST (4)
#define INT_SS_P3_MSK (0x0008)
#define INT_SS_P3_OFST (3)
#define INT_SS_P2_MSK (0x0004)
#define INT_SS_P2_OFST (2)
#define INT_SS_P1_MSK (0x0002)
#define INT_SS_P1_OFST (1)
#define INT_SS_P0_MSK (0x0001)
#define INT_SS_P0_OFST (0)

#define INT_RS(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x0C ))
#define INT_RS_FC_MSK (0x10000)
#define INT_RS_FC_OFST (16)
#define INT_RS_CR_MSK (0x8000)
#define INT_RS_CR_OFST (15)
#define INT_RS_CT_MSK (0x4000)
#define INT_RS_CT_OFST (14)
#define INT_RS_AE_MSK (0x2000)
#define INT_RS_AE_OFST (13)
#define INT_RS_PE_MSK (0x1000)
#define INT_RS_PE_OFST (12)
#define INT_RS_EE_MSK (0x0800)
#define INT_RS_EE_OFST (11)
#define INT_RS_PS_MSK (0x0400)
#define INT_RS_PS_OFST (10)
#define INT_RS_T1_MSK (0x0200)
#define INT_RS_T1_OFST (9)
#define INT_RS_T0_MSK (0x0100)
#define INT_RS_T0_OFST (8)
#define INT_RS_UA_MSK (0x0080)
#define INT_RS_UA_OFST (7)
#define INT_RS_IP_MSK (0x0040)
#define INT_RS_IP_OFST (6)
#define INT_RS_P5_MSK (0x0020)
#define INT_RS_P5_OFST (5)
#define INT_RS_P4_MSK (0x0010)
#define INT_RS_P4_OFST (4)
#define INT_RS_P3_MSK (0x0008)
#define INT_RS_P3_OFST (3)
#define INT_RS_P2_MSK (0x0004)
#define INT_RS_P2_OFST (2)
#define INT_RS_P1_MSK (0x0002)
#define INT_RS_P1_OFST (1)
#define INT_RS_P0_MSK (0x0001)
#define INT_RS_P0_OFST (0)

#define INT_ID(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x10 ))
#define INT_ID_ID_MSK (0x3F)
#define INT_ID_ID_OFST (0)

#define INT_PLD_PRIORITY(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x14 ))
#define INT_PLD_PRIORITY_PRI_MSK (0x3F)
#define INT_PLD_PRIORITY_PRI_OFST (0)
#define INT_PLD_PRIORITY_GA_MSK (0x40)
#define INT_PLD_PRIORITY_GA_OFST (6)

#define INT_PLD_MODE(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x18 ))
#define INT_PLD_MODE_MODE_MSK (0x3)
#define INT_PLD_MODE_MODE_OFST (0)

#define INT_PRIORITY_P0(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x80 ))
#define INT_PRIORITY_P0_PRI_MSK (0x3F)
#define INT_PRIORITY_P0_PRI_OFST (0)
#define INT_PRIORITY_P0_FQ_MSK (0x40)
#define INT_PRIORITY_P0_FQ_OFST (6)

#define INT_PRIORITY_P1(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x84 ))
#define INT_PRIORITY_P1_PRI_MSK (0x3F)
#define INT_PRIORITY_P1_PRI_OFST (0)
#define INT_PRIORITY_P1_FQ_MSK (0x40)
#define INT_PRIORITY_P1_FQ_OFST (6)

#define INT_PRIORITY_P2(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x88 ))
#define INT_PRIORITY_P2_PRI_MSK (0x3F)
#define INT_PRIORITY_P2_PRI_OFST (0)
#define INT_PRIORITY_P2_FQ_MSK (0x40)
#define INT_PRIORITY_P2_FQ_OFST (6)

#define INT_PRIORITY_P3(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x8C ))
#define INT_PRIORITY_P3_PRI_MSK (0x3F)
#define INT_PRIORITY_P3_PRI_OFST (0)
#define INT_PRIORITY_P3_FQ_MSK (0x40)
#define INT_PRIORITY_P3_FQ_OFST (6)

#define INT_PRIORITY_P4(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x90 ))
#define INT_PRIORITY_P4_PRI_MSK (0x3F)
#define INT_PRIORITY_P4_PRI_OFST (0)
#define INT_PRIORITY_P4_FQ_MSK (0x40)
#define INT_PRIORITY_P4_FQ_OFST (6)

#define INT_PRIORITY_P5(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x94 ))
#define INT_PRIORITY_P5_PRI_MSK (0x3F)
#define INT_PRIORITY_P5_PRI_OFST (0)
#define INT_PRIORITY_P5_FQ_MSK (0x40)
#define INT_PRIORITY_P5_FQ_OFST (6)

#define INT_PRIORITY_IP(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x98 ))
#define INT_PRIORITY_IP_PRI_MSK (0x3F)
#define INT_PRIORITY_IP_PRI_OFST (0)
#define INT_PRIORITY_IP_FQ_MSK (0x40)
#define INT_PRIORITY_IP_FQ_OFST (6)

#define INT_PRIORITY_UA(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x9C ))
#define INT_PRIORITY_UA_PRI_MSK (0x3F)
#define INT_PRIORITY_UA_PRI_OFST (0)
#define INT_PRIORITY_UA_FQ_MSK (0x40)
#define INT_PRIORITY_UA_FQ_OFST (6)

#define INT_PRIORITY_T0(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xA0 ))
#define INT_PRIORITY_T0_PRI_MSK (0x3F)
#define INT_PRIORITY_T0_PRI_OFST (0)
#define INT_PRIORITY_T0_FQ_MSK (0x40)
#define INT_PRIORITY_T0_FQ_OFST (6)

#define INT_PRIORITY_T1(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xA4 ))
#define INT_PRIORITY_T1_PRI_MSK (0x3F)
#define INT_PRIORITY_T1_PRI_OFST (0)
#define INT_PRIORITY_T1_FQ_MSK (0x40)
#define INT_PRIORITY_T1_FQ_OFST (6)

#define INT_PRIORITY_PS(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xA8 ))
#define INT_PRIORITY_PS_PRI_MSK (0x3F)
#define INT_PRIORITY_PS_PRI_OFST (0)
#define INT_PRIORITY_PS_FQ_MSK (0x40)
#define INT_PRIORITY_PS_FQ_OFST (6)

#define INT_PRIORITY_EE(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xAC ))
#define INT_PRIORITY_EE_PRI_MSK (0x3F)
#define INT_PRIORITY_EE_PRI_OFST (0)
#define INT_PRIORITY_EE_FQ_MSK (0x40)
#define INT_PRIORITY_EE_FQ_OFST (6)

#define INT_PRIORITY_PE(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xB0 ))
#define INT_PRIORITY_PE_PRI_MSK (0x3F)
#define INT_PRIORITY_PE_PRI_OFST (0)
#define INT_PRIORITY_PE_FQ_MSK (0x40)
#define INT_PRIORITY_PE_FQ_OFST (6)

#define INT_PRIORITY_AE(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xB4 ))
#define INT_PRIORITY_AE_PRI_MSK (0x3F)
#define INT_PRIORITY_AE_PRI_OFST (0)
#define INT_PRIORITY_AE_FQ_MSK (0x40)
#define INT_PRIORITY_AE_FQ_OFST (6)

#define INT_PRIORITY_CT(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xB8 ))
#define INT_PRIORITY_CT_PRI_MSK (0x3F)
#define INT_PRIORITY_CT_PRI_OFST (0)
#define INT_PRIORITY_CT_FQ_MSK (0x40)
#define INT_PRIORITY_CT_FQ_OFST (6)

#define INT_PRIORITY_CR(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xBC ))
#define INT_PRIORITY_CR_PRI_MSK (0x3F)
#define INT_PRIORITY_CR_PRI_OFST (0)
#define INT_PRIORITY_CR_FQ_MSK (0x40)
#define INT_PRIORITY_CR_FQ_OFST (6)

#define INT_PRIORITY_FC(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xC0 ))
#define INT_PRIORITY_FC_PRI_MSK (0x3F)
#define INT_PRIORITY_FC_PRI_OFST (0)
#define INT_PRIORITY_FC_FQ_MSK (0x40)
#define INT_PRIORITY_FC_FQ_OFST (6)
#define INT_PRIORITY_IP(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x98 ))
#define INT_PRIORITY_IP_PRI_MSK (0x3F)
#define INT_PRIORITY_IP_PRI_OFST (0)
#define INT_PRIORITY_IP_FQ_MSK (0x40)
#define INT_PRIORITY_IP_FQ_OFST (6)

#define INT_PRIORITY_UA(base_addr) (INT_CTRL00_TYPE (base_addr  + 0x9C ))
#define INT_PRIORITY_UA_PRI_MSK (0x3F)
#define INT_PRIORITY_UA_PRI_OFST (0)
#define INT_PRIORITY_UA_FQ_MSK (0x40)
#define INT_PRIORITY_UA_FQ_OFST (6)

#define INT_PRIORITY_T0(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xA0 ))
#define INT_PRIORITY_T0_PRI_MSK (0x3F)
#define INT_PRIORITY_T0_PRI_OFST (0)
#define INT_PRIORITY_T0_FQ_MSK (0x40)
#define INT_PRIORITY_T0_FQ_OFST (6)

#define INT_PRIORITY_T1(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xA4 ))
#define INT_PRIORITY_T1_PRI_MSK (0x3F)
#define INT_PRIORITY_T1_PRI_OFST (0)
#define INT_PRIORITY_T1_FQ_MSK (0x40)
#define INT_PRIORITY_T1_FQ_OFST (6)

#define INT_PRIORITY_PS(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xA8 ))
#define INT_PRIORITY_PS_PRI_MSK (0x3F)
#define INT_PRIORITY_PS_PRI_OFST (0)
#define INT_PRIORITY_PS_FQ_MSK (0x40)
#define INT_PRIORITY_PS_FQ_OFST (6)

#define INT_PRIORITY_EE(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xAC ))
#define INT_PRIORITY_EE_PRI_MSK (0x3F)
#define INT_PRIORITY_EE_PRI_OFST (0)
#define INT_PRIORITY_EE_FQ_MSK (0x40)
#define INT_PRIORITY_EE_FQ_OFST (6)

#define INT_PRIORITY_PE(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xB0 ))
#define INT_PRIORITY_PE_PRI_MSK (0x3F)
#define INT_PRIORITY_PE_PRI_OFST (0)
#define INT_PRIORITY_PE_FQ_MSK (0x40)
#define INT_PRIORITY_PE_FQ_OFST (6)

#define INT_PRIORITY_AE(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xB4 ))
#define INT_PRIORITY_AE_PRI_MSK (0x3F)
#define INT_PRIORITY_AE_PRI_OFST (0)
#define INT_PRIORITY_AE_FQ_MSK (0x40)
#define INT_PRIORITY_AE_FQ_OFST (6)

#define INT_PRIORITY_M0(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xB8 ))
#define INT_PRIORITY_M0_PRI_MSK (0x3F)
#define INT_PRIORITY_M0_PRI_OFST (0)
#define INT_PRIORITY_M0_FQ_MSK (0x40)
#define INT_PRIORITY_M0_FQ_OFST (6)

#define INT_PRIORITY_M1(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xBC ))
#define INT_PRIORITY_M1_PRI_MSK (0x3F)
#define INT_PRIORITY_M1_PRI_OFST (0)
#define INT_PRIORITY_M1_FQ_MSK (0x40)
#define INT_PRIORITY_M1_FQ_OFST (6)

#define INT_PRIORITY_FC(base_addr) (INT_CTRL00_TYPE (base_addr  + 0xC0 ))
#define INT_PRIORITY_FC_PRI_MSK (0x3F)
#define INT_PRIORITY_FC_PRI_OFST (0)
#define INT_PRIORITY_FC_FQ_MSK (0x40)
#define INT_PRIORITY_FC_FQ_OFST (6)

#endif /* __INT_CTRL00_H */


