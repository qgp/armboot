#ifndef __IO_CTRL00_H
#define __IO_CTRL00_H

/*
 * Register definitions for the I/O Control
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

#define IOCR_SDRAM(base_addr) (IO_CTRL00_TYPE (base_addr  + 0x0 ))
#define IOCR_SDRAM_LK_MSK (0x1)
#define IOCR_SDRAM_LK_OFST (0)
#define IOCR_SDRAM_IO_MSK (0x2)
#define IOCR_SDRAM_IO_OFST (1)
#define IOCR_SDRAM_OC_MSK (0x1C)
#define IOCR_SDRAM_OC_OFST (2)
#define IOCR_SDRAM_IC_MSK (0x60)
#define IOCR_SDRAM_IC_OFST (5)

#define IOCR_EBI(base_addr) (IO_CTRL00_TYPE (base_addr  + 0x4 ))
#define IOCR_EBI_LK_MSK (0x1)
#define IOCR_EBI_LK_OFST (0)
#define IOCR_EBI_IO_MSK (0x2)
#define IOCR_EBI_IO_OFST (1)
#define IOCR_EBI_OC_MSK (0x1C)
#define IOCR_EBI_OC_OFST (2)
#define IOCR_EBI_IC_MSK (0x60)
#define IOCR_EBI_IC_OFST (5)

#define IOCR_UART(base_addr) (IO_CTRL00_TYPE (base_addr  + 0x8 ))
#define IOCR_UART_LK_MSK (0x1)
#define IOCR_UART_LK_OFST (0)
#define IOCR_UART_IO_MSK (0x2)
#define IOCR_UART_IO_OFST (1)
#define IOCR_UART_OC_MSK (0x1C)
#define IOCR_UART_OC_OFST (2)
#define IOCR_UART_IC_MSK (0x60)
#define IOCR_UART_IC_OFST (5)

#define IOCR_TRACE(base_addr) (IO_CTRL00_TYPE (base_addr  + 0xC ))
#define IOCR_TRACE_LK_MSK (0x1)
#define IOCR_TRACE_LK_OFST (0)
#define IOCR_TRACE_IO_MSK (0x2)
#define IOCR_TRACE_IO_OFST (1)
#define IOCR_TRACE_OC_MSK (0x1C)
#define IOCR_TRACE_OC_OFST (2)
#define IOCR_TRACE_IC_MSK (0x60)
#define IOCR_TRACE_IC_OFST (5)

#define SDRAM_WIDTH(base_addr) (IO_CTRL00_TYPE (base_addr  + 0x7C ))
#define SDRAM_WIDTH_LK_MSK (0x1)
#define SDRAM_WIDTH_LK_OFST (0)
#define SDRAM_WIDTH_W_MSK (0x2)
#define SDRAM_WIDTH_W_OFST (1)

#endif /* __IO_CTRL00_H */





























