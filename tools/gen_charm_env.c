/* Generate Environment Block for armboot */
/* Create a ready to download flash-file: */
/* ./gen_env templ 500 | srec_cat - -binary -offset 0x30000 -o out.hex -intel */

/* template file could look like this: */

/* bootargs=root=/dev/mtdblock/3 mem=30M rootfstype=jffs2 console=ttyUA0,38600 easynet_mac=EA:54:5C:5B:XX:XX */
/* bootcmd=bootm 40050000 */
/* bootdelay=3 */
/* baudrate=57600 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define	__ASSEMBLY__	/* Dirty trick to get only #defines */
#include <config.h>
#undef	__ASSEMBLY__

#if defined(CFG_ENV_IS_IN_FLASH)
# ifndef  CFG_ENV_ADDR
#  define CFG_ENV_ADDR	(CFG_FLASH_BASE + CFG_ENV_OFFSET)
# endif
# ifndef  CFG_ENV_OFFSET
#  define CFG_ENV_OFFSET (CFG_ENV_ADDR - CFG_FLASH_BASE)
# endif
# ifndef  CFG_ENV_SIZE
#  define CFG_ENV_SIZE	CFG_ENV_SECT_SIZE
# endif
# if (CFG_ENV_ADDR >= CFG_MONITOR_BASE) && \
     (CFG_ENV_ADDR+CFG_ENV_SIZE) <= (CFG_MONITOR_BASE + CFG_MONITOR_LEN)
#  define ENV_IS_EMBEDDED
# endif
#endif	/* CFG_ENV_IS_IN_FLASH */



extern int errno;
extern unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);

int main (int argc, char **argv) {
  int i;
  int pos=0;
  int not_found=0;
  int hits=0;
  unsigned long crc;
  const int size=CFG_ENV_SIZE - sizeof(ulong);
  char buf[size];
  char *bp=buf;
  FILE *fp;
  char filename[256];
  char outfile[256];
  int writefile=0;
  char * mac,*boardname;
  unsigned int boardnumber;
  const char searchstring[]="charm_mac=";
  const char boardname_string[]="board_name=";
  // const int XXposition=25; // hardcoded Position of the first X of XX:XX

  memset(buf,255,size);


  if (argc < 2) { // Minimum 1 Args
    printf("Usage: %s <template file> [ <outfile> ]\n",argv[0]);
    printf("mac_address: MAC address of the SMC ethernet chip.Format XX:XX:XX:XX \n");
    printf("If you omit outfile, Output will be written to stdout\n");
    return -1;
  }


  strncpy(filename,argv[1],256);  // get template filename
  filename[255]=0;

  if (argc > 2) { // We write to outfile not to STDOUT
    strncpy(outfile,argv[2],256);
    outfile[255]=0;
    writefile=1;
    printf("Template file:%s\n",filename);
    printf("Output   file:%s\n",outfile);
  }

  fp=fopen(filename,"r");  // Open template file
  if (fp==NULL) {
    printf("Could not open %s\n",filename);
    return -1;
  }

  //////////////////////////////
  // Read in environment file 
  //////////////////////////////

  pos=0;
  while (fgets(bp,size,fp) != 0) {  // read in template file
    //    printf("%d: %s\n",pos,bp);
    pos+=strlen(bp);
    bp+=strlen(bp);
    if (buf[pos-1]=='\n') buf[pos-1]=0;
  }
  fclose(fp);



  if (writefile) {  for (i=0;i<pos+1;i++) if (buf[i]==0) printf("\n"); else printf("%c",buf[i]);}

  crc=crc32(0,buf,sizeof(buf)); // calculate CRC
  if (writefile)   printf("CRC: 0x%08lx\n",crc);

  if (writefile) {  // Write output file
    fp=fopen(outfile,"w");
    if (fp == NULL) { 
      printf ("Could not open %s for writing\n",outfile);
      return -1;
    }
    printf("Write CRC: %d\n",fwrite(&crc,sizeof(unsigned int),1,fp)); // first 4 Byte CRC
    printf("Write Buf: %d\n",fwrite(buf,sizeof(char),size,fp));  // Then remaining env+zeros

    perror("Output file");
    fclose(fp);
  }  else {  // Write to STDOUT
    bp=(unsigned char *) &crc;
    for(i=0;i<4;   i++) printf("%c",bp[i]);  // first 4 Byte CRC
    for(i=0;i<size;i++) printf("%c",buf[i]);  // Then remaining env+zeros
  }
  return 0;
}
