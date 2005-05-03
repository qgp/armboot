#ifndef __WATCHDOG_00_H
#define __WATCHDOG_00_H

/*
 * Register definitions for the watchdog
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

#define WDOG_CR(base_addr) (WATCHDOG00_TYPE (base_addr  ))
#define WDOG_CR_LK_MSK (0x1)
#define WDOG_CR_LK_OFST (0)
#define WDOG_CR_LK_ENABLE (0x1)
#define WDOG_CR_TRIGGER_MSK (0x3FFFFFF0)
#define WDOG_CR_TRIGGER_OFST (4)

#define WDOG_COUNT(base_addr) (WATCHDOG00_TYPE (base_addr  + 0x4 ))
#define WDOG_COUNT_MSK (0x3FFFFFFF)

#define WDOG_RELOAD(base_addr) (WATCHDOG00_TYPE (base_addr  + 0x8 )) 
#define WDOG_RELOAD_MAGIC_1 (0xA5A5A5A5)
#define WDOG_RELOAD_MAGIC_2 (0x5A5A5A5A)

#endif /* __WATCHDOG_00_H */
