/*-----------------------------------------------------------------
 .
 .  The driver can be entered at any of the following entry points.
 .
 .------------------------------------------------------------------  */
#include "armboot.h"
#include "command.h"
#include "net.h"

#ifdef CONFIG_DRIVER_EASYNET

#define E_INFO
//#define E_DEBUG

#define E_BASE			0x80000000

#define read_rcv_fifo      (*((volatile u32 *)( E_BASE + 0 )))
#define read_rcv_length    (*((volatile u32 *)( E_BASE + 0x04 )))
#define read_ready_to_send (*((volatile u32 *)( E_BASE + 0x08 )))
#define read_rcving        (*((volatile u32 *)( E_BASE + 0x0C )))
#define read_IRQ	   (*((volatile u32 *)( E_BASE + 0x10 )))
#define read_MAC_type	   (*((volatile u32 *)( E_BASE + 0x14 )))
#define read_system_id	   (*((volatile u32 *)( E_BASE + 0x18 )))
#define read_mdio_reg	   (*((volatile u32 *)( E_BASE + 0x1C )))

#define write_snd_fifo(v)      (*((volatile u32 *)(E_BASE + 0)) = v)
#define write_snd_length(v)    (*((volatile u32 *)(E_BASE + 0x04)) = v)
#define write_resets(v)        (*((volatile u32 *)(E_BASE + 0x08)) = v)
#define write_ACK(v)           (*((volatile u32 *)(E_BASE + 0x0C)) = v)
#define write_configuration(v) (*((volatile u32 *)(E_BASE + 0x10)) = v)
#define write_MAC(v)           (*((volatile u32 *)(E_BASE + 0x14)) = v)
#define write_SND_ACK(v)       (*((volatile u32 *)(E_BASE + 0x18)) = v)
#define write_mdio_reg(v)      (*((volatile u32 *)(E_BASE + 0x1C)) = v)


#ifdef E_DEBUG
#define PRINTK(args...) printf(args)
#else
#define PRINTK(args...)
#endif

#ifdef E_INFO
#define PRINTI(args...) printf(args)
#else
#define PRINTI(args...)
#endif

// MACs for M_type Dest addresses
const unsigned char e_broadcast_dest[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
const unsigned char     e_pause_dest[6] = {0x01,0x80,0xC2,0x00,0x00,0x01};
const unsigned char e_multicast_dest[6] = {0x01,0x80,0xC2,0x00,0x00,0x02};
static u8 easynet_mac_addr[] = {0xea, 0x54, 0x5c, 0x5b, 0x00, 0x04}; 

extern int eth_init(bd_t *bd);
extern void eth_halt(void);
extern int eth_rx(void);
extern int eth_send(volatile void *packet, int length);



/////////////////////////////////////////////////////////////////////////
//  Set MAC Address
static void set_mac_address(u8 *mac)
{
  int i;
  int sum=0;
  PRINTK("tk_debug_set_mac_address: reading MAC: ");
  for (i=0;i<6;i++) PRINTK("%2X ", (unsigned char ) mac[i]);
  PRINTK("\n");


  if (  (mac[0]) & 1){
    printf("Easynet ERROR: Setting Bit 0 in first MAC Address Byte forbidden\n");
  }

  PRINTI("Easynet: Setting MAC address to: ");
  for (i = 0; i < 6; i++) {
    if (i) PRINTI (":");
    PRINTI("%02X", mac[i]);
    write_MAC(mac[i]);
    sum += mac[i];
  }
  PRINTI("\n");

  if (!sum) printf("Easynet ERROR: MAC ADDRESS not set properly!\n");
}
/************************************* MDIO ACCESS *************************************/
/************************************* VVVVVVVVVVV *************************************/
/***** MDIO READ ***************************/
// #define MDIO_RW_ECHO
static int easynet_mdio_read(int regno) {
  /*******************************/
  /* MDIO tests */
  
  int i=0;
  
  u32 mdio_read=0x80000000;
  regno = regno & 0x1f; // not more than 32 Registers in LTX971
  write_mdio_reg(regno); // read out Register
  //  PRINTK("-------- MDIO Read:");  // This is actually a wait loop for busy going high
  // min: 1 cyle of MDC (at 25MHz PLD: 320 ns; 40 MHz PLD: 200ns) - CPU 200MHz:64 cycles
  // Better:
  udelay(1); // 1us Wait
  while ((mdio_read & 0x80000000) != 0) {
    mdio_read=read_mdio_reg;
    i++;
    if (i==1000) mdio_read=0;
  }
  //  printk("MDIO_READ: Timer i: %2d, Read MDIO Register %2d: 0x%04x\n",i,regno,mdio_read);
  //  mdio_read=read_mdio_reg;
  //  printk("MDIO_READ: Read again: 0x%04x\n",mdio_read);

  //  PRINTK("MDIO_READ: Timer i: %2d, Read MDIO Register %2d: 0x%04x\n",i,regno,(unsigned int) mdio_read);

  //      mdio_read=read_mdio_reg;
  // printk("Timer i: %d, Read MDIO Register %d: 0x%x\n",i+1,regno,mdio_read);
  if (i==1000) return(-1); else return(mdio_read);
}
/***** MDIO WRITE ***************************/
static int easynet_mdio_write(int regno,int value) {
  int i=0;
  u32 mdio_read=0x80000000;
  regno = (regno & 0x1f) | 0x400; // not more than 32 Registers in LTX971, set write bit
  value = value &0xffff;
  write_mdio_reg((value<<16) | regno); // read out Register
  //  PRINTK(" Written "); // same wait as in mdio_read
  udelay(1); // 1us Wait - better: wait till busy up, than till down (but lock?)
  while ((mdio_read & 0x80000000) != 0) {
    mdio_read=read_mdio_reg;
    i++;
    if (i==1000) mdio_read=0;
  }
  //  PRINTK("0x%04x at Register %2d (0x%08x)  - waited %d looops\n",value,regno&0x1f,((value<<16) | regno),i);
  return 0;
  // return -1 when busy dont gets low in waitloop - shouldnt, but who knows?
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int eth_init(bd_t *bd) {

  PRINTI("Easynet reset\n");
  // Reset
  write_resets(7); // Put all in reset
  udelay(1000);
  write_resets(0); // start operating
  udelay(2000000);

  // Configure operation
  write_configuration(0);
  write_configuration(0);
  write_configuration(0);
  write_configuration(0);
  write_configuration(0);
  write_configuration(0);
  write_configuration(0);
  write_configuration(1); // pause_frames
  write_configuration(1); // slow_protocol
  write_configuration(0); // promiscuous mo  

  //set mac address
  // todo: parse kernel cmd line for mac address!
  //	NetCopyEther(NetOurEther, bd->bi_enetaddr);
  // should be bd->bi_enetaddr !!!

  //  set_mac_address(easynet_mac_addr);
  set_mac_address(bd->bi_enetaddr);
  udelay(1000);

  PRINTI("Easynet autonegotiate\n");
  // Autonegotiate 10mbps HD
  easynet_mdio_write(4,0x0041);
  if (easynet_mdio_read(17) & 0x0400) {
    PRINTI("Link is up, restart autoneg\n");
    easynet_mdio_write(0,0x1200);
  }
  udelay(1000000);


  return 0;
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void eth_halt() {
  write_resets(7);
  //  smc_close();
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int eth_rx() {
  int i,len,length,words,M_type;
  int IRQ_type;
  char memory[1600];
  int *imemory = (int *) memory;
  unsigned char *copyto = (unsigned char *) NetRxPackets[0];

  // was there anything received?
  IRQ_type = read_IRQ;
  if ( IRQ_type & 0x2 ) { // pending sent interrupt, should not happen
    write_SND_ACK(1); 
  }
  if ( !(IRQ_type&1) ) return 0; // nothing received
  length = read_rcv_length;	 // get length from MAC
  len    = length + 14;          // add the header
  if (len > 1600) {
    printf("Easynet ERROR: Ethernet Packet > 1600 Bytes\n");
    write_ACK(1);
    return 0;
  }
  words  = (length+3)/4 + 2;     // number of FIFO reads
  M_type = read_MAC_type;
  for (i=0 ; i<words ; i++) 
    imemory[i] = read_rcv_fifo; // read directly integer-wise to char-array

  write_ACK(1); /* achnowledge to the MAC */

  switch(M_type) 
    {
    case  2: memcpy(copyto,e_broadcast_dest,6); break; // Broadcast
    case  4: memcpy(copyto,e_multicast_dest,6); break; // Multicast
    case  8: memcpy(copyto,e_pause_dest,    6); break; // Pause
    default: memcpy(copyto,easynet_mac_addr,6); break; // for us
    }
  memcpy(copyto+6,memory,words*4);
  /* Pass the packet up to the protocol layers. */
  NetReceive(NetRxPackets[0], len);
  return len;

}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int eth_send(volatile void *packet, int length) {
  int i;
  int  IRQ_type;
  unsigned char *data = (unsigned char *) packet;
  u32 tofifo;


  if (!(read_ready_to_send)) {
    printf("easynet: TX not yet ready\n");
    return 0;
  }
  write_snd_fifo(*(u32 *)(data+0));
  write_snd_fifo((*(u32 *)(data+12) << 16) + 
  		 (*(u32 *)(data+ 4) & 0xffff));
  /* write data */
  for (i=14 ; i <= length ; i=i+4) {
    tofifo=(data[i+0]) | (data[i+1] << 8) | (data[i+2] << 16) | (data[i+3] << 24);
    write_snd_fifo(tofifo);
  }
  /* send command */
  write_snd_length(length-14);
  // wait for ready...
  i=100000;
  while (i>0) {
    IRQ_type = read_IRQ;
    if (IRQ_type &0x2) {
      write_SND_ACK(1);
      return length;
    }
  }
  // no ACK
  printf("Easynet: TX-timeout\n");
  return 0;
}
#endif /* CONFIG_DRIVER_EASYNET */
