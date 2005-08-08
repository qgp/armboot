/*
 * Copyright (c) 2000-2002 Altera Corporation, San Jose, California, USA.  
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State 
 * of California and by the laws of the United States of America.
 */

#include "armboot.h"
#include "command.h"
#include "net.h"
#include "epxa/excalibur.h"
#include "../drivers/smc91111.h"

extern char flash_read_user(int bank, int offset);
	
/*
 * epxa1db_set_mac_addr is called prior to the open function of the SMC network
 * driver. It is used to obtain the mac address. If the environment variable
 * 'ipaddr' has a value, that is the address used. Otherwise the 
 * address is calculated using the contents of flash.
 * 
 */

void epxa1db_set_mac_addr(bd_t* bd){
	
	static const char vendor_id[2]={0x07,0xed};
	
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
        else{ 

	    /* calculate the value using the flash contents */

	    addr[5] = (char) flash_read_user(1, 0);
	    addr[4] = (char) flash_read_user(1, 1); 
	    addr[3] = 0;
	    addr[2] = vendor_id[1];
	    addr[1] = vendor_id[0];
	    addr[0] = 0;

	    /* update the environment */

	    sprintf(buf,"%x:%x:%x:%x:%x:%x", addr[0], addr[1], addr[2], addr[3],
                                                   addr[4], addr[5]);
	    setenv (bd, "ethaddr", buf);
        }

	//	smc_set_mac_addr(addr); 

}








