/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Rolf Offermanns <rof@sysgo.de>
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

#include "armboot.h"
/* ------------------------------------------------------------------------- */

extern void smc_set_mac_addr(const char *addr);

void dnp1110_set_mac_addr(bd_t* bd) {
	char addr[6];
    char buf[18];

    char* s;
    int i;
    char* e;

	/*
	 * if the mac address is defined in the environment, use that,
	 * otherwise derive it from the contents of flash.
	 */

    s = getenv(bd, "ethaddr");

    if (s) {
        for (i=0; i<6; i++) {
            addr[i] = s ? simple_strtoul(s, &e, 16) : 0;
            if (s) s = (*e) ? e+1 : e;
        }
    } 
	else {
	    /* MAC ADDRESS AT FLASHBLOCK 1 / OFFSET 0x10 */
        unsigned char *dnp1110_mac = (unsigned char *) (0x20010);

        for (i=0; i < 6; i++) {
            addr[i] = *(dnp1110_mac+i);
        }
   
        /* update the environment */
        sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2], addr[3],
                                                   addr[4], addr[5]);
        setenv (bd, "ethaddr", buf);
		
	}
	smc_set_mac_addr(addr);
}


