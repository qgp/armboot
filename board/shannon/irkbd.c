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


/*
** Keyboard low-level driver for Shannon (AKA Tuxscreen)
** infrared keyboard port
*/

#include "SA-1100.h"

/*
 * Initialise the infrared port
 *
 */
void irkbd_init(void)
{
	/*
	 * Enable NRZ modulation, and ensure that the port is disabled.
	 */
	Ser2UTCR3 = 0;
	Ser2HSCR0 &= ~HSCR0_HSSP;

	/* clear status register to prevent unwanted interrupts */
	Ser2UTSR0 &= UTSR0_RID | UTSR0_RBB | UTSR0_REB;

	/* 8O1 */
	Ser2UTCR0 = UTCR0_8BitData | UTCR0_PE | UTCR0_OddPar | UTCR0_1StpBit;

	/* 1266 baud */
	Ser2UTCR1 = 0;
	Ser2UTCR2 = 0xb5;

	Ser2UTCR4 = UTCR4_NRZ;
	Ser2HSCR2 = HSCR2_RcDataH | HSCR2_TrDataH;

	/* enable receiver, disable receive fifo interrupt */
	Ser2UTCR3 = UTCR3_RXE;

	/* clear status register to prevent unwanted interrupts */
	Ser2UTSR0 &= UTSR0_RID | UTSR0_RBB | UTSR0_REB;
}

/*
 * See if there is a character available from the keyboard
 *
 */
int irkbd_tstc(void)
{
	return(Ser2UTSR1 & UTSR1_RNE);
}

/*
 * Read a single byte from the infrared keyboard. Returns 1 on success, 0
 * otherwise. When the function is successful, the character read is
 * written into its argument c.
 */
int irkbd_getc(void)
{
	int status;
	while(!((status = Ser2UTSR1)) & UTSR1_RNE)
	{
		if(status & (UTSR1_FRE | UTSR1_ROR | UTSR1_PRE))
			return(-1);
	}
	return(Ser2UTDR & 0xff);
}

#endif /* CONFIG_KEYBOARD */
