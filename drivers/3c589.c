/*------------------------------------------------------------------------
 . smc91111.c
 . This is a driver for SMSC's 91C111 single-chip Ethernet device.
 .
 . (C) Copyright 2002
 . Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 . Rolf Offermanns <rof@sysgo.de>
 .
 . Copyright (C) 2001 Standard Microsystems Corporation (SMSC)
 .       Developed by Simple Network Magic Corporation (SNMC)
 . Copyright (C) 1996 by Erik Stahlman (ES)
 .
 . This program is free software; you can redistribute it and/or modify
 . it under the terms of the GNU General Public License as published by
 . the Free Software Foundation; either version 2 of the License, or
 . (at your option) any later version.
 .
 . This program is distributed in the hope that it will be useful,
 . but WITHOUT ANY WARRANTY; without even the implied warranty of
 . MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 . GNU General Public License for more details.
 .
 . You should have received a copy of the GNU General Public License
 . along with this program; if not, write to the Free Software
 . Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 .
 . Information contained in this file was obtained from the LAN91C111
 . manual from SMC.  To get a copy, if you really want one, you can find 
 . information under www.smsc.com.
 . 
 .
 . "Features" of the SMC chip:
 .   Integrated PHY/MAC for 10/100BaseT Operation
 .   Supports internal and external MII
 .   Integrated 8K packet memory
 .   EEPROM interface for configuration
 .
 . Arguments:
 . 	io	= for the base address
 .	irq	= for the IRQ
 .
 . author:
 . 	Erik Stahlman				( erik@vt.edu )
 . 	Daris A Nevil				( dnevil@snmc.com )
 .
 .
 . Hardware multicast code from Peter Cammaert ( pc@denkart.be )
 .
 . Sources:
 .    o   SMSC LAN91C111 databook (www.smsc.com)
 .    o   smc9194.c by Erik Stahlman
 .    o   skeleton.c by Donald Becker ( becker@cesdis.gsfc.nasa.gov )
 .
 . History:
 .	10/17/01  Marco Hasewinkel Modify for DNP/1110
 .	07/25/01  Woojung Huh      Modify for ADS Bitsy
 .	04/25/01  Daris A Nevil    Initial public release through SMSC
 .	03/16/01  Daris A Nevil    Modified smc9194.c for use with LAN91C111
 ----------------------------------------------------------------------------*/

#include "armboot.h"
#include "command.h"
#include "net.h"

#ifdef CONFIG_DRIVER_3C589

#include "3c589.h"


// Use power-down feature of the chip
#define POWER_DOWN	0

#define NO_AUTOPROBE

static const char version[] =
	"Your ad here! :P\n";


#undef EL_DEBUG 

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long int dword;
/*------------------------------------------------------------------------
 .
 . Configuration options, for the experienced user to change.
 .
 -------------------------------------------------------------------------*/

/*
 . Wait time for memory to be free.  This probably shouldn't be
 . tuned that much, as waiting for this means nothing else happens
 . in the system
*/
#define MEMORY_WAIT_TIME 16


#if (EL_DEBUG > 2 )
#define PRINTK3(args...) printf(args)
#else
#define PRINTK3(args...)
#endif

#if EL_DEBUG > 1
#define PRINTK2(args...) printf(args)
#else
#define PRINTK2(args...)
#endif

#ifdef EL_DEBUG
#define PRINTK(args...) printf(args)
#else
#define PRINTK(args...)
#endif

#define outb(args...)	mmio_outb(args)
#define mmio_outb(value, addr)	(*((volatile byte *)(addr)) = value)

#define inb(args...)	mmio_inb(args)
#define mmio_inb(addr) (*((volatile byte *)(addr)))

#define outw(args...)	mmio_outw(args)
#define mmio_outw(value, addr)	(*((volatile word *)(addr)) = value)

#define inw(args...)	mmio_inw(args)
#define mmio_inw(addr) (*((volatile word *)(addr)))

#define outsw(args...)	mmio_outsw(args)
#define mmio_outsw(r,b,l)	({	int __i; \
					word *__b2; \
					__b2 = (word *) b; \
					for (__i = 0; __i < l; __i++) { \
					    mmio_outw( *(__b2 + __i), r); \
					} \
				})

#define insw(args...)	mmio_insw(args)
#define mmio_insw(r,b,l) 	({	int __i ;  \
					word *__b2;  \
			    		__b2 = (word *) b;  \
			    		for (__i = 0; __i < l; __i++) {  \
					  *(__b2 + __i) = mmio_inw(r);  \
					  mmio_inw(0);  \
					};  \
				})
					
/*------------------------------------------------------------------------
 .
 . The internal workings of the driver.  If you are changing anything
 . here with the SMC stuff, you should have the datasheet and know
 . what you are doing.
 .
 -------------------------------------------------------------------------*/
#define EL_BASE_ADDR	0x20000000


/* Offsets from base I/O address. */
#define EL3_DATA	0x00
#define EL3_TIMER	0x0a
#define EL3_CMD		0x0e
#define EL3_STATUS	0x0e

#define EEPROM_READ	0x0080

#define EL3WINDOW(win_num) mmio_outw(SelectWindow + (win_num), EL_BASE_ADDR + EL3_CMD)

/* The top five bits written to EL3_CMD are a command, the lower
   11 bits are the parameter, if applicable. */
enum c509cmd {
    TotalReset = 0<<11, SelectWindow = 1<<11, StartCoax = 2<<11,
    RxDisable = 3<<11, RxEnable = 4<<11, RxReset = 5<<11, RxDiscard = 8<<11,
    TxEnable = 9<<11, TxDisable = 10<<11, TxReset = 11<<11,
    FakeIntr = 12<<11, AckIntr = 13<<11, SetIntrEnb = 14<<11,
    SetStatusEnb = 15<<11, SetRxFilter = 16<<11, SetRxThreshold = 17<<11,
    SetTxThreshold = 18<<11, SetTxStart = 19<<11, StatsEnable = 21<<11,
    StatsDisable = 22<<11, StopCoax = 23<<11,
};

enum c509status {
    IntLatch = 0x0001, AdapterFailure = 0x0002, TxComplete = 0x0004,
    TxAvailable = 0x0008, RxComplete = 0x0010, RxEarly = 0x0020,
    IntReq = 0x0040, StatsFull = 0x0080, CmdBusy = 0x1000
};

/* The SetRxFilter command accepts the following classes: */
enum RxFilter {
    RxStation = 1, RxMulticast = 2, RxBroadcast = 4, RxProm = 8
};

/* Register window 1 offsets, the window used in normal operation. */
#define TX_FIFO		0x00
#define RX_FIFO		0x00
#define RX_STATUS 	0x08
#define TX_STATUS 	0x0B
#define TX_FREE		0x0C	/* Remaining free bytes in Tx buffer. */


/*
  Read a word from the EEPROM using the regular EEPROM access register.
  Assume that we are in register window zero.
*/
static word read_eeprom(dword ioaddr, int index)
{
    int i;
    outw(EEPROM_READ + index, ioaddr + 0xa);
    /* Reading the eeprom takes 162 us */
    for (i = 1620; i >= 0; i--)
	if ((inw(ioaddr + 10) & EEPROM_BUSY) == 0)
	    break;
    return inw(ioaddr + 0xc);
}

static void el_get_mac_addr( word *mac_addr )
{
	int i;
	unsigned char old_window = inw( EL_BASE_ADDR + EL3_STATUS ) >> 13;
	GO_WINDOW(0);
	VX_BUSY_WAIT;
	for (i = 0; i < 3; i++)
	{
		*(mac_addr+i) = read_eeprom(EL_BASE_ADDR, 0xa+i);
	}
	GO_WINDOW(old_window);
	VX_BUSY_WAIT;
}


static void print_packet( byte * buf, int length )
{
        int i;
        int remainder;
        int lines;

        PRINTK2("Packet of length %d \n", length );

        lines = length / 16;
        remainder = length % 16;

        for ( i = 0; i < lines ; i ++ ) {
                int cur;

                for ( cur = 0; cur < 8; cur ++ ) {
                        byte a, b;

                        a = *(buf ++ );
                        b = *(buf ++ );
                        PRINTK2("%02x%02x ", a, b );
                }
                PRINTK2("\n");
        }
        for ( i = 0; i < remainder/2 ; i++ ) {
                byte a, b;

                a = *(buf ++ );
                b = *(buf ++ );
                PRINTK2("%02x%02x ", a, b );
        }
        PRINTK2("\n");
}



/**************************************************************************
ETH_RESET - Reset adapter
***************************************************************************/
static void el_reset(bd_t *bd)
{
	/***********************************************************
			Reset 3Com 595 card
	*************************************************************/
	/* QUICK HACK 
	 * - adjust timing for 3c589
	 * - enable io for PCMCIA */
	outw(0x0004, 0xa0000018);
	udelay(100);
	outw(0x0041, 0x28010000);
	udelay(100);
	
	/* issue global reset */
	outw(GLOBAL_RESET, BASE + VX_COMMAND);

	/* must wait for at least 1ms */
	udelay(100000000);

	/* set mac addr */
/*
	outw(0xfeca, BASE + VX_W2_ADDR_0);
	VX_BUSY_WAIT;
	outw(0xadde, BASE + VX_W2_ADDR_2);
	VX_BUSY_WAIT;
	outw(0xefbe, BASE + VX_W2_ADDR_4);
	VX_BUSY_WAIT;
*/
	{
		word *mac_addr = (word *)bd->bi_enetaddr;
		int i;
		
		el_get_mac_addr( mac_addr );
		
		GO_WINDOW(2);
		VX_BUSY_WAIT;

		printf("3C589 MAC Addr.: ");
		for (i = 0; i < 3; i++)
		{
			printf("%04x", mac_addr[i]);
			outw(mac_addr[i], BASE + VX_W2_ADDR_0 + i*2);
			VX_BUSY_WAIT;
		}
		printf("\n\n");
	}

	/* set RX filter */
	outw(SET_RX_FILTER | FIL_INDIVIDUAL | FIL_BRDCST, BASE + VX_COMMAND);
	VX_BUSY_WAIT;


	/* set irq mask and read_zero */
	outw(SET_RD_0_MASK | S_CARD_FAILURE | S_RX_COMPLETE |
		S_TX_COMPLETE | S_TX_AVAIL, BASE + VX_COMMAND);
	VX_BUSY_WAIT;

	outw(SET_INTR_MASK | S_CARD_FAILURE | S_RX_COMPLETE |
		S_TX_COMPLETE | S_TX_AVAIL, BASE + VX_COMMAND);
	VX_BUSY_WAIT;

	/* enable TP Linkbeat */
	GO_WINDOW(4);
	VX_BUSY_WAIT;
	
	outw( ENABLE_UTP, BASE + VX_W4_MEDIA_TYPE);
	VX_BUSY_WAIT;


/*
 * Attempt to get rid of any stray interrupts that occured during
 * configuration.  On the i386 this isn't possible because one may
 * already be queued.  However, a single stray interrupt is
 * unimportant.
 */

	outw(ACK_INTR | 0xff, BASE + VX_COMMAND);
	VX_BUSY_WAIT;

	/* enable TX and RX */
	outw( RX_ENABLE, BASE + VX_COMMAND );
	VX_BUSY_WAIT;
	
	outw( TX_ENABLE, BASE + VX_COMMAND );
	VX_BUSY_WAIT;


	/* print the diag. regs. */
	PRINTK2("Diag. Regs\n");
	PRINTK2("--> MEDIA_TYPE:   %04x\n", inw(BASE + VX_W4_MEDIA_TYPE));
	PRINTK2("--> NET_DIAG:     %04x\n", inw(BASE + VX_W4_NET_DIAG));
	PRINTK2("--> FIFO_DIAG:    %04x\n", inw(BASE + VX_W4_FIFO_DIAG));
	PRINTK2("--> CTRLR_STATUS: %04x\n", inw(BASE + VX_W4_CTRLR_STATUS));
	PRINTK2("\n\n");

	/* enter working mode */
	GO_WINDOW(1);
	VX_BUSY_WAIT;

}

	
/*-----------------------------------------------------------------
 .
 .  The driver can be entered at any of the following entry points.
 .
 .------------------------------------------------------------------  */

extern int eth_init(bd_t *bd);
extern void eth_halt(void);
extern int eth_rx(void);
extern int eth_send(volatile void *packet, int length);


/*
 ------------------------------------------------------------
 .
 . Internal routines
 .
 ------------------------------------------------------------
*/

int eth_init(bd_t *bd) 
{
	el_reset(bd);
	return 0;
}

void eth_halt() {
	return;
}

#define EDEBUG 1


/**************************************************************************
ETH_POLL - Wait for a frame
***************************************************************************/

int eth_rx()
{
	word status, rx_status, packet_size;
	
	VX_BUSY_WAIT;
	
	status = inw( BASE + VX_STATUS );

	if ( (status & S_RX_COMPLETE) == 0 ) return 0; /* nothing to do */
	
	/* Packet waiting -> check RX_STATUS */
	rx_status = inw( BASE + VX_W1_RX_STATUS );

	if ( rx_status & ERR_RX )
	{
		/* error in packet -> discard */
		PRINTK("[ERROR] Invalid packet -> discarding\n");
		outw( RX_DISCARD_TOP_PACK, BASE + VX_COMMAND );
		return 0;
	}

	/* correct pack. waiting in fifo */
	packet_size = rx_status & RX_BYTES_MASK;

	PRINTK("Correct packet waiting in fifo, size: %d\n", packet_size);
	
	{
		volatile word *packet_start = (word *)(BASE + VX_W1_RX_PIO_RD_1);
		word *RcvBuffer = (word *)(NetRxPackets[0]);
		int wcount = 0;

		for (wcount = 0; wcount < (packet_size >> 1); wcount++)
		{
			*RcvBuffer++ = *(packet_start);
		}
	
		/* handle odd packets */	
		if ( packet_size & 1 )
		{
			*RcvBuffer++ = *(packet_start);
		}
	}

	/* fifo should now be empty (besides the padding bytes) */
	if ( ((*((word *)(BASE + VX_W1_RX_STATUS))) & RX_BYTES_MASK) > 3 )
	{
		PRINTK("[ERROR] Fifo not empty after packet read (remaining pkts: %d)\n",
			(((*(word *)(BASE + VX_W1_RX_STATUS))) & RX_BYTES_MASK));
	}

	/* discard packet */
	*((word *)(BASE + VX_COMMAND)) = RX_DISCARD_TOP_PACK;

	/* Pass Packets to upper Layer */
	NetReceive(NetRxPackets[0], packet_size);
	return packet_size;
}



/**************************************************************************
ETH_TRANSMIT - Transmit a frame
***************************************************************************/
static char padmap[] = {
	0, 3, 2, 1};


int eth_send(volatile void *packet, int length) {
	int pad;
	int status;
	volatile word *buf = (word *)packet;
	int dummy = 0;

	/* padding stuff */
	pad = padmap[length & 3];

	PRINTK("eth_send(), length: %d\n", length);
	/* drop acknowledgements */
	while(( status=inb(EL_BASE_ADDR + VX_W1_TX_STATUS) )& TXS_COMPLETE ) {
		if(status & (TXS_UNDERRUN|TXS_MAX_COLLISION|TXS_STATUS_OVERFLOW)) {
			outw(TX_RESET, EL_BASE_ADDR + VX_COMMAND);
			outw(TX_ENABLE, EL_BASE_ADDR + VX_COMMAND);
			PRINTK("Bad status, resetting and reenabling transmitter\n");
		}

		outb(0x0, EL_BASE_ADDR + VX_W1_TX_STATUS);
	}

	
	while (inw(EL_BASE_ADDR + VX_W1_FREE_TX) < length + pad + 4) {
		/* no room in FIFO */
		if (dummy == 0)
		{
			PRINTK("No room in FIFO, waiting...\n");
			dummy++;
		}

	}

	PRINTK("    ---> FIFO ready\n");
	

	outw(length, EL_BASE_ADDR + VX_W1_TX_PIO_WR_1);

	/* Second dword meaningless */
	outw(0x0, EL_BASE_ADDR + VX_W1_TX_PIO_WR_1);	
	
	print_packet((byte *)buf, length);
	/* write packet */
	{
		unsigned int i, totdw;
		
		totdw = ((length + 3) >> 1);
		PRINTK("Buffer: (totdw = %d)\n", totdw);
		for (i = 0; i < totdw; i++) {
			outw( *(buf+i), EL_BASE_ADDR + VX_W1_TX_PIO_WR_1);
			PRINTK("%04x ", *(buf+i));
			if ( ((i % 8) == 0) && (i != 0) )
				PRINTK("\n"); 
			udelay(10);
		}
		PRINTK("\n\n");
	}	

        /* wait for Tx complete */
	PRINTK("Waiting for Tx to complete...\n");
        while((inw(EL_BASE_ADDR + VX_STATUS) & S_COMMAND_IN_PROGRESS) != 0)
	{
		udelay(10);
	}
	PRINTK("   ---> Tx completed\n");

	return length;
}



#endif /* CONFIG_DRIVER_3C589 */
