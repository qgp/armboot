/*
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
#include "clps7111.h"

void serial_setbrg(bd_t *bd, int baudrate)
{
    unsigned int reg = 0;
   
    if (baudrate == 1200)        reg = 191;
    else if (baudrate == 9600)   reg = 23;
    else if (baudrate == 19200)  reg = 11;
    else if (baudrate == 38400)  reg = 5;
    else if (baudrate == 57600)  reg = 3;
    else if (baudrate == 115200) reg = 1;
    else hang();

    // init serial serial 1,2
    IO_SYSCON1 = SYSCON1_UART1EN;
    IO_SYSCON2 = SYSCON2_UART2EN;
    
    reg |= UBRLCR_WRDLEN8;

    IO_UBRLCR1 = reg;
    IO_UBRLCR2 = reg;
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
}


/*
 * Output a single byte to the serial port.
 */
void serial_putc(const char c)
{
    int tmo;

    /* If \n, also do \r */
    if(c == '\n')
      serial_putc('\r');

    tmo = get_timer(0) + 1*CFG_HZ;
    while(IO_SYSFLG1 & SYSFLG1_UTXFF)
      if (get_timer(0) > tmo)
	break;

    IO_UARTDR1 = c;
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_tstc(void)
{
    return !(IO_SYSFLG1 & SYSFLG1_URXFE);
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_getc(void)
{
    while (IO_SYSFLG1 & SYSFLG1_URXFE)
      ;

    return IO_UARTDR1 & 0xff;
}
