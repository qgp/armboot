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

/*
 * FLASH support
 */
#include <armboot.h>
#include <command.h>
#include <cmd_boot.h>

#if (CONFIG_COMMANDS & CFG_CMD_FLASH)

extern flash_info_t flash_info[];	/* info for FLASH chips */

/*
 * The user interface starts numbering for Flash banks with 1
 * for historical reasons.
 */

/*
 * this routine looks for an abbreviated flash range specification.
 * the syntax is B:SF[-SL], where B is the bank number, SF is the first
 * sector to erase, and SL is the last sector to erase (defaults to SF).
 * bank numbers start at 1 to be consistent with other specs, sector numbers
 * start at zero.
 *
 * returns:	1	- correct spec; *pinfo, *psf and *psl are
 *			  set appropriately
 *		0	- doesn't look like an abbreviated spec
 *		-1	- looks like an abbreviated spec, but got
 *			  a parsing error, a number out of range,
 *			  or an invalid flash bank.
 */
static int
abbrev_spec(char *str, flash_info_t **pinfo, int *psf, int *psl)
{
    flash_info_t *fp;
    int bank, first, last;
    char *p, *ep;
    
    if ((p = strchr(str, ':')) == NULL)
      return 0;
    *p++ = '\0';
    
    bank = simple_strtoul(str, &ep, 10);
    if (ep == str || *ep != '\0' ||
	bank < 1 || bank > CFG_MAX_FLASH_BANKS ||
	(fp = &flash_info[bank - 1])->flash_id == FLASH_UNKNOWN)
      return -1;
    
    str = p;
    if ((p = strchr(str, '-')) != NULL)
      *p++ = '\0';
    
    first = simple_strtoul(str, &ep, 10);
    if (ep == str || *ep != '\0' || first >= fp->sector_count)
      return -1;
    
    if (p != NULL) {
	last = simple_strtoul(p, &ep, 10);
	if (ep == p || *ep != '\0' ||
	    last < first || last >= fp->sector_count)
	  return -1;
    }
    else
      last = first;
    
    *pinfo = fp;
    *psf = first;
    *psl = last;
    
    return 1;
}

int do_flinfo (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
    ulong bank;
    
    if (argc == 1) {	/* print info for all FLASH banks */
	for (bank=0; bank <CFG_MAX_FLASH_BANKS; ++bank) {
	    printf ("\nBank # %ld: ", bank+1);
	    
	    flash_print_info (&flash_info[bank]);
	}
	return 0;
    }
    
    bank = simple_strtoul(argv[1], NULL, 16);
    if ((bank < 1) || (bank > CFG_MAX_FLASH_BANKS)) {
	printf ("Only FLASH Banks # 1 ... # %d supported\n",
		CFG_MAX_FLASH_BANKS);
	return 1;
    }
    printf ("\nBank # %ld: ", bank);
    flash_print_info (&flash_info[bank-1]);
    return 0;
}

int do_flerase(cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
    flash_info_t *info;
    ulong bank, addr_first, addr_last;
    int n, sect_first, sect_last;
    
    if (argc < 2) {
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }
    
    if (strcmp(argv[1], "all") == 0) {
	for (bank=1; bank<=CFG_MAX_FLASH_BANKS; ++bank) {
	    printf ("Erase Flash Bank # %ld:\n", bank);
	    info = &flash_info[bank-1];
	    n = flash_erase (info, 0, info->sector_count-1);
	    if (n < 0) {
		flash_perror(n);
		return 1;
	    }
	    else
	      printf("Done.\n");
	}
	return 0;
    }
    
    if ((n = abbrev_spec(argv[1], &info, &sect_first, &sect_last)) != 0) {
	if (n < 0) {
	    printf("Bad sector specification\n");
	    return 1;
	}
	printf ("Erase Flash Sectors %d-%d in Bank # %d:\n",
		sect_first, sect_last, (info-flash_info)+1);
	n = flash_erase(info, sect_first, sect_last);
	if (n < 0) {
	  flash_perror(n);
	  return 1;
	}
	else
	  printf("Done.\n");
	return 0;
    }
    
    if (argc != 3) {
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }
    
    if (strcmp(argv[1], "bank") == 0) {
	bank = simple_strtoul(argv[2], NULL, 16);
	if ((bank < 1) || (bank > CFG_MAX_FLASH_BANKS)) {
	    printf ("Only FLASH Banks # 1 ... # %d supported\n",
		    CFG_MAX_FLASH_BANKS);
	    return 1;
	}
	printf ("Erase Flash Bank # %ld:\n", bank);
	info = &flash_info[bank-1];
	n = flash_erase (info, 0, info->sector_count-1);
	if (n < 0) {
	  flash_perror(n);
	  return 1;
	}
	else
	  printf("done.\n");
	return 0;
    }
    
    addr_first = simple_strtoul(argv[1], NULL, 16);
    addr_last  = simple_strtoul(argv[2], NULL, 16);
    
    if (addr_first >= addr_last) {
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }
    
    printf ("Erase Flash from 0x%08lx to 0x%08lx... ", addr_first, addr_last);
    n = flash_sect_erase(addr_first, addr_last);
    if (n < 0) {
      flash_perror(n);
      return 1;
    }
    else
      printf("done.\nErased %d sectors.\n", n);
    return 0;
}

int do_protect(cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
    flash_info_t *info;
    ulong bank, addr_first, addr_last;
    int i, p, n, sect_first, sect_last;
    
    if (argc < 3) {
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }
    
    if (strcmp(argv[1], "off") == 0)
      p = 0;
    else if (strcmp(argv[1], "on") == 0)
      p = 1;
    else {
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }
    
    if (strcmp(argv[2], "all") == 0) {
	for (bank=1; bank<=CFG_MAX_FLASH_BANKS; ++bank) {
	    info = &flash_info[bank-1];
	    if (info->flash_id == FLASH_UNKNOWN) {
		continue;
	    }
	    printf ("%sProtect Flash Bank # %ld\n",
		    p ? "" : "Un-", bank);
	    
	    for (i=0; i<info->sector_count; ++i) {
		info->protect[i] = p;
	    }
	}
	return 0;
    }
    
    if ((n = abbrev_spec(argv[2], &info, &sect_first, &sect_last)) != 0) {
	if (n < 0) {
	    printf("Bad sector specification\n");
	    return 1;
	}
	printf("%sProtect Flash Sectors %d-%d in Bank # %d\n",
	       p ? "" : "Un-", sect_first, sect_last,
	       (info-flash_info)+1);
	for (i = sect_first; i <= sect_last; i++) {
	    info->protect[i] = p;
	}
	return 0;
    }
    
    if (argc != 4) {
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }
    
    if (strcmp(argv[2], "bank") == 0) {
	bank = simple_strtoul(argv[3], NULL, 16);
	if ((bank < 1) || (bank > CFG_MAX_FLASH_BANKS)) {
	    printf ("Only FLASH Banks # 1 ... # %d supported\n",
		    CFG_MAX_FLASH_BANKS);
	    return 1;
	}
	printf ("%sProtect Flash Bank # %ld\n",
		p ? "" : "Un-", bank);
	info = &flash_info[bank-1];
	
	if (info->flash_id == FLASH_UNKNOWN) {
	    printf ("missing or unknown FLASH type\n");
	    return 1;
	}
	for (i=0; i<info->sector_count; ++i) {
	    info->protect[i] = p;
	}
	return 0;
    }
    
    addr_first = simple_strtoul(argv[2], NULL, 16);
    addr_last  = simple_strtoul(argv[3], NULL, 16);
    
    if (addr_first >= addr_last) {
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }
    
    return flash_sect_protect (p, addr_first, addr_last) ? 1 : 0;
}

int flash_sect_protect (int p, ulong addr_first, ulong addr_last)
{
    flash_info_t *info;
    ulong bank;
    int s_first, s_last;
    int protected, i;
    int rc = ERR_OK;
    
    protected = 0;
    
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
	    protected += s_last - s_first + 1;
	    for (i=s_first; i<=s_last; ++i) {
		info->protect[i] = p;
	    }
	}
    }
    if (protected) {
	printf ("%sProtected %d sectors\n",
		p ? "" : "Un-", protected);
    } else {
	rc = ERR_ALIGN;
	flash_perror(rc);
    }
    return rc;
}

#endif	/* CFG_CMD_FLASH */
