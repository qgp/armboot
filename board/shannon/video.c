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

	/* set the entry, leave for MS bits intact */
	entry = (video_lut_entry*)LCD_LUT_ADDR + index;
	*entry = (*entry & ~RGB_MASK) | (r << R_SHIFT) | (g << G_SHIFT) | (b << B_SHIFT);
}
#endif


/*
************************************************************
** Initialize video hardware
************************************************************
*/

/*
** Actually, we don't initialize hardware here -- We rely on
** the Inferno firmware having done this already. All we do
** here is to set the color lookup table such that we get
** sane colors for black and white, and we clear the screen.
*/

#ifdef CONFIG_CFB_CONSOLE

/*
** This is the init function required for the cfb_console driver
*/
void *video_hw_init(void)
{
	int   cnt;
	unsigned long *p = (unsigned long*)LCD_VIDEO_ADDR;

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

