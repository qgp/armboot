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

#ifdef CONFIG_ANSI_CONSOLE

/*
************************************************************
**
**  Simple ansi terminal interpreter
**
************************************************************
*/

/*
** This code covers the state machine part of a simple ansi
** terminal emulation. It uses a bunch of device dependent
** functions to do the actual output to screen.
*/

#include "cursor.h"

/*
** allocation limits                                                         
*/
/* max. number of numeric arguments in sequence; NPARAMS(+1) is ...  */
#define NPARAMS 2	/* ... the size of param[] */

/*
** structure storing parameters from ansi-sequence
*/
struct param_def
{
	int  parablock[NPARAMS + 1];	/* buffer for numeric arguments   */
	int  prmidx;					/* index of current argument      */
	char last_char;					/* character terminating sequence */
};

/*
** some enumerated types
*/
enum argstate
{
	NO_ARG,
	IN_ARG
};

enum linestate
{
	LS_INLINE,
	LS_EOL
};

enum awstate
{
	AW_OFF,
	AW_ON
};


/*
************************************************************
** External screen driver functions:
************************************************************
*/


extern void screen_init(
	unsigned int *fcol,
	unsigned int *bcol,
	unsigned int *NoOfLins,
	unsigned int *NoOfClms);

extern void screen_curset(struct curdef *curs);
extern void screen_curclr(struct curdef *curs);
extern void screen_cpylns(int from, int to, int count);
extern void screen_erase(
	int froml,
	int fromc,
	int nl,
	int nc,
	unsigned int bgcol);
extern void screen_bell(void);
extern void screen_chrlow(unsigned char c, struct curdef *curs);

/*
** stateblock
*/
struct state
{
	unsigned int nlines, nclms;	/* number of lines and columns     */
	struct selector  *current;	/* the actual machine state        */
	enum argstate    subcsi;	/* csi-arg input has begun         */
	enum awstate autowrap;
	struct param_def param;		/* parameter (arguments) structure */
	struct curdef    cursor;	/* cursor defintion record         */
	enum linestate eol;
};

/*
** machine state selector
*/
struct selector
{
	unsigned char from, to;
	void (*funct) (struct state *, unsigned char);
};

/*
************************************************************
** static storage requirements
************************************************************
*/
static struct state ansi_stateblock;
#define GetStateBlockAddr()	(&ansi_stateblock)

/*
************************************************************
** Local functions, to be used by the state machine
************************************************************
*/

static void x_setup(struct state *, unsigned int, unsigned int);
static void x_exe(struct state *, unsigned char);
static void x_null(struct state *, unsigned char);
static void x_stnrm(struct state *, unsigned char);
static void x_stcsi(struct state *, unsigned char);
static void x_stesc(struct state *, unsigned char);
static void x_stprv(struct state *, unsigned char);
static void x_stbad(struct state *, unsigned char);
static void x_argnxt(struct state *, unsigned char);
static void x_argdig(struct state *, unsigned char);
static void x_indfwd(struct state *, unsigned char);
static void x_newlin(struct state *, unsigned char);
static void x_bs(struct state *, unsigned char);
static void x_bell(struct state *, unsigned char);
static void x_tab(struct state *, unsigned char);
static void x_lf(struct state *, unsigned char);
static void x_cr(struct state *, unsigned char);
static void x_curup(struct state *, unsigned char);
static void x_curdwn(struct state *, unsigned char);
static void x_curfwd(struct state *, unsigned char);
static void x_curbak(struct state *, unsigned char);
static void x_curpos(struct state *, unsigned char);
static void x_eradis(struct state *, unsigned char);
static void x_eralin(struct state *, unsigned char);
static void x_rendit(struct state *, unsigned char);
static void x_chargl(struct state *, unsigned char);
static void x_prvon(struct state *, unsigned char);
static void x_prvoff(struct state *, unsigned char);


/*
************************************************************
** State tables
************************************************************
*/

/*
** Control code functions
*/
static const struct selector st_control[] = {
	{' ', '~', x_exe},			/* normal processing                  */
	{'\x07', '\x07', x_bell},	/* BELL                               */
	{'\x08', '\x08', x_bs},		/* BS: non-destructive backspace      */
	{'\x09', '\x09', x_tab},	/* TAB: go to next tabulator position */
	{'\x0a', '\x0a', x_lf},		/* LF: newline                        */
	{'\x0d', '\x0d', x_cr},		/* CR: first column                   */
	{'\x1b', '\x1b', x_stesc},	/* ESC: enter ESC-sequence            */
	{'\x9b', '\x9b', x_stcsi},	/* CSI: enter CSI-sequence            */
	{'\x00', '\xff', x_null}	/* ignore others by default           */
};

/*
** ESC functions
*/
static const struct selector st_esc[] = {
	{'[', '[', x_stcsi},		/* enter CSI-Sequence                 */
	{'\x00', '\xff', x_stnrm}	/* default: abort sequence            */
};

/*
** CSI functions
*/
static const struct selector st_csi[] = {
	{'A', 'A', x_curup},		/* cursor up                          */
	{'B', 'B', x_curdwn},		/* cursor down                        */
	{'C', 'C', x_curfwd},		/* cursor forward                     */
	{'D', 'D', x_curbak},		/* cursor backward                    */
	{'H', 'H', x_curpos},		/* cursor position addressing         */
	{'J', 'J', x_eradis},		/* erase in display                   */
	{'K', 'K', x_eralin},		/* erase in line                      */
	{'m', 'm', x_rendit},		/* set character rendition            */
	{'?', '?', x_stprv},		/* start private sequence             */
	{';', ';', x_argnxt},		/* next csi argument                  */
	{'0', '9', x_argdig},		/* next digit of csi argument         */
	{'\x00', '\xff', x_stbad}	/* default: abort with next alpha     */
};

/*
** Private functions
*/
static const struct selector st_prv[] = {
	{'h', 'h', x_prvon},		/* turn on private attribute          */
	{'l', 'l', x_prvoff},		/* turn off private attribute         */
	{'0', '9', x_argdig},		/* next digit of argument parameter   */
	{';', ';', x_argnxt},		/* next prv argument                  */
	{'\x00', '\xff', x_stbad}	/* default: abort with next alpha     */
};

/*
** Error status: ignore all until 0x20..0x7e received
*/
static const struct selector st_bad[] = {
	{' ', '~', x_stnrm},		/* abort sequence with next alpha     */
	{'\x00', '\xff', x_null}	/* ignore others                      */
};

/*
** Normal (not-in-sequence) functions
*/
static const struct selector st_nrm[] = {
	{' ', '~', x_chargl},		/* printing normal codes              */
	{'\x00', '\xff', x_null}	/* ignore others                      */
};

/*
** Supported CSI arguments
*/
#define P_CURVIS	25
#define P_AUTOWRAP	7

/*
************************************************************
**  Some commonly used macros
**
**  The local stateblock pointer (pSB) must be be set up
**  before using any of these.
************************************************************
*/

#define NoOfLins (pSB->nlines)
#define NoOfClms (pSB->nclms)

#define ScrollTop 1
#define ScrollBtm NoOfLins


#define CURDLTD() (pSB->cursor.vis = CS_OFF)

#define CURPOS(y,x) (pSB->cursor.lin = (y), pSB->cursor.clm = (x))

#define CLRCUR() \
{\
  if (pSB->cursor.vis == CS_ON  &&  pSB->cursor.abl == CS_ON)\
  {\
    screen_curclr (&pSB->cursor);\
    CURDLTD ();\
  }\
}

#define SETCUR() \
{\
  if (pSB->cursor.vis == CS_OFF  &&  pSB->cursor.abl == CS_ON)\
  {\
    screen_curset (&pSB->cursor);\
    pSB->cursor.vis = CS_ON;\
  }\
}

#define DEFAULT(x,y) \
{\
  if((pSB->param.prmidx < (x)) || (pSB->param.parablock[(x)] == 0))\
    pSB->param.parablock[(x)] = (y);\
}

#define ENDSEQ() (pSB->current = (struct selector *) st_nrm)

#define CLREOL() (pSB->eol = LS_INLINE)
#define SETEOL() (pSB->eol = LS_EOL)

/*
************************************************************
**  Main interpreter entrypoints:
************************************************************
*/

/*
************************************************************
**  ansi_console_init(): hardware initialization
************************************************************
*/

void ansi_console_init(void)
{
	struct state *pSB = GetStateBlockAddr();
	unsigned int fcol, bcol;

	screen_init(&fcol, &bcol, &NoOfLins, &NoOfClms);
	x_setup(pSB, fcol, bcol);
}

/*
************************************************************
** ansi_console_putc(): output character on screen
************************************************************
*/
void ansi_console_putc(const char c)
{
	/*
	** search character in table and call function from table
	*/
	struct state *pSB = GetStateBlockAddr();
	struct selector *table = (struct selector *)st_control;

	while((unsigned char)c < table->from || (unsigned char)c > table->to)
		table++;

	(*table->funct)(pSB, (unsigned char)c);
	
	if (c == '\n')
		ansi_console_putc('\r');
	SETCUR();
}

/*
************************************************************
** Local functions
************************************************************
*/

#define I_CURABL    CS_ON
#define I_CURVIS    CS_OFF
#define I_AWSTAT    AW_ON
#define I_EOLSTAT   LS_INLINE
#define I_REVERSE   0

/*
**  x_setup(): initialize static parameters
*/
static void x_setup(
	struct state *pSB,
	unsigned int fcol,
	unsigned int bcol)
{

	pSB->current = (struct selector *)st_nrm;

	pSB->subcsi = NO_ARG;

	pSB->cursor.FGndCol = fcol;
	pSB->cursor.BGndCol = bcol;

	pSB->cursor.abl     = I_CURABL;
	pSB->cursor.vis     = I_CURVIS;
	pSB->cursor.reverse = I_REVERSE;

	CURPOS(0, 0);

	pSB->autowrap = I_AWSTAT;
	pSB->eol = I_EOLSTAT;
}

/*
** x_exe(): state processor
*/
static void x_exe(struct state *pSB, unsigned char c)
{
	struct selector *table = pSB->current;

	while(c < table->from || c > table->to)
		table++;

	(*table->funct)(pSB, c);
}


/*
************************************************************
**  state switching routines
************************************************************
*/

/*
** x_null(): ignore this char - state unchanged
*/
static void x_null(struct state *pSB, unsigned char c)
{
	/* no-op */
}

/*
** x_stnrm(): abort current sequence - enter normal state
*/
static void x_stnrm(struct state *pSB, unsigned char c)
{
	ENDSEQ();
}

/*
** x_stcsi(): enter CSI-sequence
*/
static void x_stcsi(struct state *pSB, unsigned char c)
{
	pSB->current = (struct selector *)st_csi;
	pSB->param.parablock[pSB->param.prmidx = 0] = 0;	/* reset argument input */
	pSB->subcsi = NO_ARG;								/* mark 'no argument up to now' */
}

/*
** x_stesc(): enter escape-sequence
*/
static void x_stesc(struct state *pSB, unsigned char c)
{
	pSB->current = (struct selector *)st_esc;
}

/*
** x_stprv(): enter private sequence (CSI - ?)
*/
static void x_stprv(struct state *pSB, unsigned char c)
{
	if(pSB->subcsi == NO_ARG)
	{
		pSB->current = (struct selector *)st_prv;
		pSB->param.parablock[pSB->param.prmidx = 0] = 0;
	}
	else
		ENDSEQ();
}

/*
** x_stbad(): enter error-in-sequence state
*/
static void x_stbad(struct state *pSB, unsigned char c)
{
	pSB->current = (struct selector *)st_bad;
}

/*
************************************************************
**  Argument input routines
************************************************************
*/

/*
** x_argnxt(): next argument
*/
static void x_argnxt(struct state *pSB, unsigned char c)
{
	pSB->subcsi = IN_ARG;
	if(pSB->param.prmidx < NPARAMS)
		pSB->param.parablock[++pSB->param.prmidx] = 0;
}

/*
** x_argdig(): next digit
*/
static void x_argdig(struct state *pSB, unsigned char d)
{
	int *p = pSB->param.parablock + pSB->param.prmidx;

	pSB->subcsi = IN_ARG;
	*p = *p * 10 + (d & '\x0f');
}

/*
** x_indfwd(): forward index, scroll at scroll.bot
*/
static void x_indfwd(struct state *pSB, unsigned char c)
{
	if(pSB->cursor.lin == (ScrollBtm - 1))
	{
		CLRCUR();				/* avoid copying the cursor */
		screen_cpylns((ScrollTop - 1) + 1,	/* from line */
				 ScrollTop - 1,	/* to line */
				 ScrollBtm - ScrollTop);	/* size in lines */

		screen_erase(ScrollBtm - 1, 0,	/* from line / column */
				1, NoOfClms,	/* size lines * columns */
				pSB->cursor.BGndCol);	/* erase color */
	}
	else if(pSB->cursor.lin < NoOfLins)
	{
		CLRCUR();
		++pSB->cursor.lin;
	}

	ENDSEQ();
	CLREOL();
}

/*
** x_newlin(): nel: newline
*/
static void x_newlin(struct state *pSB, unsigned char c)
{
	x_cr(pSB, 0);
	x_indfwd(pSB, 0);
	if((c & 0x60) != 0)			/* do no endsequence in case of C1 */
		ENDSEQ();
}

/*
************************************************************
**  c0 control code execution: cr, lf, bs, tab etc.
************************************************************
*/

/*
** x_bell(): activate beeper
*/
static void x_bell(struct state *pSB, unsigned char c)
{
	screen_bell();
}

/*
** x_bs(): backspace, non-destructive, non-wrap
*/
static void x_bs(struct state *pSB, unsigned char c)
{
	if((pSB->cursor.clm > 0))
	{
		CLRCUR();
		--pSB->cursor.clm;
	}
	CLREOL();
}

/*
** x_tab(): handle TAB character
*/
static void x_tab(struct state *pSB, unsigned char c)
{
}

/*
** x_lf(): linefeed, scroll at scroll.bot
*/
static void x_lf(struct state *pSB, unsigned char c)
{
	x_indfwd(pSB, 0);
}

/*
** x_cr(): carriage return
*/
static void x_cr(struct state *pSB, unsigned char c)
{
	if(pSB->cursor.clm != 0)
	{
		CLRCUR();
		pSB->cursor.clm = 0;
	}

	CLREOL();
}

/*
************************************************************
**  csi-execution routines
************************************************************
*/

/*
** x_curup(): cursor n lines up, no wrap, no scroll, stops at scroll.top
*/
static void x_curup(struct state *pSB, unsigned char c)
{
	unsigned long maxup = pSB->cursor.lin;

	if(maxup)
	{
		CLRCUR();
		DEFAULT(0, 1);
		if(pSB->param.parablock[0] >= maxup)
			pSB->cursor.lin -= maxup;
		else
			pSB->cursor.lin -= pSB->param.parablock[0];
	}

	ENDSEQ();
	CLREOL();
}

/*
** x_curdwn(): cursor n lines down, no wrap, no scroll, stops at scroll.bot
*/
static void x_curdwn(struct state *pSB, unsigned char c)
{
	unsigned long maxdwn;

	if(pSB->cursor.lin <= (ScrollBtm - 1))
		maxdwn = ScrollBtm - pSB->cursor.lin - 1;
	else
		maxdwn = NoOfLins - pSB->cursor.lin - 1;

	if(maxdwn)
	{
		DEFAULT(0, 1);
		CLRCUR();

		if(pSB->param.parablock[0] >= maxdwn)
			pSB->cursor.lin += maxdwn;
		else
			pSB->cursor.lin += pSB->param.parablock[0];
	}

	ENDSEQ();
	CLREOL();
}

/*
** x_curfwd(): cursor n columns forward, no wrap, no scroll
*/
static void x_curfwd(struct state *pSB, unsigned char c)
{
	if(pSB->cursor.clm < (NoOfClms - 1))
	{
		DEFAULT(0, 1);
		CLRCUR();
		if((pSB->cursor.clm += pSB->param.parablock[0]) > (NoOfClms - 1))
			pSB->cursor.clm = NoOfClms - 1;
	}

	ENDSEQ();
	CLREOL();
}

/*
** x_curbak(): cursor n columns backward, no wrap, no scroll
*/

static void x_curbak(struct state *pSB, unsigned char c)
{
	if(pSB->cursor.clm > 0)
	{
		DEFAULT(0, 1);
		CLRCUR();
		if(pSB->cursor.clm < pSB->param.parablock[0])
			pSB->cursor.clm = 0;
		else
			pSB->cursor.clm -= pSB->param.parablock[0];
	}

	ENDSEQ();
	CLREOL();
}

/*
** x_curpos(): cursor position addressing, regular ceiling
*/
static void x_curpos(struct state *pSB, unsigned char c)
{
	int top, bot;

	DEFAULT(0, 1);
	--pSB->param.parablock[0];
	DEFAULT(1, 1);
	--pSB->param.parablock[1];

	top = 0;
	bot = NoOfLins;

	pSB->param.parablock[0] += top;

	if(pSB->param.parablock[0] > bot)
		pSB->param.parablock[0] = bot - 1;
	if(pSB->param.parablock[1] > NoOfClms)
		pSB->param.parablock[1] = NoOfClms - 1;
	if(pSB->param.parablock[0] != pSB->cursor.lin ||
	   pSB->param.parablock[1] != pSB->cursor.clm)
	{
		CLRCUR();
		CURPOS(pSB->param.parablock[0], pSB->param.parablock[1]);
	}

	ENDSEQ();
	CLREOL();
}

/*
** x_eradis(): erase in display
*/
static void x_eradis(struct state *pSB, unsigned char c)
{
	screen_erase(pSB->cursor.lin, pSB->cursor.clm,
			1, (NoOfClms - 1) - pSB->cursor.clm + 1, pSB->cursor.BGndCol);
	if(pSB->cursor.lin < (NoOfLins - 1))
		screen_erase(pSB->cursor.lin + 1, 0,
				NoOfLins - 1 - pSB->cursor.lin,
				NoOfClms, pSB->cursor.BGndCol);

	CURDLTD();
	ENDSEQ();
	CLREOL();
}

/*
** x_eralin(): erase in line
*/
static void x_eralin(struct state *pSB, unsigned char c)
{
	screen_erase(pSB->cursor.lin, pSB->cursor.clm,
			1, (NoOfClms - 1) - pSB->cursor.clm + 1, pSB->cursor.BGndCol);

	CURDLTD();
	ENDSEQ();
	CLREOL();
}

/*
** x_rendit(): set character rendition
*/
static void x_rendit(struct state *pSB, unsigned char c)
{
	int i, *p;

	for(i = 0, p = pSB->param.parablock; i <= pSB->param.prmidx; ++i, ++p)
		if(*p >= 1 && *p <= 19)
			pSB->cursor.reverse = 1;
		else
			pSB->cursor.reverse = 0;

	ENDSEQ();
}

/*
** x_chargl(): character, gl set
*/
static void x_chargl(struct state *pSB, unsigned char c)
{
	/* AUTOWRAP start */
	if(pSB->eol == LS_EOL)
	{
		x_newlin(pSB, 0);
		CLREOL();
	}

	/* AUTOWRAP end */
	screen_chrlow(c, &pSB->cursor);
	CURDLTD();

	if(pSB->cursor.clm < (NoOfClms - 1))
		++pSB->cursor.clm;

	/* AUTOWRAP start */
	else if(pSB->autowrap == AW_ON)
		SETEOL();

	/* AUTOWRAP start */
}

/*
** x_prvon(): set private features
*/
static void x_prvon(struct state *pSB, unsigned char c)
{
	switch (pSB->param.parablock[0])
	{
		case P_CURVIS:
			pSB->cursor.abl = CS_ON;
			break;

		case P_AUTOWRAP:
			pSB->autowrap = AW_ON;
			break;
	}
	ENDSEQ();
}

/*
** x_prvoff(): reset private features
*/
static void x_prvoff(struct state *pSB, unsigned char c)
{
	switch (pSB->param.parablock[0])
	{
		case P_CURVIS:
			CLRCUR();
			pSB->cursor.abl = CS_OFF;
			break;

		case P_AUTOWRAP:
			CLREOL();
			pSB->autowrap = AW_OFF;
			break;
	}

	ENDSEQ();
}

#endif /* CONFIG_ANSI_CONSOLE */
