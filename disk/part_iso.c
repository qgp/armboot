/*
 * (C) Copyright 2001
 * Denis Peter, MPL AG Switzerland, d.peter@mpl.ch.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <armboot.h>
#include <command.h>
#include <cmd_disk.h>
#include "part_iso.h"

#if ((CONFIG_COMMANDS & CFG_CMD_IDE) || (CONFIG_COMMANDS & CFG_CMD_SCSI)) && defined(CONFIG_ISO_PARTITION)

#undef	ISO_PART_DEBUG

#ifdef	ISO_PART_DEBUG
#define	PRINTF(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF(fmt,args...)
#endif

#define CD_SECTSIZE 2048

static unsigned char tmpbuf[CD_SECTSIZE];
 
/* Convert char[4] in little endian format to the host format integer
 */
static inline unsigned long le32_to_int(unsigned char *le32)
{
    return ((le32[3] << 24) +
	    (le32[2] << 16) +
	    (le32[1] << 8) +
	     le32[0]
	   );
}

/* only boot records will be listed as valid partitions */ 
int get_partition_info_iso_verb(block_dev_desc_t * dev_desc, int part_num, disk_partition_t * info, int verb)
{
	int i,offset,entry_num;
	unsigned short *chksumbuf;
	unsigned short chksum;
	unsigned long newblkaddr,blkaddr,lastsect,bootaddr;
	iso_boot_rec_t *pbr = (iso_boot_rec_t	*)tmpbuf; /* boot record */
	iso_pri_rec_t *ppr = (iso_pri_rec_t	*)tmpbuf;	/* primary desc */
	iso_val_entry_t *pve = (iso_val_entry_t *)tmpbuf;
	iso_init_def_entry_t *pide;	
	iso_header_entry_t *phe;	

	/* the first sector (sector 0x10) must be a primary volume desc */
	blkaddr=PVD_OFFSET;
	if (dev_desc->block_read (dev_desc->dev, PVD_OFFSET, 1, (ulong *) tmpbuf) != 1)
   	return (-1);
	if(ppr->desctype!=0x01) {
		if(verb)
			printf(" first descriptor is NOT a primary desc\n");
		return (-1);
	}
	if(strncmp(ppr->stand_ident,"CD001",5)!=0) {
		if(verb)
			printf("ISO Ident: %s wrong\n",ppr->stand_ident);
		return (-1);
	}
	lastsect= ((ppr->firstsek_LEpathtab1_LE & 0xff)<<24) +
						((ppr->firstsek_LEpathtab1_LE & 0xff00)<<8) +
						((ppr->firstsek_LEpathtab1_LE & 0xff0000)>>8) +
						((ppr->firstsek_LEpathtab1_LE & 0xff000000)>>24);
	info->blksz=ppr->secsize_BE; /* assuming same block size for all entries */
	PRINTF(" Lastsect:%08lx\n",lastsect);
	for(i=blkaddr;i<lastsect;i++) {
		if (dev_desc->block_read (dev_desc->dev, i, 1, (ulong *) tmpbuf) != 1)
  	 	return (-1);
		if(ppr->desctype==0x00)
			break; /* boot entry found */
		if(ppr->desctype==0xff) {
			if(verb)
				printf(" No valid boot catalog found\n");
			return (-1);
		}
	}
 	/* boot entry found */
	if(strncmp(pbr->ident_str,"EL TORITO SPECIFICATION",23)!=0) {
		if(verb)
			printf("Ident: %s wrong\n",pbr->ident_str);
		return (-1);
	}
	bootaddr=le32_to_int(pbr->pointer);
	PRINTF(" Boot Entry at: %08lX\n",bootaddr);
	if (dev_desc->block_read (dev_desc->dev, bootaddr, 1, (ulong *) tmpbuf) != 1)
		return (-1);
	chksum=0;
	chksumbuf = (unsigned short *)tmpbuf;
	for(i=0;i<0x10;i++)
		chksum+=((chksumbuf[i] &0xff)<<8)+((chksumbuf[i] &0xff00)>>8);
	if(chksum!=0) {
		if(verb)
			printf(" checksum Error in booting catalog validation entry\n");
		return (-1);
	}
	if((pve->key[0]!=0x55)||(pve->key[1]!=0xAA)) {
		if(verb)
			printf(" key 0x55 0xAA error\n");
		return(-1);
	}
	/* the validation entry seems to be ok, now search the "partition" */
	entry_num=0;
	offset=0x20;
	if(part_num==0) { /* the default partition */			
		#if 0
		if(pve->platform!=0x01) {
			if(verb)
				printf(" default entry is no PowerPC partition\n");
			return(-1);
		}
		#endif
		sprintf(info->name,"%.32s",pve->manu_str); 
		sprintf (info->type, "ARMBoot");
		pide=(iso_init_def_entry_t *)&tmpbuf[offset];
		goto found;
	}
	/* partition is not the default partition */
	offset+=0x20;
	while(1) {
		phe=(iso_header_entry_t *)&tmpbuf[offset];
		if((phe->header_id & 0xfe)!=0x90) { /* no valid header ID */
			if(verb)
				printf(" part %d not found\n",part_num);
			return(-1);
		}
		if((entry_num+(phe->numentry[1]<<8)+phe->numentry[0])>=part_num) { /* partition lies in this header */
			#if 0
			if(phe->platform!=0x01) {
				if(verb)
					printf(" part %d is no PowerPC partition\n",part_num);
				return(-1);
			}
			#endif
			sprintf(info->type,"%.32s",phe->id_str);
			entry_num++;
			offset+=0x20;
			while(entry_num!=part_num) {
				offset+=0x20;
				while(tmpbuf[offset]==0x44)
					offset+=0x20; /* skip extension records */
				entry_num++;
			}
			/* part entry should be here */
			pide=(iso_init_def_entry_t *)&tmpbuf[offset];
			goto found;
		}
		else { /* search next header */
			if((phe->header_id)==0x91) { /* last header ID */
				if(verb)
					printf(" part %d not found\n",part_num);
				return(-1);
			}
			entry_num+=((phe->numentry[1]<<8)+phe->numentry[0]); /* count partitions */
			offset+=0x20;
			while((tmpbuf[offset]=0x44) || /* skip extension records */
						(tmpbuf[offset]=0x88) || /* skip boot entries */
						(tmpbuf[offset]=0x00)) {	 /* skip no boot entries */
				offset+=0x20;
				if(offset>=CD_SECTSIZE) { /* to prevent overflow */
					if(verb)
						printf(" part %d not found\n",part_num);
					return(-1);
				}
			} /* while skipping all unused records */
		} /* else */
	} /* while(TRUE) */
found:				
	if(pide->boot_ind!=0x88) {
		if(verb)
			printf(" part %d is not bootable\n",part_num);
		return (-1);
	}
	switch(pide->boot_media) {
		case 0x00:	info->size=2880>>2; break; /* dummy (No Emulation) */
		case 0x01:	info->size=2400>>2; break; /* 1.2MByte Floppy */
		case 0x02:	info->size=2880>>2; break; /* 1.44MByte Floppy */
		case 0x03:	info->size=5760>>2; break; /* 2.88MByte Floppy */
		case 0x04:	info->size=2880>>2; break; /* dummy (HD Emulation) */
		default:		info->size=0; break;
	}
	newblkaddr=le32_to_int(pide->rel_block_addr);
	info->start=newblkaddr;
	if(verb)
		printf(" part %d found @ %lx size %lx\n",part_num,newblkaddr,info->size);
	return 0;
}		

int get_partition_info_iso(block_dev_desc_t * dev_desc, int part_num, disk_partition_t * info)
{
	return(get_partition_info_iso_verb(dev_desc, part_num, info, 0));
}

						
												
void print_part_iso(block_dev_desc_t * dev_desc)
{
	disk_partition_t info;
	int i;
	if(get_partition_info_iso_verb(dev_desc,0,&info,0)==-1) {
		printf("** No boot partition found on device %d **\n",dev_desc->dev);
		return;
	}
	printf("Part   Start     Sect x Size Type\n"); 
	i=0;
	do {
		printf(" %2d %8ld %8ld %6ld %.32s\n",i,info.start,info.size,info.blksz,info.type);
		i++;
	}while(get_partition_info_iso_verb(dev_desc,i,&info,0)!=-1);
}

int test_part_iso (block_dev_desc_t *dev_desc)
{
	disk_partition_t info;

	return(get_partition_info_iso_verb(dev_desc,0,&info,0));
}

#endif /* ((CONFIG_COMMANDS & CFG_CMD_IDE) || (CONFIG_COMMANDS & CFG_CMD_SCSI)) && defined(CONFIG_ISO_PARTITION) */


