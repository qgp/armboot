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
#include "config.h"

#ifdef CONFIG_FRAMEBUFFER


/*
************************************************************
** 
** Bitmap screen driver
** 
************************************************************
*/

/*
** The routines in this module provide an interface for the ansi
** terminal emulator (ansi_console.c) to access a raster graphics
** screen (aka framebuffer).
*/

#include "font.h"
#include "cursor.h"

/*
************************************************************
** Imports
************************************************************
*/

extern void video_set_lut(
    unsigned int index,     /* color number */
    unsigned char r,        /* red */
    unsigned char g,        /* green */
    unsigned char b);       /* blue */

extern void *video_framebuffer_init(
	unsigned int *ax,		/* number of columns */
	unsigned int *ay,		/* number of columns */
	unsigned int *pitch,	/* vertical address offset */
	unsigned int *fcol,		/* foreground color */
	unsigned int *bcol);	/* background color */

/*
************************************************************
** Some macros
************************************************************
*/

/*
** This defines the pixel size used by the frame buffer.
** BPP_SHIFT = log2(BitsPerPixel), i.e. for eight bits
** per pixel, this is 3.
**
** Note:
** This code *should* work for BPP_SHIFT= 0, 1, 2, 3 and 4,
** but as of today, only BPP_SHIFT = 3 has been tested!
*/

#if (CONFIG_FRAMEBUFFER_BPP == 16)
#	define BPP_SHIFT	4
#elif (CONFIG_FRAMEBUFFER_BPP == 8)
#	define BPP_SHIFT	3
#elif (CONFIG_FRAMEBUFFER_BPP == 4)
#	define BPP_SHIFT	2
#elif (CONFIG_FRAMEBUFFER_BPP == 2)
#	define BPP_SHIFT	1
#elif (CONFIG_FRAMEBUFFER_BPP == 1)
#	define BPP_SHIFT	0
#else
#error "unsupported CONFIG_FRAMEBUFFER_BPP"
#endif


#define ABS(x)	((x) < 0 ? -(x) : (x))

/*
** All framebuffer accesses done by this code are 32-bit wide
*/
#define BITS_PER_PIXEL	(1 << BPP_SHIFT)
#define PIXEL_PER_LONG	(32 >> BPP_SHIFT)

#define UNIT_SHIFT		5
#define BITS_PER_UNIT	(1 << UNIT_SHIFT)
#define PIXEL_PER_UNIT	(BITS_PER_UNIT / BITS_PER_PIXEL)
#define ALIGN_MASK		(BITS_PER_UNIT - 1)
#define UNITS_PER_LINE	(PITCH / PIXEL_PER_UNIT)
#define PTNSHIFT		(1 << (UNIT_SHIFT-BPP_SHIFT))
#define PTNMASK			((1 << PTNSHIFT) - 1)

/*
** Macros for adapting this code to different endianesses:
**
** Note:
** This code *should* work for little endian as well as
** big endian configurations, however as of today, only
** little endian has been tested!
*/
#ifdef __BIG_ENDIAN

/* big endian case */
#define hosttofb(x)		(x)
#define fbtohost(x)		(x)
#define fb_shr(x, s)	((x) >> (s))
#define fb_shl(x, s)	((x) << (s))

#else

/* little endian case */
#define SWAP32(x)	((((x) >> 24) & 0x000000ff)| \
					(((x) >>  8) & 0x0000ff00) | \
					(((x) <<  8) & 0x00ff0000) | \
					(((x) << 24) & 0xff000000) )

#define hosttofb(x)		SWAP32(x)
#define fbtohost(x)		SWAP32(x)
#define fb_shr(x, s)	((x) << (s))
#define fb_shl(x, s)	((x) >> (s))

#endif



/*
************************************************************
** Static storage requirements
************************************************************
*/
struct ScreenDev
{
	unsigned int			PlnSizX, PlnSizY;	/* screen size (in pixels) */
	const struct FontHead	*pFontHead;			/* font description parameters  */
	const unsigned char		*pFont;				/* Pointer to 1.st char pattern */
	volatile unsigned long	*DispStartAddr;		/* display start address */
	unsigned int	   		Pitch;				/* vertical address offset */
	/*
	** conversion tables
	** Note: for BPP_SHIFT = 1, CharConv will be *very* big (256KB)
	*/       
	unsigned long			maskleft[BITS_PER_UNIT+1];
	unsigned long			maskright[BITS_PER_UNIT+1];
#if (BPP_SHIFT > 0)
	unsigned long			CharConv[1 << PTNSHIFT];
#endif
};

static struct ScreenDev this_dev;
#define GetScreenDev()	(&this_dev)

/*
** shortcuts to access static variables:
*/
#define FNTHEAD		(pSB->pFontHead)
#define PITCH		(pSB->Pitch)
#define MASKLEFT	(pSB->maskleft)
#define MASKRIGHT	(pSB->maskright)
#define CHARCONV	(pSB->CharConv)
#define ADRS(y_start, x_aligned)\
	((volatile unsigned long *) ((pSB->DispStartAddr) + \
		((y_start) * units_per_line + (x_aligned))))

/*
** Imports from font module:
*/
extern const struct FontHead	font_header;
extern const unsigned char		font_pattern[];

#define GetFntHeadAddr()	(&font_header)
#define GetFntPtnAddr()		(font_pattern)

/*
************************************************************
**
** Raster graphics (framebuffer) manipulation routines
**
************************************************************
*/

/*
************************************************************
** Local helper routine: the "color repeater"
************************************************************
*/
static __inline__ unsigned long repcol(int color)
{
	unsigned long d = color;
	int ColoredPixel,BitsPerPixel, ppl;

	ppl = PIXEL_PER_LONG;

	BitsPerPixel = BITS_PER_PIXEL;
	ColoredPixel = ((1<<BitsPerPixel)-1) & d;

	while (--ppl > 0) {
		d <<= BitsPerPixel;
		d |= ColoredPixel ;
	}
	return(d);
}


/*
************************************************************
** Initialize the device
************************************************************
*/
static void raster_init(
	struct ScreenDev *pSB,
	unsigned int *fcol,
	unsigned int *bcol)
{
	unsigned int i, j, ones;

	/*
	** Initialize hardware, set video parameters
	*/
	pSB->DispStartAddr = (volatile unsigned long*)
		video_framebuffer_init(&pSB->PlnSizX, &pSB->PlnSizY,
		                       &pSB->Pitch, fcol, bcol);
	/*
	** set color palette entries for black and white
	*/
#ifdef CONFIG_FRAMEBUFFER_CLUT
	video_set_lut(*bcol, 0, 0, 0);
	video_set_lut(*fcol, -1, -1, -1);
#endif

	/*
	** Initialize conversion arrays in static storage
	*/
	for(i = 0; i < BITS_PER_UNIT+1; i++)
	{
		pSB->maskleft[i]  = hosttofb(0xffffffff << (BITS_PER_UNIT - i));
		pSB->maskright[i] = hosttofb(0xffffffff >> (BITS_PER_UNIT - i));
	}
	ones = (1 << BITS_PER_PIXEL) - 1;
	for (i = 0; i < (1 << PTNSHIFT); i++)
	{
		CHARCONV[i] = 0x00000000;
		for(j = 0; j < PTNSHIFT; j++)
		{
			if (i & (1<<j))
				CHARCONV[i] |= hosttofb(ones << (j * BITS_PER_PIXEL));
		}
	}
}


/*
************************************************************
** Clear window
************************************************************
*/
static void raster_clear(
	struct ScreenDev *pSB,
	int    color,
	int    xs,
	int    ys,
	int    ax,
	int    ay)
{
	unsigned long col = repcol(color);
	unsigned long lftmsk, rgtmsk;
	long tmp;		/* general purpose register */
	int xs_aligned;
	int nr_of_longs;
	volatile unsigned long *source_ptr, *sp;
	int xs_bits, ax_bits; 	/* pixel coordinates in bits */
	int units_per_line = UNITS_PER_LINE;

	/* Lefthand Edge of Cursor */
	xs_bits = xs << BPP_SHIFT;
	xs_aligned = xs_bits >> UNIT_SHIFT;	/* LONG offset from FRMBUF's left */
	lftmsk = ~MASKLEFT[xs_bits & ALIGN_MASK];	/* mask for lefthand tail */


	/* Righthand Edge of Cursor */
	ax_bits = ax << BPP_SHIFT;
	tmp = xs_bits + ax_bits - 1;	/* bit offset from FRMBUF's left margin  */
	rgtmsk = MASKLEFT[(tmp & ALIGN_MASK) + 1];	/* mask for righthand tail   */

	/* Horizontal Sizing */
	nr_of_longs = ((tmp >> UNIT_SHIFT) - xs_aligned) - 1;	/* LONGS between tails */

	source_ptr = ADRS(ys, xs_aligned);

	while (--ay >= 0)
	{
		sp = source_ptr;

		source_ptr += units_per_line;

		if (nr_of_longs < 0)
			*sp = (*sp & (~lftmsk | ~rgtmsk)) | (col & lftmsk & rgtmsk);
		else
		{
			*sp++ = (*sp & ~lftmsk) | (col & lftmsk);

			tmp = nr_of_longs;
			while (--tmp >= 0)
				*sp++ = col;

			*sp = (*sp & ~rgtmsk) | (col & rgtmsk);
		}
	}
}

/*
************************************************************
** Screen copy routine
************************************************************
*/

/*
**  some macros to copy a normal line
*/

#define CPY_LEFT(sp, dp, maskl)\
{\
	*dp++ = (*sp++ & ~(maskl)) | (*dp & (maskl));\
}

#define CPY_RIGHT(sp, dp, maskr)\
{\
	*dp = (*sp & (maskr)) | (*dp & (~(maskr)));\
}

/*
**  macro to copy a line if copy region is within one longword
*/

#define CPY_SPECIAL(sp, dp, mask, notmask)\
{\
	*dp = (*sp & (mask)) | (*dp & (notmask));\
}


static void raster_vcopy(
	struct ScreenDev *pSB,
	long x_src,
	long y_src,
	long ax,
	long ay,
	long y_dst)
{
	volatile unsigned long *sp, *dp;
	long counter2;
	long units_per_line = UNITS_PER_LINE;
	long counter1;
	long right_margin;
	unsigned long maskr, maskl;
	char *source_ptr, *dest_ptr;
	long abs_ay = ABS(ay);
	long modl, modr;
	long x_aligned;
	long xs_bits, ax_bits;

    /* special mask: combination of left and right margin mask */
	xs_bits = x_src << BPP_SHIFT;
	ax_bits = ax << BPP_SHIFT;
	x_aligned = xs_bits >> UNIT_SHIFT;

	right_margin = (xs_bits + ax_bits) / BITS_PER_UNIT - 1;
	modl = xs_bits & ALIGN_MASK;
	modr = (xs_bits + ax_bits) & ALIGN_MASK;

	/* calculate source and destination address */
	sp = ADRS (y_src, x_aligned);
	dp = ADRS (y_dst, x_aligned);

	if (ay < 0)
		units_per_line = -units_per_line;

	/*
	** speed optimization: declaring source_ptr and dest_ptr
	** as char pointers will allow us pre-multipy units_per_line
	** by four (sizeof(unsigned long)) OUTSIDE of all loops
	*/
	source_ptr = (char*)sp;
	dest_ptr   = (char*)dp;
	units_per_line <<= 2;

	/* copy region within the same longword */
	if (xs_bits + BITS_PER_UNIT > xs_bits + ax_bits)
	{	/* combine left and right margin mask */
		maskl = MASKRIGHT[modl] & MASKLEFT[modr];
		maskr = ~maskl;
		for (counter1 = 0; counter1 <= abs_ay; ++counter1)
		{
			sp = (unsigned long*)source_ptr;
			dp = (unsigned long*)dest_ptr;
			source_ptr += units_per_line;
			dest_ptr   += units_per_line;
			CPY_SPECIAL (sp, dp, maskl, maskr);
		}
	}
	else	/* copy region in different longwords */
	{
		maskl = MASKLEFT[modl];
		maskr = MASKLEFT[modr];
		if(maskr || maskl)
		{
			for (counter1 = 0; counter1 <= abs_ay; ++counter1)
			{
				sp = (unsigned long*)source_ptr;
				dp = (unsigned long*)dest_ptr;
				source_ptr += units_per_line;
				dest_ptr   += units_per_line;
				CPY_LEFT   (sp, dp, maskl);     /* copy left margin of region */
				if((counter2 = right_margin)) do
					*dp++ = *sp++;
				while(--counter2);
				CPY_RIGHT  (sp, dp, maskr);     /* copy right margin of region */
			}
		}
		else
		{	/*
			** maskr = maskl = 0: this special case is well worth
			** to be considered seperately as it seems to be the most
			** common case when scrolling:
			*/
			++right_margin;
			if(((counter1 = (abs_ay + 1)) > 0) && 
			   ((counter2 = right_margin) > 0)) do
			{
				sp = (unsigned long*)source_ptr;
				dp = (unsigned long*)dest_ptr;
				source_ptr += units_per_line;
				dest_ptr   += units_per_line;
				do
					*dp++ = *sp++;
				while(--counter2);
				counter2 = right_margin;
			}
			while(--counter1);
		}
	}
}

/*
************************************************************
** Draw filled rectangle routine
************************************************************
*/

static void raster_frct(
	struct ScreenDev *pSB,
	int color,
	int xs,
	int ys,
	int ax,
	int ay)
{
	unsigned long col = repcol(color);
	unsigned long lftmsk, rgtmsk;
	long tmp;
	int xs_aligned;
	int nr_of_longs;
	volatile unsigned long *source_ptr, *sp;
	int xs_bits, ax_bits;
	int units_per_line = UNITS_PER_LINE;

	/* Lefthand Edge of Cursor */
	xs_bits = xs << BPP_SHIFT;
	xs_aligned = xs_bits >> UNIT_SHIFT;    /* LONG offset from FRMBUF's left margin */
	lftmsk = ~MASKLEFT[xs_bits & ALIGN_MASK]; /* mask for lefthand tail */

	ax_bits = ax << BPP_SHIFT;

	/* Righthand Edge of Cursor */
	tmp = xs_bits + ax_bits - 1;	/* bit offset from FRMBUF's left margin */
	rgtmsk = MASKLEFT[(tmp & ALIGN_MASK) + 1];	/* mask for righthand tail */

	nr_of_longs = ((tmp >> UNIT_SHIFT) - xs_aligned) - 1; /* LONGS between tails */

	source_ptr = ADRS (ys, xs_aligned);

	while(--ay >= 0)
	{
		sp = source_ptr;

		source_ptr += units_per_line;

		if(nr_of_longs < 0)
			*sp ^= col & lftmsk & rgtmsk;
		else
		{
			*sp++ ^= col & lftmsk;

			tmp = nr_of_longs;
			while( --tmp >= 0)
				*sp++ ^= col;
			*sp ^= col & rgtmsk;
		}
	}
}

/*
************************************************************
** Character printer routine
************************************************************
*/

#define CHAR_2LONGS(fp, work_reg)\
{\
	work_reg |= fb_shr(char_reg, shift);\
	*fp++ = work_reg;\
	work_reg = *fp & rgtmsk;\
	work_reg |= fb_shl(char_reg, (BITS_PER_UNIT - shift)) & (~rgtmsk);\
	*fp = work_reg;\
}

#define CHAR_1LONG(fp, work_reg)\
{\
	work_reg |= fb_shr(char_reg, shift) & (~rgtmsk);\
	*fp = work_reg;\
}

#define NEXT_ROW()\
{\
	char_row_ptr += rowsize;\
	ax_bits = ax << BPP_SHIFT;\
}

#define SETUP()\
{\
	crp = (unsigned char *)char_row_ptr;\
	fp = frame_ptr;\
	frame_ptr += units_per_line;\
	work_reg = *fp & lftmsk;\
	ptnshift = 8;\
}

#if (BPP_SHIFT == 0)
#define GET_CHAR_REG(index)\
{\
	char_reg  = fbtohost(*((unsigned long*)crp)++);\
}
#endif

#if (BPP_SHIFT == 1)
#define GET_CHAR_REG(index)\
{\
	char_reg = CHARCONV[*((unsigned short*)crp)++];\
	char_reg = (fcol & char_reg) | (bcol & (char_reg ^ MASKLEFT[index]));\
}
#endif

#if (BPP_SHIFT == 2)
#define GET_CHAR_REG(index)\
{\
	char_reg = CHARCONV[*crp++];\
	char_reg = (fcol & char_reg) | (bcol & (char_reg ^ MASKLEFT[index]));\
}
#endif

#if (BPP_SHIFT == 3)
#define GET_CHAR_REG(index)\
{\
	if((ptnshift -= PTNSHIFT) == 0) {\
		char_reg = CHARCONV[*crp & PTNMASK];\
		crp++;\
		ptnshift = 8;\
	}\
	else\
	{\
		char_reg = CHARCONV[(*crp >> ptnshift) & PTNMASK];\
	}\
	char_reg = (fcol & char_reg) | (bcol & (char_reg ^ MASKLEFT[index]));\
}
#endif

#if (BPP_SHIFT == 4)
#define GET_CHAR_REG(index)\
{\
	if((ptnshift -= PTNSHIFT) == 0) {\
		char_reg = CHARCONV[*crp & PTNMASK];\
		crp++;\
		ptnshift = 8;\
	}\
	else\
	{\
		char_reg = CHARCONV[(*crp >> ptnshift) & PTNMASK];\
	}\
	char_reg = (fcol & char_reg) | (bcol & (char_reg ^ MASKLEFT[index]));\
}
#endif

static void raster_print(
	struct ScreenDev *pSB,
	const unsigned char *char_ptr,
	unsigned long rowsize,				/* width of one pattern line */
	long xs,
	long ys,
	long ax,
	long ay,
	int fgcolor,
	int bgcolor)
{
	unsigned long fcol = repcol(fgcolor);
	unsigned long bcol = repcol(bgcolor);
	unsigned long work_reg, char_reg;
	volatile unsigned long *fp;
	unsigned char *crp;
	const unsigned char *char_row_ptr;
	unsigned long ax_bits;
	int shift;
	unsigned long lftmsk, rgtmsk, ptnshift;
	long tmp;
	volatile unsigned long *frame_ptr;
	int xs_aligned;
	unsigned long xs_bits;
	int units_per_line = UNITS_PER_LINE;

	/* Lefthand Edge of cursor */
	xs_bits = xs << BPP_SHIFT;
	xs_aligned = xs_bits >> UNIT_SHIFT;	/* long offset from frmbuf's left margin */
	lftmsk = MASKLEFT[xs_bits & ALIGN_MASK];	/* mask for lefthand tail */

	char_row_ptr = char_ptr;
	ax_bits = ax << BPP_SHIFT;

	/* Righthand Edge of Cursor */
	tmp = xs_bits + ax_bits - 1;	/* bit offset from frmbuf's left margin */
	rgtmsk = ~MASKLEFT[(tmp & ALIGN_MASK) + 1];	/* mask for righthand tail */

	frame_ptr = ADRS(ys, xs_aligned);

	shift = (xs_bits & ALIGN_MASK);

	if((xs_bits & ALIGN_MASK) == 0)
	{	/* n * long (aligned) + rest within 1 long (aligned) */
		while(--ay >= 0)
		{
			SETUP();

			while(ax_bits > BITS_PER_UNIT)
			{
				GET_CHAR_REG(BITS_PER_UNIT);
				ax_bits -= BITS_PER_UNIT;
				*fp++ = char_reg;
			}

			work_reg = *fp & rgtmsk;
			GET_CHAR_REG(ax_bits);
			CHAR_1LONG(fp, work_reg);

			NEXT_ROW();
		}
	}
	else
	{
		if(ax_bits <= BITS_PER_UNIT)
		{
			if((xs_bits / BITS_PER_UNIT) ==
			   ((xs_bits + ax_bits - 1) / BITS_PER_UNIT))
			{					/* only rest within 1 long (unaligned) */
				while(--ay >= 0)
				{
					SETUP();

					work_reg |= *fp & rgtmsk;
					GET_CHAR_REG(ax_bits);
					CHAR_1LONG(fp, work_reg);

					NEXT_ROW();
				}
			}
			else
			{	/* only rest over 2 longs (unaligned) */
				while(--ay >= 0)
				{	/* loop over rows */
					SETUP();

					GET_CHAR_REG(ax_bits);
					CHAR_2LONGS(fp, work_reg);
					NEXT_ROW();
				}
			}
		}
		else
		{
			if((xs_bits / BITS_PER_UNIT) ==
			   ((xs_bits + ((ax_bits - 1) & ALIGN_MASK)) / BITS_PER_UNIT))
			{	/* n * long (unaligned) + rest within 1 long (unaligned) */
				while(--ay >= 0)
				{
					SETUP();

					while(ax_bits > BITS_PER_UNIT)	/* loop within one row */
					{
						GET_CHAR_REG(BITS_PER_UNIT);
						ax_bits -= BITS_PER_UNIT;
						work_reg |= fb_shr(char_reg, shift);
						*fp++ = work_reg;
						work_reg = fb_shl(char_reg, (BITS_PER_UNIT - shift));
					}

					GET_CHAR_REG(ax_bits);
					work_reg |= *fp & rgtmsk;
					CHAR_1LONG(fp, work_reg);
					NEXT_ROW();
				}
			}
			else
			{	/* n * long (unaligned) + rest over 2 longs (unaligned ) */
				while(--ay >= 0)
				{
					SETUP();

					while(ax_bits > BITS_PER_UNIT)	/* loop within one row */
					{
						GET_CHAR_REG(BITS_PER_UNIT);

						ax_bits -= BITS_PER_UNIT;
						work_reg |= fb_shr(char_reg, shift);
						*fp++ = work_reg;
						work_reg = fb_shl(char_reg, (BITS_PER_UNIT - shift));
					}

					GET_CHAR_REG(ax_bits);
					CHAR_2LONGS(fp, work_reg);
					NEXT_ROW();
				}
			}
		}
	}
}

/*
************************************************************
**
** Entry points
**
************************************************************
*/

/*
************************************************************
** initialize hardware, handle font
************************************************************
*/
void screen_init(
	unsigned int *fcol, 
	unsigned int *bcol, 
	unsigned int *nlines,
	unsigned int *nclms)
{
	struct ScreenDev *pSB = GetScreenDev();

	pSB->pFontHead = GetFntHeadAddr();
	pSB->pFont     = GetFntPtnAddr();

	/* initialize raster graphics hardware */
	raster_init(pSB, fcol, bcol);

	/* clear screen */
	raster_clear(pSB, *bcol, 0, 0, pSB->PlnSizX, pSB->PlnSizY);

	/*
	** Calculate size (in characters) of the terminal window based on
	** the size of the font and the graphical resolution
	*/
	*nclms  = pSB->PlnSizX / FNTHEAD->width;
	*nlines = pSB->PlnSizY / FNTHEAD->height;
}


/*
************************************************************
**  screen_curset(): set block or dash cursor
************************************************************
*/
void screen_curset(struct curdef *cursor)
{
	struct ScreenDev *pSB = GetScreenDev();
	int curcol = cursor->FGndCol ^ cursor->BGndCol;
	int x_start, y_start, delta_x, delta_y;

	x_start = cursor->clm * FNTHEAD->width;
	y_start = cursor->lin * FNTHEAD->height;
	delta_x = FNTHEAD->width;
	delta_y = FNTHEAD->height;
	raster_frct(pSB, curcol, x_start, y_start, delta_x, delta_y);
}


/*
************************************************************
** screen_curclr(): clear cursor
************************************************************
*/
void screen_curclr(struct curdef *cursor)
{
	screen_curset(cursor);
}

/*
************************************************************
**  screen_cpylns(): copy a number of lines from source to destination
************************************************************
*/
void screen_cpylns(
	int from_lin,			/* first line to be copied */
	int to_lin,				/* first line of destination area */
	int nr_of_lins)			/* number of lines to be copied */
{
	struct ScreenDev *pSB = GetScreenDev();
	int from, to;
	int dy, dy1;

	if(nr_of_lins)				/* if more than zero lines */
	{
		if(nr_of_lins < 0)		/* standardize to positive nr_of_lines */
		{
			nr_of_lins = -nr_of_lins;
			from_lin -= nr_of_lins + 1;	/* goto other edge! */
			to_lin -= nr_of_lins + 1;
		}

		/*
		** transform line numbers to raster coordinates
		** size of area in units of raster lines
		*/
		dy = nr_of_lins * FNTHEAD->height;
		from = from_lin * FNTHEAD->height;
		to =   to_lin   * FNTHEAD->height;
		dy1 = dy - 1;			/* delta y */

		/* if TO is within [FROM,FROM+DY] then change the direction */
		if(((from - to) ^ (from + dy - to)) < 0)
		{
			to = to + dy1;
			from = from + dy1;
			dy1 = -dy;
		}
		/* call hardware copy function */
		raster_vcopy(pSB, 0, from, pSB->PlnSizX, dy1, to);
	}
}


/*
************************************************************
** screen_erase(): clear a number of lines and columns
************************************************************
*/
void screen_erase(
	int from_lin,			/* first line to be erased */
	int from_clm,			/* first col to be erased */
	int nr_of_lins,			/* number of lines to be erased */
	int nr_of_clms,			/* number of columns to be erased */
	unsigned int BGndCol)
{
	struct ScreenDev *pSB = GetScreenDev();
	int line, column;
	int delta_line, delta_column;

	if(nr_of_clms && nr_of_lins)	/* if more than 0 columns and lines */
	{
		/* standardize to positive nr_of_lines and nr_of_columns */
		if(nr_of_clms < 0)
		{
			nr_of_clms = -nr_of_clms;
			from_clm -= nr_of_clms + 1;	/* goto other edge! */
		}

		if(nr_of_lins < 0)
		{
			nr_of_lins = -nr_of_lins;
			from_lin -= nr_of_lins + 1;	/* goto other edge! */
		}

		/* transform line and column number to raster coordinates */
		line = from_lin * FNTHEAD->height;
		column = from_clm * FNTHEAD->width;

		delta_line = nr_of_lins * FNTHEAD->height;
		delta_column = nr_of_clms * FNTHEAD->width;

		raster_clear(pSB, BGndCol, column, line, delta_column, delta_line);
	}
}


/*
************************************************************
** screen_chrlow(): print one character from the first characterset
**
** Note: blink attribute not supported
************************************************************
*/
void screen_chrlow(
	unsigned char charcode,
	struct curdef *cursor)
{
	struct ScreenDev *pSB = GetScreenDev();
	const unsigned char *ptnpointer;	/* ptr to actual character's pattern */
	unsigned int BGndCol, FGndCol, TempCol;	/* to exchange colors */
	int curxpos, curypos;

	if((charcode -= FNTHEAD->firstcode) >= FNTHEAD->fontlength)
		charcode = FNTHEAD->fontlength - 1;
	ptnpointer = (pSB->pFont + charcode * FNTHEAD->ptnsize);

	FGndCol = cursor->FGndCol;
	BGndCol = cursor->BGndCol;

	if(cursor->reverse)
	{
		TempCol = BGndCol;
		BGndCol = FGndCol;
		FGndCol = TempCol;
	}

	curxpos = cursor->clm * FNTHEAD->width;
	curypos = cursor->lin * FNTHEAD->height;

	raster_print(pSB, ptnpointer, FNTHEAD->rowsize, curxpos, curypos,
			FNTHEAD->width, FNTHEAD->height, FGndCol, BGndCol);
}

/*
************************************************************
** screen_bell(): ring the terminal bell
************************************************************
*/
void screen_bell(void)
{	/* currently not supported -- some day we might add  visible bell here */
}

#endif /* CONFIG_FRAMEBUFFER */
