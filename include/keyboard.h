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

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_ 1

#include <armboot.h>

/*
** A sequence key is identified by it's bit #14 being set,
** a modifier key is identified by it's bit #13 being set.
** an autocompose key is identified by it's bit #12 being set.
*/
#define SEQUENCE_KEY 0x4000
#define SKEY(x) (SEQUENCE_KEY|(x))

#define MODIFIER_KEY	0x2000
#define MKEY(x) (MODIFIER_KEY|(x))

#define AUTOCOMPOSE_KEY 0x1000
#define AKEY(x) (AUTOCOMPOSE_KEY|(x))

/*
** Status bits for modifier keys:
**
** The driver must keep track of each modifier key individually.
** The status of the right and left ALT, CONTROL and SHIFT key pairs
** (6 keys->64 possibilities) is translated via a status
** translation table in the keyboard map to determine the
** actual logical SHIFT, ALT and CONTROL state.
** This means, that -for example- it is possible to give the
** right ALT key the function of CTRL-ALT, e.g. for a german
** DIN keyboard.
*/

#define S_LCTRL		0x001
#define S_RCTRL		0x002

#define S_LALT		0x004
#define S_RALT		0x008

#define S_LSHIFT	0x010
#define S_RSHIFT	0x020

#define S_NUMLOCK	0x080
#define S_CAPSLOCK	0x100
#define S_COMPOSE	0x200

#define L_NONE		0x00
#define L_CTRL      0x01
#define L_ALT       0x02
#define L_SHIFT     0x04

/*
** Number of logical modifier states:
*/
#define NUM_CONVTABLES	((L_CTRL|L_ALT|L_SHIFT) + 1)

/*
** Some ASCII codes
*/
#define ESC	0x1b
#define SPC	' '
#define BSP	0x08
#define DEL	0x7f
#define TAB	0x09
#define ENT	0x0d

/*
** If one of the following symbols appears in a conversion
** table, the corresponding key becomes a modifier key.
** These special function keys do not issue any codes
** when being pressed, but are used to switch between key
** assignments (i.e. different conversion tables). The
** numlock and capslock functions are toggles, i.e. their
** function is activated by pressing the corresponding key
** once and is deactivated by pressing it once more.
** The compose key switches the keyboard into compose mode
** which will cause the next two keys to be combined via the
** compose table. Compose mode is always reset after two keys
** have been pressed.
** All the other functions are active only while the corresponding
** key is being held down.
*/

#define LCT MKEY(S_LCTRL)   /* left control key */
#define RCT MKEY(S_RCTRL)   /* right   "     " */
#define LAL MKEY(S_LALT)    /* left alt key */
#define RAL MKEY(S_RALT)    /* right "   " */
#define LSH MKEY(S_LSHIFT)  /* left shift key */
#define RSH MKEY(S_RSHIFT)  /* right  "    " */
#define NML MKEY(S_NUMLOCK) /* numlock key */
#define CPL MKEY(S_CAPSLOCK)/* capslock key */
#define CMP MKEY(S_COMPOSE) /* compose key */



/*
** Compose characters allow to enter some international
** characters by "composing" two keys. To enter a composed
** character, the user must enter the "compose key" followed
** by the two characters to be composed. E.g. by entering
** <Compose-Key>-A-", you get the german umlaut Ä.
**
** structure of one entry in the compose key table:
*/
struct compose_char
{
	unsigned char c[2];  /* characters to be composed  */
	unsigned char subst; /* resulting compose key code */
};

/*
** structure to describe a keyboard mapping
**
** Some keyboards issue two bytes for each keypress/keyrelease
** event, where the first byte indicates wether the key has
** been pressed/released and the second byte indicates the
** individual key. In these cases, the driver must "consume"
** the first byte (i.e. the make/break code), while passing the
** the key code down to the encoder state machine.
**
** Some keyboards issue a single byte per keypress/keyrelease
** event, where -typically- the high bit indicates wether the
** key has been pressed/released and the lower bits indicate
** the individual key. In these cases, the driver must strip
** off the make/break bit(s) and pass the remaining key code
** down to the encoder state machine.
**
** It is possible to control this behavior by specifying
** three values in the keyboard map:
**
** if <byte> & mkbrkmask == makecode -> key has been pressed
** if <byte> & mkbrkmask == breakcode -> key has been released
** if mkbrkmask == 0xffffffff -> consume make/break code
*/
struct keyboardmap
{
	unsigned int				convtabsize;	/* # of entries in each table */
	unsigned int				makecode;		/* make code */
	unsigned int				breakcode;		/* break code */
	unsigned int				mkbrkmask;		/* make/break code mask */
	const unsigned char			*statustable;	/* state translation table */
	const unsigned short		*convtables[NUM_CONVTABLES];
	const unsigned char			* const * seqstring;
	const struct compose_char	*composetable;
};

/*
** entry in list of keyboard mappings
*/
struct maptable
{
	const char *name;
	const struct keyboardmap *map;
};


/*
** Function prototytes:
*/
extern void kbd_init(bd_t *bd);
extern int  kbd_tstc(void);
extern int  kbd_getc(void);
extern void kbd_mapping(const char *name);

#endif /* _KEYBOARD_H_ */
