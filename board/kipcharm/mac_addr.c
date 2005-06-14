/* Set the Mac Address for the CIA2/CHARM Board. It is read out of the kernel command line */
/* or here named bootargs. Missing parameter will lead to a fallback MAC addr       */
#include "armboot.h"
#include "command.h"
#include "net.h"
#include "epxa/excalibur.h"

const unsigned char default_mac_address[6]={0x40, 0xC1, 0xA0, 0x02, 0x00, 0x00}; 

/* This sets a default MAC Address */
static void set_default_mac(unsigned char *mac) {
  memcpy(mac, default_mac_address, 6);
}

/* function searches for parameter charm_mac in string *ba and if it is */
/* in the form of charm_mac=00:11:e:FA:33:2 it writes these values in a */
/* the character string *mac. mac must be allocated. returns 0 on success */
static int charm_mac(char *ba, unsigned char *mac) {
  char *strp;
  unsigned int readval=0;
  const char pa[]="charm_mac";
  int i;
  i=strlen(ba)-strlen(pa)-12; // start at the end - 11 chars
  if (i<0) goto undo_read_ins;
  while( (strncmp(&ba[i],pa,strlen(pa))) && i) i--;
  if (i==0) goto undo_read_ins;;
  //  printf("Found %s here: %d = %s\n",pa,i,&ba[i]);
  //  printf("MAC: %s\n",&ba[i+strlen(pa)+1]);

  strp=&ba[i+strlen(pa)+1];

  for (i=0; i<6; i++) {

    readval=simple_strtoul(strp,NULL,16);
    if (readval > 255) {
      printf ("Error reading Element %d of MAC\n",i+1);
      goto undo_read_ins;
    }
    mac[i]=(unsigned char) readval;
    if (i != 5) {
      strp++;
      if (strp[0] == '\0') { printf ("MAC too short\n"); goto undo_read_ins;}
      if (strp[0] != ':') strp++;      
      if (strp[0] == '\0') { printf ("MAC too short\n"); goto undo_read_ins;}
      if (strp[0] != ':')  { printf ("MAC Element %d too long\n",i+1); goto undo_read_ins;}
      strp++; 
    }
  }
  return 0;
 undo_read_ins:
  set_default_mac(mac);
  return -1;
}

/////////////////////////////////////
void kipcharm_set_mac_addr(bd_t* bd){


  char addr[6];
  char buf[18];
  char* s;


  set_default_mac(addr); 
  // look for the charm_mac string in kernel cmdline
  s = getenv(bd, "bootargs");
  if (s) {
    if (charm_mac(s,addr) != 0) { // no or wrong mac in bootargs
      printf ("Add the parameter charm_mac=xx:xx:xx:xx:xx:xx to bootargs!\n");
  }
  } else { // no bootargs !? #$@ 
    printf ("Add the parameter charm_mac=xx:xx:xx:xx:xx:xx to bootargs!\n");
  }

  sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3],
                                                   addr[4], addr[5]);
  setenv (bd, "ethaddr", buf);
}








