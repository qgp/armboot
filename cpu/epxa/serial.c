/*
 * Copyright (c) 2000-2002 Altera Corporation, San Jose, California, USA.  
 * All rights reserved.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "armboot.h"
#define UART00_TYPE (volatile unsigned int*)
#include "epxa/uart00.h"
#include "epxa/excalibur.h"

#define DIVISOR_FROM_BAUD(baud,clk) ((clk) /(16*(baud)))


void serial_setbrg(bd_t *bd, int baudrate)
{
        /*
         * configure the uart for 38400 baud, 8 data,
         * 1 stop, no parity
         */

        *UART_MC(EXC_UART00_BASE) = UART_MC_CLS_CHARLEN_8;
        *UART_DIV_LO(EXC_UART00_BASE) = DIVISOR_FROM_BAUD(baudrate,EXC_AHB2_CLK_FREQUENCY)& 0xFF;
        *UART_DIV_HI(EXC_UART00_BASE) = (DIVISOR_FROM_BAUD(baudrate,EXC_AHB2_CLK_FREQUENCY)& 0xFF00) >> 8;

 
}


/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 *
 */
void serial_init(bd_t *bd)
{
	const char *baudrate;

	if ((baudrate = getenv(bd, "baudrate")) != 0)
		bd->bi_baudrate = simple_strtoul(baudrate, NULL, 10);

	serial_setbrg(bd, bd->bi_baudrate);
	/* Setup and clear FIFOs */
        *UART_FCR(EXC_UART00_BASE)=UART_FCR_RX_THR_1 | UART_FCR_TX_THR_15 |
                UART_FCR_RC_MSK | UART_FCR_TC_MSK;
}


/*
 * Output a single byte to the serial port.
 */
void serial_putc(const char c)
{
	/* Wait for space in the fifo */
        while(((*UART_TSR(EXC_UART00_BASE) & UART_TSR_TX_LEVEL_MSK)==15));

	*UART_TD(EXC_UART00_BASE)=c;

	/* If \n, also do \r */
	if(c == '\n')
		serial_putc('\r');
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_tstc(void)
{

	if(*UART_RSR(EXC_UART00_BASE) & UART_RSR_RX_LEVEL_MSK){
		return 1;
	} else {
		return 0;
	}
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_getc(void)
{


	while(!(*UART_RSR(EXC_UART00_BASE) & UART_RSR_RX_LEVEL_MSK));
	return *UART_RD(EXC_UART00_BASE);
}






