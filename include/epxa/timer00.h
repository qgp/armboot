#ifndef __TIMER00_H
#define __TIMER00_H

/*
 * Register definitions for the timers
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

#define TIMER0_CR(base_addr) (TIMER00_TYPE (base_addr  + 0x00 ))
#define TIMER0_CR_B_MSK (0x20)
#define TIMER0_CR_B_OFST (0x5)
#define TIMER0_CR_S_MSK  (0x10)
#define TIMER0_CR_S_OFST (0x4)
#define TIMER0_CR_CI_MSK (0x08)
#define TIMER0_CR_CI_OFST (0x3)
#define TIMER0_CR_IE_MSK (0x04)
#define TIMER0_CR_IE_OFST (0x2)
#define TIMER0_CR_MODE_MSK (0x3)
#define TIMER0_CR_MODE_OFST (0)
#define TIMER0_CR_MODE_FREE (0)
#define TIMER0_CR_MODE_ONE  (1)
#define TIMER0_CR_MODE_INTVL (2)

#define TIMER0_SR(base_addr) (TIMER00_TYPE (base_addr  + 0x00 ))
#define TIMER0_SR_B_MSK (0x20)
#define TIMER0_SR_B_OFST (0x5)
#define TIMER0_SR_S_MSK  (0x10)
#define TIMER0_SR_S_OFST (0x4)
#define TIMER0_SR_IS_MSK (0x08)
#define TIMER0_SR_IS_OFST (0x3)
#define TIMER0_SR_IE_MSK (0x04)
#define TIMER0_SR_IE_OFST (0x2)
#define TIMER0_SR_MODE_MSK (0x3)
#define TIMER0_SR_MODE_OFST (0)
#define TIMER0_SR_MODE_FREE (0)
#define TIMER0_SR_MODE_ONE  (1)
#define TIMER0_SR_MODE_INTVL (2)

#define TIMER0_PRESCALE(base_addr) (TIMER00_TYPE (base_addr  + 0x010 ))
#define TIMER0_LIMIT(base_addr) (TIMER00_TYPE (base_addr  + 0x020 ))
#define TIMER0_READ(base_addr) (TIMER00_TYPE (base_addr  + 0x030 ))

#define TIMER1_CR(base_addr) (TIMER00_TYPE (base_addr  + 0x40 ))
#define TIMER1_CR_B_MSK (0x20)
#define TIMER1_CR_B_OFST (0x5)
#define TIMER1_CR_S_MSK  (0x10)
#define TIMER1_CR_S_OFST (0x4)
#define TIMER1_CR_CI_MSK (0x08)
#define TIMER1_CR_CI_OFST (0x3)
#define TIMER1_CR_IE_MSK (0x04)
#define TIMER1_CR_IE_OFST (0x2)
#define TIMER1_CR_MODE_MSK (0x3)
#define TIMER1_CR_MODE_OFST (0)
#define TIMER1_CR_MODE_FREE (0)
#define TIMER1_CR_MODE_ONE  (1)
#define TIMER1_CR_MODE_INTVL (2)

#define TIMER1_SR(base_addr) (TIMER00_TYPE (base_addr  + 0x40 ))
#define TIMER1_SR_B_MSK (0x20)
#define TIMER1_SR_B_OFST (0x5)
#define TIMER1_SR_S_MSK  (0x10)
#define TIMER1_SR_S_OFST (0x4)
#define TIMER1_SR_IS_MSK (0x08)
#define TIMER1_SR_IS_OFST (0x3)
#define TIMER1_SR_IE_MSK (0x04)
#define TIMER1_SR_IE_OFST (0x2)
#define TIMER1_SR_MODE_MSK (0x3)
#define TIMER1_SR_MODE_OFST (0)
#define TIMER1_SR_MODE_FREE (0)
#define TIMER1_SR_MODE_ONE  (1)
#define TIMER1_SR_MODE_INTVL (2)

#define TIMER1_PRESCALE(base_addr) (TIMER00_TYPE (base_addr  + 0x050 ))
#define TIMER1_LIMIT(base_addr) (TIMER00_TYPE (base_addr  + 0x060 ))
#define TIMER1_READ(base_addr) (TIMER00_TYPE (base_addr  + 0x070 ))

#endif /* __TIMER00_H */
