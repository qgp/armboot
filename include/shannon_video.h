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
** Definitions for the Shannon video interface:
*/

/*
** This driver does not initialize the screen interface, but rather relies
** on the Inferno firmware having done this in a known fashion:
**
** - Resolution: 640x480 pixels
** - Eight bits per pixel
** - Color palette memory at 0xd83a4d00
** - Frame buffer memory at 0xd83a4d00+0x200
**
*/
#define LCD_LUT_ADDR            (0xd83a4d00)        /* address of color map */
#define LCD_VIDEO_ADDR          (LCD_LUT_ADDR+512)  /* address of frame buffer */
#define LCD_VIDEO_SIZE          ((640*480) / 4)
#define LCD_VIDEO_COLS          640                 /* # of colomns (pixels) */
#define LCD_VIDEO_ROWS          480                 /* # of lines (pixels) */
#define LCD_VIDEO_BPP			8					/* # of bits per pixel */
#define LCD_VIDEO_FORMAT		0					/* see cfb_console.c GDF__8BIT_INDEX */

/*
** Entries in the color lookup table are 16-bits wide, where
** four bits are used for each color component:
**
** 15  12    8    4    0
** +----+----+----+----+
** |xxxx| r  | g  | b  |
** +----+----+----+----+
**
*/

typedef unsigned short video_lut_entry;

#define COL_MASK	0xf	/* four bits per color */
#define R_SHIFT		8
#define G_SHIFT		4
#define B_SHIFT		0

#define R_MASK	(COL_MASK<<R_SHIFT)
#define G_MASK	(COL_MASK<<G_SHIFT)
#define B_MASK	(COL_MASK<<B_SHIFT)

#define RGB_MASK	(R_MASK|G_MASK|B_MASK)


