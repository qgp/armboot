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

#include <armboot.h>
#include "keyboard.h"

/*
** Keyboard encoder for simple (make/break) keyboard
*/

/*
************************************************************
** Configuration section
************************************************************
*/

/*
** define the lowlevel driver (the one that delivers the
** scancodes):
*/
#ifdef CONFIG_KEYBOARD_IRDA

/* use IrDA keyboard driver */
extern void irkbd_init(void);
extern void irkbd_setleds(int);
extern int  irkbd_tstc(void);
extern int  irkbd_getc(void);

#define KEYBOARD_DEV_INIT		irkbd_init
#define KEYBOARD_DEV_TSTC		irkbd_tstc
#define KEYBOARD_DEV_GETC		irkbd_getc
#define KEYBOARD_DEV_SETLEDS	0
#endif

/*
** The table of supported keyboard mappings:
*/
extern const struct maptable kbd_mapping_table[];


/*
************************************************************
** static storage requirements
************************************************************
*/
struct state
{
	unsigned int				status;
	unsigned int				released;
	unsigned int				composereceived;
	unsigned int				lookahead;
	unsigned int				lookaheadvalid;
	int							(*kbd_tstc)(void);
	int							(*kbd_getc)(void);
	void						(*set_leds)(int);
	const unsigned char			*sequenceptr;
	const struct keyboardmap	*kbd_header;
	unsigned char				composebuffer[2];
};

#define Status			(pSB->status)
#define	SequencePtr		(pSB->sequenceptr)
#define ComposeReceived	(pSB->composereceived)
#define Kbd_getc		(pSB->kbd_getc)
#define Kbd_tstc		(pSB->kbd_tstc)
#define Set_leds		(pSB->set_leds)
#define CC				(pSB->composebuffer)
#define Released		(pSB->released)
#define LookAheadValid	(pSB->lookaheadvalid)
#define LookAhead		(pSB->lookahead)
#define KbMapping		(pSB->kbd_header)

static struct state kbdencode_stateblock;
#define GetStateBlockAddr()	(&kbdencode_stateblock)


#define StatusTable		(pKB->statustable)
#define ConvTabSize		(pKB->convtabsize)
#define ConvTables		(pKB->convtables)
#define SeqString		(pKB->seqstring)
#define ComposeTable	(pKB->composetable)
#define MakeCode		(pKB->makecode)
#define BreakCode		(pKB->breakcode)
#define MkBrkMask		(pKB->mkbrkmask)

/*
** mask for all modifier keys which are non-toggle keys, i.e.
** keys which are active only as long as they are held down:
*/
#define S_NOTOGGLE	(S_LCTRL|S_RCTRL|S_LALT|S_RALT|S_LSHIFT|S_RSHIFT)
#define S_MODIFIER	(S_LCTRL|S_RCTRL|S_LALT|S_RALT|S_LSHIFT|S_RSHIFT) 

/*
**************************************************************
** Look up a compose character from compose table
**************************************************************
*/
static int composechar(struct state  *pSB, const struct keyboardmap *pKB, unsigned char *c)
{
	unsigned char       *cp = c;
	const struct compose_char *ct;

	ct = ComposeTable;
	while(ct->subst)
	{
		if(ct->c[0] == *cp)
			if(ct->c[1] == *(cp + 1))
				break;
		++ct;
	}
	if(ct->subst)
		return(ct->subst);

	ct = ComposeTable;
	cp++;

	while(ct->subst)
	{
		if(ct->c[0] == *cp)
			if(ct->c[1] == *(cp - 1))
				break;
		++ct;
	}
	return(ct->subst);
}


/*
**************************************************************
* scancode to ASCII encoding state machine
**************************************************************
*/
static int encode(
	struct state  *pSB,
	const struct keyboardmap *pKB,
	unsigned int  c,		/* the scancode */
	int           released,	/* wether the key was pressed or released */
	const unsigned char **sequence)
{
	unsigned short	Code;
	const unsigned char	*Seq, *sq;
	int				TableIndex;

	/*
	** compose table index from control, shift, alt and prefix states:
	**
	** The state of the six modifier keys (6 bits) is translated via
	** the status translation table to obtain the actual logical status
	** of SHIFT, CONTROL and ALT (3 bits).
	*/
	TableIndex = *(StatusTable + (Status & (S_MODIFIER)));

	TableIndex &= (NUM_CONVTABLES-1);	/* just a safety measure */
	if(c >= ConvTabSize)
		c = 0;

	Code = *(ConvTables[TableIndex] + c);  /* get code from table */
	if(Code & 0x8000)
		Code &= 0xff;

	if(Code & MODIFIER_KEY)  /* was it shift, alt, ctrl, numlock or capslock ? */
	{
		if(Code & S_NOTOGGLE) /* was it shift, alt or ctrl */
		{
			Code &= S_NOTOGGLE;
			if(released) 			   /* if the key has been released,   */
			{                         /* reset the appropiate status bit */
				Status &= ~Code;
				return(-1);
			}
			else
			{   /* if the key has been pressed, set the status bit */
				Status |= Code;
				return(-1);
			}
		}
		else   /* compose key, numlock or capslock: */
		{
			Code &= (S_COMPOSE | S_NUMLOCK | S_CAPSLOCK);
			if(released)
			{  /* key released: ignore it */
				return(-1);
			}
			else
			{   /* if key has been newly pressed,
				   toggle the appropriate status bit */
				Status ^= Code;
				if(Code & S_COMPOSE)     /* was it the compose key ?    */
					ComposeReceived = 0; /* init compose sequence if so */
				else if(Set_leds)
				{ /* update keyboard status indicators if numlock or capslock */
					Set_leds(((!!(Status & S_NUMLOCK)) << 2)|
					         ((!!(Status & S_CAPSLOCK)) << 1));
				}
				return(-1);
			}
		}
	}
	else  /* non-special key pressed or released: */
	{
		if(released)  /* key has been released: */
		{   /* return 'no character' */
			return(-1);
		}
		else    /* key is pressed: */
		{
			if(Code & AUTOCOMPOSE_KEY)    /* autocompose key ? */
			{	/*
				** If a key's conversion table value has 
				** the AUTOCOMPOSE_KEY bit set, the value
				** is treated as an offset to a string
				** which determines a sequence to be generated
				** by that key:
				*/
				if(!(Status & S_COMPOSE)) /* not yet in compose sequence ? */
				{
					Status |= S_COMPOSE;  /* init compose sequence if so */
					ComposeReceived = 0;
				}
				Code &= ~AUTOCOMPOSE_KEY; /* strip compose bit off key code */
			}
			if(Code & SEQUENCE_KEY)
			{ /* if sequence key: */
				Code &= ~SEQUENCE_KEY;
				Seq = SeqString[Code];	/* get pointer to sequence */

				if(Status & S_NUMLOCK)
				{                       /* if numlock is active:        */
					sq = Seq;           /* see if there is an alternate */
					while(*sq++)        /* character at the end of the  */
						;               /* sequence. If so, return this */
					if((Code = *sq))    /* character, otherwise return  */
						return(Code);   /* the sequence                 */
					else
					{
						*sequence = Seq;
						return(-1);
					}
				}
				else
				{  /* no numlock: flag sequence and return it: */
					*sequence = Seq;
					return(-1);
				}
			}
			else
			{   /* normal key */
				if(Status & S_COMPOSE) /* are we in a compose sequence ? */
				{
					CC[ComposeReceived++] = Code; /* store the character */
					if(ComposeReceived == 2)      /* do we have both ?   */
					{
						Status &= ~S_COMPOSE;     /* clear compose status */
						Code = composechar(pSB, pKB, CC);  /* return composed character */
					}
					else /* only first component: wait for next one */
						return(-1);
				}
				else if(Status & S_CAPSLOCK)
				{	/* convert to upper case if capslock active */
					if(Code >= 'a' && Code <= 'z')
						Code -= 'a' - 'A';
				}
				return(Code);
			}
		}
	}
}


/*
**************************************************************
** get next byte from the keyboard
**************************************************************
*/

static int nextchar(struct state  *pSB)
{
	const struct keyboardmap *pKB = KbMapping;
	int c = -1;

	if(!SequencePtr)	/* no sequence of characters being output ? */
	{
		/*
		** read from keyboard port and pass the keycode
		** through the encoder
		*/
nextk:	if(!Kbd_tstc())			/* any  key available ?   */
			return(-1);
		else if((c = Kbd_getc()) < 0)
			return(c);			/* error reading keyboard */
		else if((c & MkBrkMask) == MakeCode)
		{
			Released = 0;		/* key pressed: continue  */
			if(MkBrkMask == 0xffffffff)
				goto nextk;		/* consume make code */
			else
				c &= ~MkBrkMask;	/* strip make bit(s) from keycode */
		}
		else if((c & MkBrkMask) == BreakCode)
		{
			Released = 1;		/* key released: continue */
			if(MkBrkMask == 0xffffffff)
				goto nextk;		/* consume break code */
			else
				c &= ~MkBrkMask;	/* strip break bit(s) from keycode */
		}
		else if((c = encode(pSB, pKB, c, Released, &SequencePtr)) >= 0)
			return(c);			/* normal character: return it */
	}
	if(SequencePtr)	/* a sequence of characters being output ? */
	{
		c = *SequencePtr++;	/* get next key of sequence     */
		if(!*SequencePtr)	/* end of sequence ?            */
			SequencePtr = (unsigned char*)0;  /* terminate it */
	}
	return(c);
}

/*
**************************************************************
** See if a byte is available from the keyboard
**************************************************************
*/

int kbd_tstc(void)
{
	struct state  *pSB = GetStateBlockAddr();
    int c;

	if(LookAheadValid)	/* do we have a byte in the lookahead buffer ? */
	{	/* if cfb_console is in use, draw cursor while waiting for input */
		return(1);
	}
	else if((c = nextchar(pSB)) >= 0)	/* get a byte from the keyboard */
    {
		LookAhead = c;
		LookAheadValid = 1;
		return(1);
	}
	else
	{
		return(0);
	}
}

/*
**************************************************************
** Read a byte from the keyboard.
**************************************************************
*/

int kbd_getc(void)
{
	struct state  *pSB = GetStateBlockAddr();
	int c;

	if(LookAheadValid)	/* do we have a byte in the lookahead buffer ? */
	{
		LookAheadValid = 0;
		return(LookAhead);
	}
	else
	{	/* wait until there is something available from the keyboard */
		while((c = nextchar(pSB)) < 0)
			;
	}
	return(c);
}

/*
**************************************************************
** Select keyboard mapping
**************************************************************
*/

void kbd_mapping(const char *name)
{
	int i;
	struct state  *pSB = GetStateBlockAddr();

	/* scan mapping table for a matching name */
	for(i = 0; kbd_mapping_table[i].name; i++)
	{
		if(!strcmp(name, kbd_mapping_table[i].name))
		{	/* match found: select this mapping */
			KbMapping = kbd_mapping_table[i].map;
			return;
		}
	}
	/* no match: use default mapping */
	KbMapping = kbd_mapping_table[0].map;
	return;
}

/*
**************************************************************
** Initialize keyboard encoder
**************************************************************
*/

void kbd_init(bd_t *bd)
{
	char *mapping_name;
	void (*Kbd_init)(void);
	struct state  *pSB = GetStateBlockAddr();

	/* Initialize status variables */
	Status          = 0;
	ComposeReceived = 0;
	SequencePtr     = (unsigned char *)0;
	Released		= 0;
	LookAheadValid	= 0;
	LookAhead		= 0;

	/* get keyboard mapping table, use "us" by default */
	if(!(mapping_name = getenv(bd, "keymap")))
		mapping_name = "us";
	kbd_mapping(mapping_name);

	/* get hardware access functions */
	Kbd_init        = KEYBOARD_DEV_INIT;
	Kbd_getc		= KEYBOARD_DEV_GETC;
	Kbd_tstc		= KEYBOARD_DEV_TSTC;
	Set_leds		= KEYBOARD_DEV_SETLEDS;

	/* initialize hardware */
	Kbd_init();
}

#endif /* CONFIG_KEYBD */
