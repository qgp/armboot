/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Robert Kaiser <rkaiser@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
** Shannon specific support routines for framebuffer driver
*/

#include "config.h"
#include <shannon_video.h>
#include <SA-1100.h>
#include <armboot.h>

#if defined(CONFIG_CFB_CONSOLE) || defined(CONFIG_FRAMEBUFFER)

#define SHANNON_UCB_GPIO_BACKLIGHT  9
#define SHANNON_UCB_GPIO_BRIGHT_MASK    7
#define SHANNON_UCB_GPIO_BRIGHT     6
#define SHANNON_UCB_GPIO_CONTRAST_MASK  0x3f
#define SHANNON_UCB_GPIO_CONTRAST   0

#define UCB_IO_DATA 0x00
#define UCB_IO_DIR  0x01


static inline void ucb1x00_enable(void)
{
    Ser4MCSR = -1;
    Ser4MCCR0 |= MCCR0_MCE;
}


static inline void ucb1x00_disable(void)
{
    Ser4MCCR0 &= ~MCCR0_MCE;
}

static inline void ucb1x00_reg_write(unsigned int reg, unsigned int val)
{
    int i;

    Ser4MCDR2 = reg << 17 | MCDR2_Wr | (val & 0xffff);

    for (i = 0; i < 2; i++) {
        udelay(10000);
        if (Ser4MCSR & MCSR_CWC) {
            break;
        }
    }
}

void ucb1x00_io_write(unsigned int set, unsigned int clear)
{
	static unsigned short io_out = 0;

    io_out |= set;
    io_out &= ~clear;

    ucb1x00_reg_write(UCB_IO_DATA, io_out);
}

void ucb1x00_io_set_dir(unsigned int in, unsigned int out)
{
	static unsigned short io_dir = 0;

    io_dir |= out;
    io_dir &= ~in;

    ucb1x00_reg_write(UCB_IO_DIR, io_dir);
}

int lcd_contrast(int arg)
{
	arg &= SHANNON_UCB_GPIO_CONTRAST_MASK;
    ucb1x00_enable();
    ucb1x00_io_write(arg << SHANNON_UCB_GPIO_CONTRAST,
        (~arg & SHANNON_UCB_GPIO_CONTRAST_MASK) << SHANNON_UCB_GPIO_CONTRAST);
    ucb1x00_disable();
    return 0;
}


int lcd_brightness(int arg)
{
	arg &= SHANNON_UCB_GPIO_BRIGHT_MASK;
    arg = SHANNON_UCB_GPIO_BRIGHT_MASK - arg;
    ucb1x00_enable();
    ucb1x00_io_write(arg << SHANNON_UCB_GPIO_BRIGHT,
        (~arg & SHANNON_UCB_GPIO_BRIGHT_MASK) << SHANNON_UCB_GPIO_BRIGHT);
    ucb1x00_disable();
    return 0;
}

#if defined(CONFIG_INIT_CRITICAL) && (defined(CONFIG_CFB_CONSOLE) || defined(CONFIG_FRAMEBUFFER))

static int lcd_power(int arg)
{
	arg = !!arg;
    arg = 1 - arg;
    ucb1x00_enable();
    ucb1x00_io_write(arg << SHANNON_UCB_GPIO_BACKLIGHT,
        (~arg & 1) << SHANNON_UCB_GPIO_BACKLIGHT);
    ucb1x00_disable();
    return 0;
}

static void lcd_backlight_init(void)
{
    unsigned long mask;

    /*
     * Setup the PPC unit correctly.
     */
    PPDR &= ~PPC_RXD4;
    PPDR |= PPC_TXD4 | PPC_SCLK | PPC_SFRM;
    PSDR |= PPC_RXD4;
    PSDR &= ~(PPC_TXD4 | PPC_SCLK | PPC_SFRM);
    PPSR &= ~(PPC_TXD4 | PPC_SCLK | PPC_SFRM);
    Ser4MCSR = -1;
    Ser4MCCR1 = 0;
    Ser4MCCR0 = 0x00047f7f;

    ucb1x00_enable();
    mask = (SHANNON_UCB_GPIO_CONTRAST_MASK << SHANNON_UCB_GPIO_CONTRAST) |
           (SHANNON_UCB_GPIO_BRIGHT_MASK << SHANNON_UCB_GPIO_BRIGHT) |
           (1 << SHANNON_UCB_GPIO_BACKLIGHT);
    ucb1x00_io_set_dir(0, mask);
    ucb1x00_disable();
}
#endif

#endif

/*
************************************************************
** Initialize video hardware
************************************************************
*/

/*
** This is only used if we run stand-alone (i.e. #ifdef CONFIG_INIT_CRITICAL).
** Otherwise we rely on the Inferno firmware having done this already.
*/

#if defined(CONFIG_INIT_CRITICAL) && (defined(CONFIG_CFB_CONSOLE) || defined(CONFIG_FRAMEBUFFER))
static void video_init_crit(void)
{
	/* lookup table to tell pixel clock setting from CPU clock */
	static int pixclk[0x20] = 
	{
		5,	/* 59 MHz */
		7,	/* 73 MHz */
		7,	/* 88 MHz */
		8,	/* 103 MHz */
		9,	/* 118 MHz */
		10,	/* 132 MHz */
		11,	/* 147 MHz */
		13,	/* 162 MHz */
		14,	/* 176 MHz */
		15,	/* 191 MHz */
		16	/* 206 MHz */
	};
	int cpuclk;

	/* disable LCD */
	LCCR0 = 0x0001003c;

	/* assign LCD GPIO pins */
	GPDR = 0x004403fc;
	GAFR = 0x000003fc;
	GPSR = 0x00440000;

	/* set up a mininimal, "inferno-compliant" color palette */
	((video_lut_entry*)LCD_LUT_ADDR)[0x00] = 0x1000;
	((video_lut_entry*)LCD_LUT_ADDR)[0xc9] = 0x1fff;

	/* read CPU clock, set pixel clock divider accordingly */
	cpuclk = PPCR & 0x1f;
	if(cpuclk >= sizeof(pixclk)/sizeof(pixclk[0]))
		cpuclk = sizeof(pixclk)/sizeof(pixclk[0]) - 1;
	/* set up other LCD control registers */
	LCCR3 = pixclk[cpuclk];
	LCCR2 = 0x010000ef;
	LCCR1 = 0x01010a70;

	/* set DMA base address registers */
	LCCR0 = 0x0001003c;
	DBAR1 = (Address)LCD_LUT_ADDR;
	DBAR2 = (Address)(LCD_VIDEO_ADDR+LCD_VIDEO_COLS*LCD_VIDEO_ROWS/2);
	LCCR0 = 0x0001003d;

	lcd_backlight_init();
	lcd_power(1);
	lcd_brightness(7);
	lcd_contrast(0x1f);
}
#endif

/*
************************************************************
** Access the color lookup table
************************************************************
*/

#if defined (CONFIG_CFB_CONSOLE) || defined(CONFIG_FRAMEBUFFER_CLUT)
void video_set_lut(
	unsigned int index,     /* color number */
	unsigned char r,        /* red */
	unsigned char g,        /* green */
	unsigned char b)        /* blue */
{
	video_lut_entry *entry;

	/*
	** color values are passed as 8-bit values, scale them
	** down to 4-bit
	*/
	r >>= 4;
	g >>= 4;
	b >>= 4;

	r &= COL_MASK;
	g &= COL_MASK;
	b &= COL_MASK;

	/* set the entry, leave four MS bits intact */
	entry = (video_lut_entry*)LCD_LUT_ADDR + index;
	*entry = (*entry & ~RGB_MASK) |
 (r << R_SHIFT) | (g << G_SHIFT) | (b << B_SHIFT);
}
#endif


/*
************************************************************
** Framebuffer driver init function
************************************************************
*/

#ifdef CONFIG_CFB_CONSOLE

/*
** This is the init function required for the cfb_console driver
*/
void *video_hw_init(void)
{
	int   cnt;
	unsigned long *p = (unsigned long*)LCD_VIDEO_ADDR;

#ifdef CONFIG_INIT_CRITICAL
	video_init_crit();
#endif

	/* set lookup table for black and white */
	video_set_lut(0, 0, 0, 0);
	video_set_lut(1, 0xff, 0xff, 0xff);

	/* clear the framebuffer memory */
	for(cnt = 0; cnt < LCD_VIDEO_SIZE; cnt++)
		*p++ = 0;

	return (void*)LCD_VIDEO_ADDR;
}
#endif	/* CONFIG_CFB_CONSOLE */


#ifdef CONFIG_FRAMEBUFFER

/*
** This is the init function required for the framebuffer driver
*/
void *video_framebuffer_init(
	unsigned int *ax,
	unsigned int *ay,
	unsigned int *pitch,
	unsigned int *fcol,
	unsigned int *bcol)
{
#ifdef CONFIG_INIT_CRITICAL
	video_init_crit();
#endif
	/*
	** just deliver the required infomation to the caller
	*/
	*ax    = LCD_VIDEO_COLS;
	*ay    = LCD_VIDEO_ROWS;

	*pitch = LCD_VIDEO_COLS;/* address offset between vertically adjacent pixels */
	*fcol  = 0x00;			/* these colors are also used by the Inferno firmware */
	*bcol  = 0xc9;
	return (void*)LCD_VIDEO_ADDR;
}
#endif	/* CONFIG_FRAMEBUFFER */

