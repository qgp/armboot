/*
 * (C) Copyright 2002 ELTEC Elektronik AG
 * Frank Gottschling <fgottschling@eltec.de>
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
 * cfb_console.c
 *
 * Color Framebuffer Console driver for 8/15/16/24/32 bits per pixel.
 *
 * At the moment only the 8x16 font is tested and the font fore- and
 * background color is limited to black/white/gray colors. The Linux
 * logo can be placed in the upper left corner and additional board
 * information strings (that normaly goes to serial port) can be drawed.
 *
 * The console driver can use the standard PC keyboard interface (i8042)
 * for character input. Character output goes to a memory mapped video
 * framebuffer with little or big-endian organisation.
 * With environment setting 'console=serial' the console i/o can be
 * forced to serial port.

 The driver uses graphic specific defines/parameters/functions:

 (for SMI LynxE graphic chip)

 CONFIG_VIDEO_SMI_LYNXEM - use graphic driver for SMI 710,712,810
 VIDEO_FB_LITTLE_ENDIAN  - framebuffer organisation default: big endian
 VIDEO_HW_RECTFILL       - graphic driver supports hardware rectangle fill
 VIDEO_HW_BITBLT         - graphic driver supports hardware bit blt

 Console Parameters are set by graphic drivers global struct:

 VIDEO_VISIBLE_COLS          - x resolution
 VIDEO_VISIBLE_ROWS          - y resolution
 VIDEO_PIXEL_SIZE            - storage size in byte per pixel
 VIDEO_DATA_FORMAT           - graphical data format GDF
 VIDEO_FB_ADRS               - start of video memory

 CONFIG_I8042_KBD            - AT Keyboard driver for i8042
 VIDEO_KBD_INIT_FCT          - init function for keyboard
 VIDEO_TSTC_FCT              - keyboard_tstc function
 VIDEO_GETC_FCT              - keyboard_getc function

 CONFIG_CONSOLE_CURSOR_BLINK - on/off drawing cursor is done with delay
                               loop in VIDEO_TSTC_FCT (i8042)
 CFG_CONSOLE_BLINK_COUNT     - value for delay loop - blink rate
 CONFIG_CONSOLE_TIME         - display time/date in upper right corner,
                               needs CFG_CMD_DATE and CONFIG_CONSOLE_CURSOR_BLINK
 CONFIG_VIDEO_LOGO           - display Linux Logo in upper left corner
 CONFIG_CONSOLE_EXTRA_INFO   - display additional board information strings
                               that normaly goes to serial port. This define
                               requires a board specific function:
                               video_drawstring (VIDEO_INFO_X,
                                                 VIDEO_INFO_Y + i*VIDEO_FONT_HEIGHT,
                                                 info);
                               that fills a info buffer at i=row.
                               s.a: board/eltec/bab7xx.
*/

#include <config.h>

#ifdef CONFIG_CFB_CONSOLE

/*****************************************************************************/
/* Console device defines with SMI graphic                                   */
/* Any other graphic must change this section                                */
/*****************************************************************************/

#ifdef  CONFIG_VIDEO_SMI_LYNXEM
#include <smiLynxEM.h>

#define VIDEO_FB_LITTLE_ENDIAN
#define VIDEO_HW_RECTFILL
#define VIDEO_HW_BITBLT

#define VIDEO_VISIBLE_COLS      (smi.winSizeX)
#define VIDEO_VISIBLE_ROWS      (smi.winSizeY)
#define VIDEO_PIXEL_SIZE        (smi.gdfBytesPP)
#define VIDEO_DATA_FORMAT       (smi.gdfIndex)
#define VIDEO_FB_ADRS           (smi.frameAdrs)
#endif

/*****************************************************************************/
/* Console device defines for Shannon (SA-1100) board                        */
/*****************************************************************************/

#ifdef  CONFIG_VIDEO_SHANNON
#include <shannon_video.h>

#define VIDEO_VISIBLE_COLS      LCD_VIDEO_COLS
#define VIDEO_VISIBLE_ROWS      LCD_VIDEO_ROWS
#define VIDEO_PIXEL_SIZE        (LCD_VIDEO_BPP/8)
#define VIDEO_DATA_FORMAT       LCD_VIDEO_FORMAT
#define VIDEO_FB_ADRS           LCD_VIDEO_ADDR
#endif

/*****************************************************************************/
/* Console device defines with i8042 keyboard controller                     */
/* Any other keyboard controller must change this section                    */
/*****************************************************************************/

#ifdef  CONFIG_I8042_KBD
#include <i8042.h>

#define VIDEO_KBD_INIT_FCT      i8042_kbd_init()
#define VIDEO_TSTC_FCT          i8042_tstc
#define VIDEO_GETC_FCT          i8042_getc
#endif


/*****************************************************************************/
/* Console device                                                            */
/*****************************************************************************/

#include <version.h>
#include <armboot.h>
#include <types.h>
#include <video_font.h>
#ifdef CFG_CMD_DATE
#include <rtc.h>

#endif

#ifndef cpu_to_be32
#ifdef __BIG_ENDIAN
#define cpu_to_be32(x)	(x)
#else
#define cpu_to_be32(x)   ((((x) & 0x000000ff) << 24) | (((x) & 0x0000ff00) << 8)|\
                          (((x) & 0x00ff0000) >>  8) | (((x) & 0xff000000) >> 24) )
#endif
#endif

#define CONSOLE_BG_COL            0x00
#define CONSOLE_FG_COL            0xa0

/*
 * Graphic Data Format (GDF) bits for VIDEO_DATA_FORMAT
 */
#define GDF__8BIT_INDEX         0
#define GDF_15BIT_555RGB        1
#define GDF_16BIT_565RGB        2
#define GDF_32BIT_X888RGB       3
#define GDF_24BIT_888RGB        4
#define GDF__8BIT_332RGB        5

#ifdef  CONFIG_CONSOLE_CURSOR
void    console_cursor (int state);
static int cursor_state = 0; /* cursor state (on or off) */
#define CURSOR_OFF console_cursor(0);
#ifdef CONFIG_I8042_KBD
/* Cursor drawing done in drivers/i8042.c */
#define CURSOR_ON
#define CURSOR_CLR
#else
#define CURSOR_ON  console_cursor(1);
#define CURSOR_CLR	(cursor_state = 0);
#endif
#else
#define CURSOR_ON
#define CURSOR_OFF
#define CURSOR_CLR
#ifdef  CONFIG_CONSOLE_TIME
#error  CONFIG_CONSOLE_CURSOR must be defined for CONFIG_CONSOLE_TIME
#endif
#endif

#ifdef  CONFIG_VIDEO_LOGO
#define LINUX_LOGO_WIDTH        80
#define LINUX_LOGO_HEIGHT       80
#define LINUX_LOGO_COLORS       214
#define LINUX_LOGO_LUT_OFFSET   0x20
#define __initdata
#include <linux_logo.h>
#define VIDEO_LOGO_WIDTH        LINUX_LOGO_WIDTH
#define VIDEO_LOGO_HEIGHT       LINUX_LOGO_HEIGHT

#define VIDEO_INFO_X            (VIDEO_LOGO_WIDTH)
#define VIDEO_INFO_Y            (VIDEO_FONT_HEIGHT/2)
#else
#define VIDEO_LOGO_WIDTH        0
#define VIDEO_LOGO_HEIGHT       0
#endif

#define VIDEO_COLS              VIDEO_VISIBLE_COLS
#define VIDEO_ROWS              VIDEO_VISIBLE_ROWS
#define VIDEO_SIZE              (VIDEO_ROWS*VIDEO_COLS*VIDEO_PIXEL_SIZE)
#define VIDEO_PIX_BLOCKS        (VIDEO_SIZE >> 2)
#define VIDEO_LINE_LEN          (VIDEO_COLS*VIDEO_PIXEL_SIZE)
#define VIDEO_BURST_LEN         (VIDEO_COLS/8)

#ifdef  CONFIG_VIDEO_LOGO
#define CONSOLE_ROWS            ((VIDEO_ROWS - VIDEO_LOGO_HEIGHT) / VIDEO_FONT_HEIGHT)
#else
#define CONSOLE_ROWS            (VIDEO_ROWS / VIDEO_FONT_HEIGHT)
#endif

#define CONSOLE_COLS            (VIDEO_COLS / VIDEO_FONT_WIDTH)
#define CONSOLE_ROW_SIZE        (VIDEO_FONT_HEIGHT * VIDEO_LINE_LEN)
#define CONSOLE_ROW_FIRST       (video_console_address)
#define CONSOLE_ROW_SECOND      (video_console_address + CONSOLE_ROW_SIZE)
#define CONSOLE_ROW_LAST        (video_console_address + CONSOLE_SIZE - CONSOLE_ROW_SIZE)
#define CONSOLE_SIZE            (CONSOLE_ROW_SIZE * CONSOLE_ROWS)
#define CONSOLE_SCROLL_SIZE     (CONSOLE_SIZE - CONSOLE_ROW_SIZE)

/* Macros */

#ifdef  VIDEO_FB_LITTLE_ENDIAN
#define swap16(x)        ((((x) & 0x00ff) << 8) | ( (x) >> 8))
#define swap32(x)        ((((x) & 0x000000ff) << 24) | (((x) & 0x0000ff00) << 8)|\
                          (((x) & 0x00ff0000) >>  8) | (((x) & 0xff000000) >> 24) )
#define shortswap32(x)   ((((x) & 0x000000ff) <<  8) | (((x) & 0x0000ff00) >> 8)|\
                          (((x) & 0x00ff0000) <<  8) | (((x) & 0xff000000) >> 8) )
#else
#define swap16(x)        (x)
#define swap32(x)        (x)
#define shortswap32(x)   (x)
#endif

#if defined(DEBUG) || defined(DEBUG_CFB_CONSOLE)
#define PRINTD(x)         printf(x)
#else
#define PRINTD(x)
#endif

/* Imports */

extern void *video_hw_init (void);    /* return the graphic device struct or NULL */

extern void video_hw_bitblt (
    unsigned int bpp,       /* bytes per pixel */
    unsigned int src_x,     /* source pos x */
    unsigned int src_y,     /* source pos y */
    unsigned int dst_x,     /* dest pos x */
    unsigned int dst_y,     /* dest pos y */
    unsigned int dim_x,     /* frame width */
    unsigned int dim_y      /* frame height */
    );

extern void video_hw_rectfill (
    unsigned int bpp,       /* bytes per pixel */
    unsigned int dst_x,     /* dest pos x */
    unsigned int dst_y,     /* dest pos y */
    unsigned int dim_x,     /* frame width */
    unsigned int dim_y,     /* frame height */
    unsigned int color      /* fill color */
    );

extern void video_set_lut (
    unsigned int index,     /* color number */
    unsigned char r,        /* red */
    unsigned char g,        /* green */
    unsigned char b         /* blue */
    );

extern void video_get_info_str (    /* setup a board string: type, speed, etc. */
    int line_number,        /* location to place info string beside logo */
    char *info              /* buffer for info string */
    );


/* Locals */
static void *video_fb_address;          /* frame buffer address */
static void *video_console_address;     /* console buffer start address */

static int console_col = 0; /* cursor col */
static int console_row = 0; /* cursor row */

static u32 eorx, fgx, bgx;  /* color pats */

static const int video_font_draw_table8[] = {
#ifdef __BIG_ENDIAN
            0x00000000, 0x000000ff, 0x0000ff00, 0x0000ffff,
            0x00ff0000, 0x00ff00ff, 0x00ffff00, 0x00ffffff,
            0xff000000, 0xff0000ff, 0xff00ff00, 0xff00ffff,
            0xffff0000, 0xffff00ff, 0xffffff00, 0xffffffff };
#else
            0x00000000, 0xff000000, 0x00ff0000, 0xffff0000,
            0x0000ff00, 0xff00ff00, 0x00ffff00, 0xffffff00,
            0x000000ff, 0xff0000ff, 0x00ff00ff, 0xffff00ff,
            0x0000ffff, 0xff00ffff, 0x00ffffff, 0xffffffff };
#endif


static const int video_font_draw_table15[] = {
#ifdef __BIG_ENDIAN
            0x00000000, 0x00007fff, 0x7fff0000, 0x7fff7fff };
#else
            0x00000000, 0xff7f0000, 0x0000ff7f, 0xff7fff7f };
#endif


static const int video_font_draw_table16[] = {
#ifdef __BIG_ENDIAN
            0x00000000, 0x0000ffff, 0xffff0000, 0xffffffff };
#else
            0x00000000, 0xffff0000, 0x0000ffff, 0xffffffff };
#endif


static const int video_font_draw_table24[16][3] = {
#ifdef __BIG_ENDIAN
            { 0x00000000, 0x00000000, 0x00000000 },
            { 0x00000000, 0x00000000, 0x00ffffff },
            { 0x00000000, 0x0000ffff, 0xff000000 },
            { 0x00000000, 0x0000ffff, 0xffffffff },
            { 0x000000ff, 0xffff0000, 0x00000000 },
            { 0x000000ff, 0xffff0000, 0x00ffffff },
            { 0x000000ff, 0xffffffff, 0xff000000 },
            { 0x000000ff, 0xffffffff, 0xffffffff },
            { 0xffffff00, 0x00000000, 0x00000000 },
            { 0xffffff00, 0x00000000, 0x00ffffff },
            { 0xffffff00, 0x0000ffff, 0xff000000 },
            { 0xffffff00, 0x0000ffff, 0xffffffff },
            { 0xffffffff, 0xffff0000, 0x00000000 },
            { 0xffffffff, 0xffff0000, 0x00ffffff },
            { 0xffffffff, 0xffffffff, 0xff000000 },
            { 0xffffffff, 0xffffffff, 0xffffffff } };
#else
            { 0x00000000, 0x00000000, 0x00000000 },
            { 0x00000000, 0x00000000, 0xffffff00 },
            { 0x00000000, 0xffff0000, 0x000000ff },
            { 0x00000000, 0xffff0000, 0xffffffff },
            { 0xff000000, 0x0000ffff, 0x00000000 },
            { 0xff000000, 0x0000ffff, 0xffffff00 },
            { 0xff000000, 0xffffffff, 0x000000ff },
            { 0xff000000, 0xffffffff, 0xffffffff },
            { 0x00ffffff, 0x00000000, 0x00000000 },
            { 0x00ffffff, 0x00000000, 0xffffff00 },
            { 0x00ffffff, 0xffff0000, 0x000000ff },
            { 0x00ffffff, 0xffff0000, 0xffffffff },
            { 0xffffffff, 0x0000ffff, 0x00000000 },
            { 0xffffffff, 0x0000ffff, 0xffffff00 },
            { 0xffffffff, 0xffffffff, 0x000000ff },
            { 0xffffffff, 0xffffffff, 0xffffffff } };
#endif


static const int video_font_draw_table32[16][4] = {
#ifdef __BIG_ENDIAN
            { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
            { 0x00000000, 0x00000000, 0x00000000, 0x00ffffff },
            { 0x00000000, 0x00000000, 0x00ffffff, 0x00000000 },
            { 0x00000000, 0x00000000, 0x00ffffff, 0x00ffffff },
            { 0x00000000, 0x00ffffff, 0x00000000, 0x00000000 },
            { 0x00000000, 0x00ffffff, 0x00000000, 0x00ffffff },
            { 0x00000000, 0x00ffffff, 0x00ffffff, 0x00000000 },
            { 0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff },
            { 0x00ffffff, 0x00000000, 0x00000000, 0x00000000 },
            { 0x00ffffff, 0x00000000, 0x00000000, 0x00ffffff },
            { 0x00ffffff, 0x00000000, 0x00ffffff, 0x00000000 },
            { 0x00ffffff, 0x00000000, 0x00ffffff, 0x00ffffff },
            { 0x00ffffff, 0x00ffffff, 0x00000000, 0x00000000 },
            { 0x00ffffff, 0x00ffffff, 0x00000000, 0x00ffffff },
            { 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00000000 },
            { 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff } };
#else
            { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
            { 0x00000000, 0x00000000, 0x00000000, 0xffffff00 },
            { 0x00000000, 0x00000000, 0xffffff00, 0x00000000 },
            { 0x00000000, 0x00000000, 0xffffff00, 0xffffff00 },
            { 0x00000000, 0xffffff00, 0x00000000, 0x00000000 },
            { 0x00000000, 0xffffff00, 0x00000000, 0xffffff00 },
            { 0x00000000, 0xffffff00, 0xffffff00, 0x00000000 },
            { 0x00000000, 0xffffff00, 0xffffff00, 0xffffff00 },
            { 0xffffff00, 0x00000000, 0x00000000, 0x00000000 },
            { 0xffffff00, 0x00000000, 0x00000000, 0xffffff00 },
            { 0xffffff00, 0x00000000, 0xffffff00, 0x00000000 },
            { 0xffffff00, 0x00000000, 0xffffff00, 0xffffff00 },
            { 0xffffff00, 0xffffff00, 0x00000000, 0x00000000 },
            { 0xffffff00, 0xffffff00, 0x00000000, 0xffffff00 },
            { 0xffffff00, 0xffffff00, 0xffffff00, 0x00000000 },
            { 0xffffff00, 0xffffff00, 0xffffff00, 0xffffff00 } };
#endif

/******************************************************************************/


static void video_drawchars (int xx, int yy, unsigned char *s, int count)
{
    u8  *cdat, *dest, *dest0;
    int rows, offset, c;

    offset = yy * VIDEO_LINE_LEN + xx * VIDEO_PIXEL_SIZE;
    dest0 = video_fb_address + offset;

    switch (VIDEO_DATA_FORMAT)
    {
    case GDF__8BIT_INDEX:
    case GDF__8BIT_332RGB:
        while (count--)
        {
        c = *s ;
        cdat = video_fontdata + c * VIDEO_FONT_HEIGHT;
        for (rows = VIDEO_FONT_HEIGHT, dest = dest0; rows--; dest += VIDEO_LINE_LEN)
        {
            u8 bits = *cdat++;
            ((u32 *)dest)[0] = (video_font_draw_table8[bits >> 4] & eorx) ^ bgx;
            ((u32 *)dest)[1] = (video_font_draw_table8[bits & 15] & eorx) ^ bgx;
        }
        dest0 += VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE;
        s++;
        }
        break;

    case GDF_15BIT_555RGB:
        while (count--)
        {
        c = *s ;
        cdat = video_fontdata + c * VIDEO_FONT_HEIGHT;
        for (rows = VIDEO_FONT_HEIGHT, dest = dest0; rows--; dest += VIDEO_LINE_LEN)
        {
            u8 bits = *cdat++;
            ((u32 *)dest)[0] = shortswap32((video_font_draw_table15[bits >> 6] & eorx) ^ bgx);
            ((u32 *)dest)[1] = shortswap32((video_font_draw_table15[bits >> 4 & 3] & eorx) ^ bgx);
            ((u32 *)dest)[2] = shortswap32((video_font_draw_table15[bits >> 2 & 3] & eorx) ^ bgx);
            ((u32 *)dest)[3] = shortswap32((video_font_draw_table15[bits & 3] & eorx) ^ bgx);
            }
            dest0 += VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE;
            s++ ;
        }
        break;

    case GDF_16BIT_565RGB:
        while (count--)
        {
        c = *s ;
        cdat = video_fontdata + c * VIDEO_FONT_HEIGHT;
        for (rows = VIDEO_FONT_HEIGHT, dest = dest0; rows--; dest += VIDEO_LINE_LEN)
        {
            u8 bits = *cdat++;
            ((u32 *)dest)[0] = shortswap32((video_font_draw_table16[bits >> 6] & eorx) ^ bgx);
            ((u32 *)dest)[1] = shortswap32((video_font_draw_table16[bits >> 4 & 3] & eorx) ^ bgx);
            ((u32 *)dest)[2] = shortswap32((video_font_draw_table16[bits >> 2 & 3] & eorx) ^ bgx);
            ((u32 *)dest)[3] = shortswap32((video_font_draw_table16[bits & 3] & eorx) ^ bgx);
        }
        dest0 += VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE;
        s++ ;
        }
        break;

    case GDF_32BIT_X888RGB:
        while (count--)
        {
        c = *s ;
        cdat = video_fontdata + c * VIDEO_FONT_HEIGHT;
        for (rows = VIDEO_FONT_HEIGHT, dest = dest0; rows--; dest += VIDEO_LINE_LEN)
        {
            u8 bits = *cdat++;
            ((u32 *)dest)[0] = swap32((video_font_draw_table32[bits >> 4][0] & eorx) ^ bgx);
            ((u32 *)dest)[1] = swap32((video_font_draw_table32[bits >> 4][1] & eorx) ^ bgx);
            ((u32 *)dest)[2] = swap32((video_font_draw_table32[bits >> 4][2] & eorx) ^ bgx);
            ((u32 *)dest)[3] = swap32((video_font_draw_table32[bits >> 4][3] & eorx) ^ bgx);
            ((u32 *)dest)[4] = swap32((video_font_draw_table32[bits & 15][0] & eorx) ^ bgx);
            ((u32 *)dest)[5] = swap32((video_font_draw_table32[bits & 15][1] & eorx) ^ bgx);
            ((u32 *)dest)[6] = swap32((video_font_draw_table32[bits & 15][2] & eorx) ^ bgx);
            ((u32 *)dest)[7] = swap32((video_font_draw_table32[bits & 15][3] & eorx) ^ bgx);
        }
        dest0 += VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE;
        s++ ;
        }
        break;

    case GDF_24BIT_888RGB:
        while (count--)
        {
        c = *s ;
        cdat = video_fontdata + c * VIDEO_FONT_HEIGHT;
        for (rows = VIDEO_FONT_HEIGHT, dest = dest0; rows--; dest += VIDEO_LINE_LEN)
        {
            u8 bits = *cdat++;
            ((u32 *)dest)[0] = (video_font_draw_table24[bits >> 4][0] & eorx) ^ bgx;
            ((u32 *)dest)[1] = (video_font_draw_table24[bits >> 4][1] & eorx) ^ bgx;
            ((u32 *)dest)[2] = (video_font_draw_table24[bits >> 4][2] & eorx) ^ bgx;
            ((u32 *)dest)[3] = (video_font_draw_table24[bits & 15][0] & eorx) ^ bgx;
            ((u32 *)dest)[4] = (video_font_draw_table24[bits & 15][1] & eorx) ^ bgx;
            ((u32 *)dest)[5] = (video_font_draw_table24[bits & 15][2] & eorx) ^ bgx;
        }
        dest0 += VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE;
        s++ ;
        }
        break;
    }
}

/*****************************************************************************/

static inline void video_drawstring(int xx, int yy, unsigned char *s)
{
    video_drawchars (xx, yy, s, strlen(s));
}

/*****************************************************************************/

static void video_putchar(int xx, int yy, unsigned char c)
{
#ifdef CONFIG_VIDEO_LOGO
    video_drawchars (xx, yy + VIDEO_LOGO_HEIGHT, &c, 1);
#else
    video_drawchars (xx, yy, &c, 1);
#endif
}

/*****************************************************************************/

#ifdef  CONFIG_CONSOLE_CURSOR
void console_cursor (int state)
{
#ifdef CONFIG_CONSOLE_TIME
    struct rtc_time tm;
    char info[16];

    /* time update only if cursor is on (faster scroll) */
    if (state)
    {
    rtc_get (&tm);

    sprintf(info, " %02d:%02d:%02d ", tm.tm_hour, tm.tm_min, tm.tm_sec);
    video_drawstring(VIDEO_VISIBLE_COLS-10*VIDEO_FONT_WIDTH,
             VIDEO_INFO_Y, info);

    sprintf(info, "%02d.%02d.%04d", tm.tm_mday, tm.tm_mon, tm.tm_year);
    video_drawstring(VIDEO_VISIBLE_COLS-10*VIDEO_FONT_WIDTH,
             VIDEO_INFO_Y+1*VIDEO_FONT_HEIGHT, info);
    }
#endif

    if (state && (cursor_state != state))
    {
        /* swap drawing colors */
        eorx = fgx;
        fgx  = bgx;
        bgx  = eorx;
        eorx = fgx ^ bgx;
        /* draw cursor */
        video_putchar (console_col * VIDEO_FONT_WIDTH,
                       console_row * VIDEO_FONT_HEIGHT, ' ');
        /* restore drawing colors */
        eorx = fgx;
        fgx  = bgx;
        bgx  = eorx;
        eorx = fgx ^ bgx;
    }

    if (!state && (cursor_state != state))
    {
        /* clear cursor */
        video_putchar (console_col * VIDEO_FONT_WIDTH,
                       console_row * VIDEO_FONT_HEIGHT, ' ');
    }

    cursor_state = state;
}
#endif

/*****************************************************************************/

#ifndef VIDEO_HW_RECTFILL
static void memsetl (int *p, int c, int v)
{
    while (c--)
        *(p++) = v;
}
#endif

/*****************************************************************************/

#ifndef VIDEO_HW_BITBLT
static void memcpyl (int *d, int *s, int c)
{
    while (c--)
        *(d++) = *(s++);
}
#endif

/*****************************************************************************/

static void console_scrollup (void)
{
    /* copy up rows ignoring the first one */

#ifdef VIDEO_HW_BITBLT
    video_hw_bitblt (
    VIDEO_PIXEL_SIZE,     /* bytes per pixel */
    0,                    /* source pos x */
    VIDEO_LOGO_HEIGHT + VIDEO_FONT_HEIGHT, /* source pos y */
    0,                    /* dest pos x */
    VIDEO_LOGO_HEIGHT,    /* dest pos y */
    VIDEO_VISIBLE_COLS,   /* frame width */
    VIDEO_VISIBLE_ROWS - VIDEO_LOGO_HEIGHT - VIDEO_FONT_HEIGHT /* frame height */
    );
#else
    memcpyl (CONSOLE_ROW_FIRST, CONSOLE_ROW_SECOND, CONSOLE_SCROLL_SIZE >> 2);
#endif

    /* clear the last one */
#ifdef VIDEO_HW_RECTFILL
    video_hw_rectfill (
    VIDEO_PIXEL_SIZE,     /* bytes per pixel */
    0,                    /* dest pos x */
    VIDEO_VISIBLE_ROWS - VIDEO_FONT_HEIGHT,  /* dest pos y */
    VIDEO_VISIBLE_COLS,   /* frame width */
    VIDEO_FONT_HEIGHT,    /* frame height */
    CONSOLE_BG_COL          /* fill color */
    );
#else
    memsetl (CONSOLE_ROW_LAST, CONSOLE_ROW_SIZE >> 2, CONSOLE_BG_COL);
#endif
}

/*****************************************************************************/

static inline void console_back (void)
{
    CURSOR_OFF
    console_col--;

    if (console_col < 0)
    {
        console_col = CONSOLE_COLS - 1;
        console_row--;
        if (console_row < 0)
        console_row = 0;
    }
    video_putchar (console_col * VIDEO_FONT_WIDTH,
                   console_row * VIDEO_FONT_HEIGHT, ' ');
}

/*****************************************************************************/

static void console_newline (void)
{
    CURSOR_OFF
    console_row++;
    console_col = 0;

    /* Check if we need to scroll the terminal */
    if (console_row >= CONSOLE_ROWS)
    {
    /* Scroll everything up */
    console_scrollup ();

    /* Decrement row number */
    console_row--;
    }
}

/*****************************************************************************/

void video_putc (const char c)
{
    switch (c)
    {
    case 13: /* ignore */
        break;

    case '\n': /* next line */
        console_newline();
		CURSOR_ON
        break;

    case 9:    /* tab 8 */
        CURSOR_OFF
        console_col |=  0x0008;
        console_col &= ~0x0007;

        if (console_col >= CONSOLE_COLS)
            console_newline();
		CURSOR_ON
        break;

    case 8:    /* backspace */
        console_back();
		CURSOR_ON
        break;

    default: /* draw the char */
        CURSOR_CLR
        video_putchar (console_col * VIDEO_FONT_WIDTH,
                       console_row * VIDEO_FONT_HEIGHT, c);
        console_col++ ;

        /* check for newline */
        if (console_col >= CONSOLE_COLS)
            console_newline();
		CURSOR_ON
		break;
    }
}



/*****************************************************************************/

void video_puts (const char *s)
{
    int count = strlen(s);

    while(count--)
        video_putc(*s++);
}

/*****************************************************************************/

#ifdef CONFIG_VIDEO_LOGO
void logo_plot (void *screen, int width, int x, int y)
{
    int skip = (width - LINUX_LOGO_WIDTH) * VIDEO_PIXEL_SIZE,
        xcount, i,
        ycount = LINUX_LOGO_HEIGHT;
    unsigned char
        *source = linux_logo,
        *dest   = (unsigned char *) screen + ((y * width * VIDEO_PIXEL_SIZE) + x),
        r, g, b;

    if (VIDEO_DATA_FORMAT == GDF__8BIT_INDEX)
    {
        for (i = 0; i < LINUX_LOGO_COLORS; i++)
        {
            r = (unsigned char)linux_logo_red  [i];
            g = (unsigned char)linux_logo_green[i];
            b = (unsigned char)linux_logo_blue [i];
            video_set_lut (LINUX_LOGO_LUT_OFFSET + i, r, g, b);
        }
    }

    while (ycount--)
    {
    xcount = LINUX_LOGO_WIDTH;
    while (xcount--)
    {
        r = (unsigned char)linux_logo_red  [*source - LINUX_LOGO_LUT_OFFSET];
        g = (unsigned char)linux_logo_green[*source - LINUX_LOGO_LUT_OFFSET];
        b = (unsigned char)linux_logo_blue [*source - LINUX_LOGO_LUT_OFFSET];

        switch (VIDEO_DATA_FORMAT)
        {
        case GDF__8BIT_INDEX:
            *dest = *source;
            break;
        case GDF__8BIT_332RGB:
            *dest = ((r>>5)<<5) | ((g>>5)<<2) | (b>>6);
            break;
        case GDF_15BIT_555RGB:
            *(unsigned short *)dest =
            swap16((unsigned short)(((r>>3)<<10) | ((g>>3)<<5) | (b>>3)));
            break;
        case GDF_16BIT_565RGB:
            *(unsigned short *)dest =
            swap16((unsigned short)(((r>>3)<<11) | ((g>>2)<<5) | (b>>3)));
            break;
        case GDF_32BIT_X888RGB:
            *(unsigned long  *)dest =
            swap32((unsigned long)((r<<16) | (g<<8) | b));
            break;
        case GDF_24BIT_888RGB:
#ifdef VIDEO_FB_LITTLE_ENDIAN
            dest[0] = b;
            dest[1] = g;
            dest[2] = r;
#else
            dest[0] = r;
            dest[1] = g;
            dest[2] = b;
#endif
            break;
        }
        source++;
        dest += VIDEO_PIXEL_SIZE;
    }
    dest += skip;
    }
}


/*****************************************************************************/

static void *video_logo (void)
{
    char info[128];

    logo_plot (video_fb_address, VIDEO_COLS, 0, 0);

    sprintf(info, " %s (%s - %s)", ARMBOOT_VERSION, __DATE__, __TIME__);
    video_drawstring (VIDEO_INFO_X, VIDEO_INFO_Y, info);

#ifdef CONFIG_CONSOLE_EXTRA_INFO
    {
    int i, n = ((VIDEO_LOGO_HEIGHT-VIDEO_FONT_HEIGHT)/VIDEO_FONT_HEIGHT);

    for (i = 1; i < n; i++)
    {
        video_get_info_str (i, info);
        if (*info)
        video_drawstring (VIDEO_INFO_X,
                          VIDEO_INFO_Y + i*VIDEO_FONT_HEIGHT, info);
    }
    }
#endif

    return (video_fb_address + VIDEO_LOGO_HEIGHT * VIDEO_LINE_LEN);
}
#endif


/*****************************************************************************/

int video_init(void)
{
    unsigned char color8;

    if (video_hw_init() == NULL)
        return -1;

    video_fb_address = (void*)VIDEO_FB_ADRS;

    /* Init drawing pats */
    switch (VIDEO_DATA_FORMAT)
    {
    case GDF__8BIT_INDEX:
        video_set_lut (0x01, CONSOLE_FG_COL, CONSOLE_FG_COL, CONSOLE_FG_COL);
        video_set_lut (0x00, CONSOLE_BG_COL, CONSOLE_BG_COL, CONSOLE_BG_COL);
        fgx = 0x01010101;
        bgx = 0x00000000;
        break;
    case GDF__8BIT_332RGB:
        color8 = ((CONSOLE_FG_COL & 0xe0) | ((CONSOLE_FG_COL>>3) & 0x1c) | CONSOLE_FG_COL>>6);
        fgx = (color8<<24) | (color8<<16) | (color8<<8) | color8;
        color8 = ((CONSOLE_BG_COL & 0xe0) | ((CONSOLE_BG_COL>>3) & 0x1c) | CONSOLE_BG_COL>>6);
        bgx = (color8<<24) | (color8<<16) | (color8<<8) | color8;
        break;
    case GDF_15BIT_555RGB:
        fgx = (((CONSOLE_FG_COL>>3)<<26) | ((CONSOLE_FG_COL>>3)<<21) | ((CONSOLE_FG_COL>>3)<<16) |
           ((CONSOLE_FG_COL>>3)<<10) | ((CONSOLE_FG_COL>>3)<<5)  |  (CONSOLE_FG_COL>>3));
        bgx = (((CONSOLE_BG_COL>>3)<<26) | ((CONSOLE_BG_COL>>3)<<21) | ((CONSOLE_BG_COL>>3)<<16) |
           ((CONSOLE_BG_COL>>3)<<10) | ((CONSOLE_BG_COL>>3)<<5)  |  (CONSOLE_BG_COL>>3));
        break;
    case GDF_16BIT_565RGB:
        fgx = (((CONSOLE_FG_COL>>3)<<27) | ((CONSOLE_FG_COL>>2)<<21) | ((CONSOLE_FG_COL>>3)<<16) |
           ((CONSOLE_FG_COL>>3)<<11) | ((CONSOLE_FG_COL>>2)<<5)  |  (CONSOLE_FG_COL>>3));
        bgx = (((CONSOLE_BG_COL>>3)<<27) | ((CONSOLE_BG_COL>>2)<<21) | ((CONSOLE_BG_COL>>3)<<16) |
           ((CONSOLE_BG_COL>>3)<<11) | ((CONSOLE_BG_COL>>2)<<5)  |  (CONSOLE_BG_COL>>3));
        break;
    case GDF_32BIT_X888RGB:
        fgx = (CONSOLE_FG_COL<<16) | (CONSOLE_FG_COL<<8) | CONSOLE_FG_COL;
        bgx = (CONSOLE_BG_COL<<16) | (CONSOLE_BG_COL<<8) | CONSOLE_BG_COL;
        break;
    case GDF_24BIT_888RGB:
        fgx = (CONSOLE_FG_COL<<24) | (CONSOLE_FG_COL<<16) | (CONSOLE_FG_COL<<8) | CONSOLE_FG_COL;
        bgx = (CONSOLE_BG_COL<<24) | (CONSOLE_BG_COL<<16) | (CONSOLE_BG_COL<<8) | CONSOLE_BG_COL;
        break;
    }

	/* adapt fgx/bgx to host endianess */
	fgx = cpu_to_be32(fgx);
	bgx = cpu_to_be32(bgx);

    eorx = fgx ^ bgx;

#ifdef CONFIG_VIDEO_LOGO
    /* Plot the logo and get start point of console */
    PRINTD("Video: Drawing the logo ...\n");
    video_console_address = video_logo();
#else
    video_console_address = video_fb_address;
#endif

    /* Initialize the console */
    console_col  = 0;
    console_row  = 0;

    return 0 ;
}



#endif /* CONFIG_CFB_CONSOLE */
