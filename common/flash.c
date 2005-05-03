/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

#include <armboot.h>

extern flash_info_t	flash_info[CFG_MAX_FLASH_BANKS]; /* info for FLASH chips	*/

/*-----------------------------------------------------------------------
 * Set protection status for monitor sectors
 *
 * The monitor always occupies the _first_ part of the _first_ Flash bank.
 */
void flash_protect (int flag, ulong from, ulong to, flash_info_t *info)
{
    ulong b_end = info->start[0] + info->size - 1;	/* bank end address */
    short s_end = info->sector_count - 1;	/* index of last sector */
    int i;
    
    /* Do nothing if input data is bad. */
    if (info->sector_count == 0 || info->size == 0 || to < from) {
	return;
    }
    
    /* There is nothing to do if we have no data about the flash
     * or the protect range and flash range don't overlap.
     */
    if (info->flash_id == FLASH_UNKNOWN ||
	to < info->start[0] || from > b_end) {
	return;
    }
    
    for (i=0; i<info->sector_count; ++i) {
	ulong end;		/* last address in current sect	*/
	
	end = (i == s_end) ? b_end : info->start[i + 1] - 1;
	
	/* Update protection if any part of the sector
	 * is in the specified range.
	 */
	if (from <= end && to >= info->start[i]) {
	    if (flag & FLAG_PROTECT_CLEAR) {
		info->protect[i] = 0;
	    }
	    else if (flag & FLAG_PROTECT_SET) {
		info->protect[i] = 1;
	    }
	}
    }
}

/*-----------------------------------------------------------------------
 */

flash_info_t *
  addr2info (ulong addr)
{
    flash_info_t *info;
    int i;
    for (i=0, info=&flash_info[0]; i<CFG_MAX_FLASH_BANKS; ++i, ++info) {
	if (info->flash_id != FLASH_UNKNOWN &&
	    addr >= info->start[0] &&
	    /* WARNING - The '- 1' is needed if the flash
	     * is at the end of the address space, since
	     * info->start[0] + info->size wraps back to 0.
	     * Please don't change this unless you understand this.
	     */
	    addr <= info->start[0] + info->size - 1) {
	    return (info);
	}
    }
    
    return (NULL);
}

/*-----------------------------------------------------------------------
 * Copy memory to flash.
 * Make sure all target addresses are within Flash bounds,
 * and no protected sectors are hit.
 * 
 * If return code negative -> Flash Error ERR_xyz
 * Returns 0 (ERR_OK) if operation succeeded. 
 */
int flash_write (uchar *src, ulong addr, ulong cnt)
{
    int i;
    ulong         end        = addr + cnt - 1;
    flash_info_t *info_first = addr2info (addr);
    flash_info_t *info_last  = addr2info (end );
    flash_info_t *info;
    
    if (cnt == 0) {
	return (ERR_OK);
    }
    
    if (!info_first || !info_last) {
	return (ERR_INVAL);
    }
    
    for (info = info_first; info <= info_last; ++info) {
	ulong b_end = info->start[0] + info->size;	/* bank end addr */
	short s_end = info->sector_count - 1;
	for (i=0; i<info->sector_count; ++i) {
	    ulong e_addr = (i == s_end) ? b_end : info->start[i + 1];
	    
	    if ((end >= info->start[i]) && (addr < e_addr) &&
		(info->protect[i] != 0) ) {
		return (ERR_PROTECTED);
	    }
	}
    }
    
    /* finally write data to flash */
    for (info = info_first; info <= info_last && cnt>0; ++info) {
	ulong len;
	
	len = info->start[0] + info->size - addr;
	if (len > cnt)
	  len = cnt;
	if ((i = write_buff(info, src, addr, len)) != 0) {
	    return (i);
	}
	cnt  -= len;
	addr += len;
	src  += len;
    }
    return (ERR_OK);
}

/*-----------------------------------------------------------------------
 * Erase Flash sectors
 * 
 * If return code negative -> Flash Error ERR_xyz
 * If successful, returns number of erased sectors.
 */
int flash_sect_erase (ulong addr_first, ulong addr_last)
{
    flash_info_t *info;
    ulong bank;
    int s_first, s_last;
    int erased;
    int rc = ERR_OK;
   
    erased = 0;
    
    for (bank=0,info=&flash_info[0]; bank < CFG_MAX_FLASH_BANKS; ++bank, ++info) {
	ulong b_end;
	int sect;
	
	if (info->flash_id == FLASH_UNKNOWN) {
	    continue;
	}
	
	b_end = info->start[0] + info->size - 1; /* bank end addr */
	
	s_first = -1;		/* first sector to erase	*/
	s_last  = -1;		/* last  sector to erase	*/
	
	for (sect=0; sect < info->sector_count; ++sect) {
	    ulong end;		/* last address in current sect	*/
	    short s_end;
	    
	    s_end = info->sector_count - 1;
	    
	    end = (sect == s_end) ? b_end : info->start[sect + 1] - 1;
	    
	    if (addr_first > end)
	      continue;
	    if (addr_last < info->start[sect])
	      continue;
	    if (addr_first == info->start[sect]) {
		s_first = sect;
	    }
	    if (addr_last  == end) {
		s_last  = sect;
	    }
	}
	if (s_first>=0 && s_first<=s_last) {
	    erased += s_last - s_first + 1;
	    rc = flash_erase (info, s_first, s_last);
	}
	if (rc)
	  break;
    }
    return erased ? erased : rc;
}

/*-----------------------------------------------------------------------
 */

void flash_perror(int err)
{
    switch (err)
    {
    case ERR_OK:
	break;
    case ERR_TIMOUT:
	printf ("Timeout writing to Flash\n");
	break;
    case ERR_NOT_ERASED:
	printf ("Flash not Erased\n");
	break;
    case ERR_PROTECTED:
	printf ("Can't write to protected Flash sectors\n");
	break;
    case ERR_INVAL:
	printf ("Outside available Flash\n");
	break;
    case ERR_ALIGN:
	printf ("Start and/or end address not on sector boundary\n");
	break;
    case ERR_UNKNOWN_FLASH_VENDOR:
	printf ("Unknown Vendor of Flash\n");
	break;
    case ERR_UNKNOWN_FLASH_TYPE:
	printf ("Unknown Type of Flash\n");
	break;
    case ERR_PROG_ERROR:
	printf ("General Flash Programming Error\n");
	break;
    default:
	printf ("%s[%d] FIXME: rc=%d\n",__FILE__,__LINE__,err);
	break;
    }
}

/*-----------------------------------------------------------------------
 */
