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


#ifdef CONFIG_KEYBOARD


#include "keyboard.h"

/*
** Keyboard map for Shannon (aka Tuxscreen) infrared keyboard
*/

/*
** Status translation table
**
** Used to combine the status of the left & right shift, control
** and alt keys (6 keys) into a 3 bit value determining the logical
** status of shift, control and alt:
*/
static const unsigned char T_STATETRANS[] =
{		/*  SAC  rs ls ra la rc lc */
	0,	/* %000   -  -  -  -  -  - */
	1,	/* %001   -  -  -  -  -  + */
	1,	/* %001   -  -  -  -  +  - */
	1,	/* %001   -  -  -  -  +  + */
	2,	/* %010   -  -  -  +  -  - */
	3,	/* %011   -  -  -  +  -  + */
	3,	/* %011   -  -  -  +  +  - */
	3,	/* %011   -  -  -  +  +  + */
	2,	/* %010   -  -  +  -  -  - */
	3,	/* %011   -  -  +  -  -  + */
	3,	/* %011   -  -  +  -  +  - */
	3,	/* %011   -  -  +  -  +  + */
	2,	/* %010   -  -  +  +  -  - */
	3,	/* %011   -  -  +  +  -  + */
	3,	/* %011   -  -  +  +  +  - */
	3,	/* %011   -  -  +  +  +  + */
	4,	/* %100   -  +  -  -  -  - */
	5,	/* %101   -  +  -  -  -  + */
	5,	/* %101   -  +  -  -  +  - */
	5,	/* %101   -  +  -  -  +  + */
	6,	/* %110   -  +  -  +  -  - */
	7,	/* %111   -  +  -  +  -  + */
	7,	/* %111   -  +  -  +  +  - */
	7,	/* %111   -  +  -  +  +  + */
	6,	/* %110   -  +  +  -  -  - */
	7,	/* %111   -  +  +  -  -  + */
	7,	/* %111   -  +  +  -  +  - */
	7,	/* %111   -  +  +  -  +  + */
	6,	/* %110   -  +  +  +  -  - */
	7,	/* %111   -  +  +  +  -  + */
	7,	/* %111   -  +  +  +  +  - */
	7,	/* %111   -  +  +  +  +  + */
	4,	/* %100   +  -  -  -  -  - */
	5,	/* %101   +  -  -  -  -  + */
	5,	/* %101   +  -  -  -  +  - */
	5,	/* %101   +  -  -  -  +  + */
	6,	/* %110   +  -  -  +  -  - */
	7,	/* %111   +  -  -  +  -  + */
	7,	/* %111   +  -  -  +  +  - */
	7,	/* %111   +  -  -  +  +  + */
	6,	/* %110   +  -  +  -  -  - */
	7,	/* %111   +  -  +  -  -  + */
	7,	/* %111   +  -  +  -  +  - */
	7,	/* %111   +  -  +  -  +  + */
	6,	/* %110   +  -  +  +  -  - */
	7,	/* %111   +  -  +  +  -  + */
	7,	/* %111   +  -  +  +  +  - */
	7,	/* %111   +  -  +  +  +  + */
	4,	/* %100   +  +  -  -  -  - */
	5,	/* %101   +  +  -  -  -  + */
	5,	/* %101   +  +  -  -  +  - */
	5,	/* %101   +  +  -  -  +  + */
	6,	/* %110   +  +  -  +  -  - */
	7,	/* %111   +  +  -  +  -  + */
	7,	/* %111   +  +  -  +  +  - */
	7,	/* %111   +  +  -  +  +  + */
	6,	/* %110   +  +  +  -  -  - */
	7,	/* %111   +  +  +  -  -  + */
	7,	/* %111   +  +  +  -  +  - */
	7,	/* %111   +  +  +  -  +  + */
	6,	/* %110   +  +  +  +  -  - */
	7,	/* %111   +  +  +  +  -  + */
	7,	/* %111   +  +  +  +  +  - */
	7	/* %111   +  +  +  +  +  + */
};

/*
** Compose Table:
**
** This allows to enter characters from the international character set.
**
** for instance: entering <Compose-key>-'a'-'"' will issue 'ä'.
**
** The following table contains one entry for each possible compose
** character with the two characters it is composed from:
*/
const static struct compose_char T_COMPOSE[] =
{
   {{'!','!'}, 0xa1},
   {{'c','/'}, 0xa2},
   {{'C','/'}, 0xa2},
   {{'l','-'}, 0xa3},
   {{'L','-'}, 0xa3},
   {{'y','='}, 0xa5},
   {{'Y','='}, 0xa5},
   {{'s','o'}, 0xa7},
   {{'S','o'}, 0xa7},
   {{'s','O'}, 0xa7},
   {{'S','O'}, 0xa7},
   {{'x','o'}, 0xa8},
   {{'X','o'}, 0xa8},
   {{'x','O'}, 0xa8},
   {{'X','O'}, 0xa8},
   {{'c','o'}, 0xa9},
   {{'C','o'}, 0xa9},
   {{'c','O'}, 0xa9},
   {{'C','O'}, 0xa9},
   {{'a','_'}, 0xaa},
   {{'A','_'}, 0xaa},
   {{'<','<'}, 0xab},
   {{'0','^'}, 0xb0},
   {{'+','-'}, 0xb1},
   {{'^','2'}, 0xb2},
   {{'^','3'}, 0xb3},
   {{'/','u'}, 0xb5},
   {{'p','!'}, 0xb6},
   {{'P','!'}, 0xb6},
   {{'^','.'}, 0xb7},
   {{'^','1'}, 0xb9},
   {{'o','_'}, 0xba},
   {{'O','_'}, 0xba},
   {{'>','>'}, 0xbb},
   {{'1','4'}, 0xbc},
   {{'1','2'}, 0xbd},
   {{'?','?'}, 0xbf},
   {{'A','`'}, 0xc0},
   {{'A','\''}, 0xc1},
   {{'A','^'}, 0xc2},
   {{'A','~'}, 0xc3},
   {{'A','"'}, 0xc4},
   {{'A','*'}, 0xc5},
   {{'A','E'}, 0xc6},
   {{'a','E'}, 0xc6},
   {{'C',','}, 0xc7},
   {{'E','`'}, 0xc8},
   {{'E','\''}, 0xc9},
   {{'E','"'}, 0xcb},
   {{'I','`'}, 0xcc},
   {{'I','\''}, 0xcd},
   {{'I','^'}, 0xce},
   {{'I','"'}, 0xcf},
   {{'N','~'}, 0xd1},
   {{'O','`'}, 0xd2},
   {{'O','\''}, 0xd3},
   {{'O','^'}, 0xd4},
   {{'O','~'}, 0xd5},
   {{'O','"'}, 0xd6},
   {{'O','E'}, 0xd7},
   {{'o','E'}, 0xf7},
   {{'O','/'}, 0xd8},
   {{'U','`'}, 0xd9},
   {{'U','\''}, 0xda},
   {{'U','^'}, 0xdb},
   {{'U','"'}, 0xdc},
   {{'Y','"'}, 0xdd},
   {{'s','s'}, 0xdf},
   {{'S','s'}, 0xdf},
   {{'s','S'}, 0xdf},
   {{'S','S'}, 0xdf},
   {{'a','`'}, 0xe0},
   {{'a','\''}, 0xe1},
   {{'a','^'}, 0xe2},
   {{'a','~'}, 0xe3},
   {{'a','"'}, 0xe4},
   {{'a','*'}, 0xe5},
   {{'a','e'}, 0xe6},
   {{'A','e'}, 0xe6},
   {{'c',','}, 0xe7},
   {{'e','`'}, 0xe8},
   {{'e','\''}, 0xe9},
   {{'e','"'}, 0xeb},
   {{'i','`'}, 0xec},
   {{'i','\''}, 0xed},
   {{'i','^'}, 0xee},
   {{'i','"'}, 0xef},
   {{'n','~'}, 0xf1},
   {{'o','`'}, 0xf2},
   {{'o','\''}, 0xf3},
   {{'o','^'}, 0xf4},
   {{'o','~'}, 0xf5},
   {{'o','"'}, 0xf6},
   {{'o','e'}, 0xf7},
   {{'O','e'}, 0xf7},
   {{'o','/'}, 0xf8},
   {{'u','`'}, 0xf9},
   {{'u','\''}, 0xfa},
   {{'u','^'}, 0xfb},
   {{'u','"'}, 0xfc},
   {{'y','"'}, 0xfd},
   {{'+','+'}, '#'},
   {{'a','a'}, '@'},
   {{'a','A'}, '@'},
   {{'A','a'}, '@'},
   {{'A','A'}, '@'},
   {{'(','('}, '['},
   {{'/','/'}, 0x5c},
   {{')',')'}, ']'},
   {{'-','('}, '{'},
   {{'/','^'}, '|'},
   {{')','-'}, '}'},
   {{'^',' '}, '^'},
   {{'^','^'}, '^'},
   {{'\'',' '}, '\''},
   {{'\'','\''}, '\''},
   {{'`',' '}, '`'},
   {{'`','`'}, '`'},
   {{0,0}, 0},  /* Endmarker!!! */
};

/*
** String definitions for sequence keys:
**
** NOTE: if the byte after the null-terminator is non-zero, it
**       is the alternate code issued by that key when the
**       keyboard is in numlock state.
**
**
** The strings defined here are appropriate for a DEC vt-220-type
** keyboard. Alternate numlock codes are assigned where useful.
*/
const static char _CUP[] = {'\033','[','A','\000','\000'};/* Cursor up, down, left, right */
const static char _CDW[] = {'\033','[','B','\000','\000'};
const static char _CRT[] = {'\033','[','C','\000','\000'};
const static char _CLF[] = {'\033','[','D','\000','\000'};

const static char _F1[] = {'\033','[','2','~','\000','\000'}; /* F1: Insert Here */
const static char _F2[] = {'\033','[','3','~','\000','\000'}; /* F2: Remove */
const static char _F3[] = {'\033','[','4','~','\000','\000'}; /* F3: Select */
const static char _F4[] = {'\033','[','5','~','\000','\000'}; /* F4: Prev. Screen */
const static char _F5[] = {'\033','[','6','~','\000','\000'}; /* F5: Next Screen */
const static char _F6[] = {'\033','[','1','7','~','\000','\000'}; /* F6 */
const static char _F7[] = {'\033','[','1','8','~','\000','\000'}; /* F7 */
const static char _F8[] = {'\033','[','1','9','~','\000','\000'}; /* F8 */
const static char _F9[] = {'\033','[','2','0','~','\000','\000'}; /* F9 */
const static char _F10[] = {'\033','[','2','1','~','\000','\000'}; /* F10 */
const static char _F11[] = {'\033','[','2','3','~','\000','\033'}; /* F11 <ESC> */
const static char _F12[] = {'\033','[','2','4','~','\000', 0x08 }; /* F12: <BS> */
const static char _F13[] = {'\033','[','2','5','~','\000','\012'}; /* F13: <LF> */

const unsigned char * const T_SEQSTRING[] =
{
	_CUP,   _CDW,   _CRT,   _CLF,	_F1,    _F2,    _F3,    _F4,
	_F5,    _F6,    _F7,    _F8,    _F9,    _F10,   _F11,   _F12,   _F13
};

#define _CUP_Index     0
#define _CDW_Index     1
#define _CRT_Index     2
#define _CLF_Index     3
#define _F1_Index      4
#define _F2_Index      5
#define _F3_Index      6
#define _F4_Index      7
#define _F5_Index      8
#define _F6_Index      9
#define _F7_Index     10
#define _F8_Index     11
#define _F9_Index     12
#define _F10_Index    13
#define _F11_Index    14
#define _F12_Index    15
#define _F13_Index    16

#define CUP SKEY(_CUP_Index)    /* Cursor up, down, left, right  */
#define CDW SKEY(_CDW_Index)
#define CRT SKEY(_CRT_Index)
#define CLF SKEY(_CLF_Index)
#define F01 SKEY(_F1_Index)     /* F1: Insert Here */
#define F02 SKEY(_F2_Index)     /* F2: Remove */
#define F03 SKEY(_F3_Index)     /* F2: Select */
#define F04 SKEY(_F4_Index)     /* F3: Prev. Screen */
#define F05 SKEY(_F5_Index)     /* F4: Next Screen */
#define F06 SKEY(_F6_Index)     /* F6 */
#define F07 SKEY(_F7_Index)     /* F7 */
#define F08 SKEY(_F8_Index)     /* F8 */
#define F09 SKEY(_F9_Index)     /* F9 */
#define F10 SKEY(_F10_Index)    /* F10 */
#define F11 SKEY(_F11_Index)    /* F11: <ESC> */
#define F12 SKEY(_F12_Index)    /* F12: <BS> */
#define F13 SKEY(_F13_Index)    /* F13: <LF> */

#define NUL	0x00
#define SDW	0x00
#define SUP	0x00
#define PSC	0x00
#define CSR	0x00
#define HOM	0x00
#define PGU	0x00
#define PGD	0x00
#define END	0x00
#define SDW	0x00

/* Some links, to get same addresses for different lables */
#define T_S_C T___C
#define T__AC T___C
#define T_SAC T___C
#define T_SA_ T___C
#define T__A_ T___C


const static unsigned short T____[] =
{
/*
** Conversion table for no control, shift, alt or prefix
**
**	0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
*/
	NUL,NUL,NUL,NUL,SDW,NUL,NUL,NUL,NUL,CPL,'x',F05,F01,'2','s','w',
	NUL,F03,'c',F04,F02,'3','d','e',LAL,TAB,'z',ESC,LCT,'1','a','q',
	'b','t','v','g','5','4','f','r',RAL,SUP,NUL,CMP,NUL,PSC,NUL,NML,
	NUL,LSH,RSH,NUL,NUL,NUL,NUL,NUL,SPC,BSP,ENT,F06,F09,F10,'\\',NUL,
	NUL,NUL,NUL,NUL,NUL,NUL,NUL,NUL,CLF,NUL,CSR,CUP,HOM,NUL,PGU,NUL,
	CDW,NUL,F13,NUL,RCT,F11,PGD,NUL,CRT,NUL,NUL,END,'`',F12,NUL,NUL,
	NUL,F07,'.',NUL,F08,'9','l','o','/','[',NUL,'\'','-','0',';','p',
	NUL,']',',',NUL,'=','8','k','i','n','y','m','h','6','7','j','u'
};

const static unsigned short T_S__[] =
{
/*
** Conversion table for shift
**  0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
**
*/
	NUL,NUL,NUL,NUL,SDW,NUL,NUL,NUL,NUL,CPL,'X',F05,F01,'@','S','W',
	NUL,F03,'C',F04,F02,'#','D','E',LAL,TAB,'Z',ESC,LCT,'!','A','Q',
	'B','T','V','G','%','$','F','R',RAL,SUP,NUL,CMP,NUL,PSC,NUL,NML,
	NUL,LSH,RSH,NUL,NUL,NUL,NUL,NUL,SPC,BSP,ENT,F06,F09,F10,'|',NUL,
	NUL,NUL,NUL,NUL,NUL,NUL,NUL,NUL,CLF,NUL,CSR,CUP,HOM,NUL,PGU,NUL,
	CDW,NUL,F13,NUL,RCT,F11,PGD,NUL,CRT,NUL,NUL,END,'~',F12,NUL,NUL,
	NUL,F07,'>',NUL,F08,'(','L','O','?','{',NUL,'\"','_',')',':','P',
	NUL,'}','<',NUL,'+','*','K','I','N','Y','M','H','^','&','J','U'
};

const static unsigned short T___C[] = /* T__S_C[] = T___AC[] = T__SAC[], */
{                  /* T__A_[] = T__SA_[] linked by #defines above        */
/*
** Conversion table for control, control-shift, control-alt
** control-shift-alt, alt and shift-alt
**
**  0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
*/
	NUL,NUL,NUL,NUL,SDW,NUL,NUL,NUL,NUL,CPL,0x18,F05,F01,0x00,0x13,0x17,
	NUL,F03,0x03,F04,F02,'3',0x04,0x05,LAL,TAB,0x1a,ESC,LCT,'1',0x01,0x11,
	0x02,0x14,0x16,0x07,'5','4',0x06,0x12,RAL,SUP,NUL,CMP,NUL,PSC,NUL,NML,
	NUL,LSH,RSH,NUL,NUL,NUL,NUL,NUL,SPC,BSP,ENT,F06,F09,F10,0x1c,NUL,
	NUL,NUL,NUL,NUL,NUL,NUL,NUL,NUL,CLF,NUL,CSR,CUP,HOM,NUL,PGU,NUL,
	CDW,NUL,F13,NUL,RCT,F11,PGD,NUL,CRT,NUL,NUL,END,0x1e,F12,NUL,NUL,
	NUL,F07,',',NUL,F08,'9',0x0c,0x0f,'/',0x1b,NUL,'\'',0x1f,'0',';',0x10,
	NUL,0x1d,'.',NUL,'=','8',0x0b,0x09,0x0e,0x19,0x0d,0x08,'6','7',0x0a,0x15
};

const struct keyboardmap kbd_header =
{
	128,					/* convtabsize */
	0x94,					/* makecode    */
	0xd4,					/* breakcode   */
	0xffffffff,				/* mkbrkmask   */
	&T_STATETRANS[0],		/* statustable */
	{						/* convtables  */
				/* Shift, Alt, Control  */
		T____,	/*   -     -     -      */
		T___C,	/*   -     -     +      */
		T__A_,	/*   -     +     -      */
		T__AC,	/*   -     +     +      */
		T_S__,	/*   +     -     -      */
		T_S_C,	/*   +     -     +      */
		T_SA_,	/*   +     +     -      */
		T_SAC,	/*   +     +     +      */
	},
	&T_SEQSTRING[0],		/* seqstring    */
    &T_COMPOSE[0]			/* composetable */
};

const struct maptable kbd_mapping_table[] =
{
	{ "us", &kbd_header },	/* shannon only has a single mapping (us) */
	{ 0, 0 }				/* end marker !! */
};

#endif /* CONFIG_KEYBOARD */
