/*
 * (C) Copyright 2001
 * Kyle Harris, kharris@nexus-tech.net
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
	autoscript allows a remote host to download a command file and, optionally,
	binary data for automatically updating the target. For example, you create
	a new kernel image and want the user to be able to simply download the 
	image and the machine does the rest. The kernel image is postprocessed 
	with mkimage, which creates an image with a script file prepended. If
	enabled, autoscript will verify the script and contents of the download
	and execute the script portion. This would be responsible for erasing
	flash, copying the new image, and rebooting the machine.
*/

#include <armboot.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <cmd_boot.h>
#include <cmd_autoscript.h>

#if defined(CONFIG_AUTOSCRIPT) || \
	 (CONFIG_COMMANDS & CFG_CMD_AUTOSCRIPT)

extern image_header_t header;           /* from cmd_bootm.c */

void
autoscript(bd_t *bd, ulong addr)
{
	ulong crc, data, len;
	image_header_t *hdr = &header;
	ulong *len_ptr;
	char *cmd = 0;

	memcpy (hdr, (char *)addr, sizeof(image_header_t));

	if (SWAP32(hdr->ih_magic) != IH_MAGIC)
	{
		printf("Bad magic number\n");
		return;
	}

	crc = SWAP32(hdr->ih_hcrc);
	hdr->ih_hcrc = 0;
	len = sizeof(image_header_t);
	data = (ulong)hdr;
	if (crc32(0, (char *)data, len) != crc)
	{
		printf("Bad header crc\n");
		return;
	}

	data = addr + sizeof(image_header_t);
	len = SWAP32(hdr->ih_size);
	if (crc32(0, (char *)data, len) != SWAP32(hdr->ih_dcrc))
	{
		printf("Bad data crc\n");
		return;
	}

	if (hdr->ih_type != IH_TYPE_SCRIPT)
	{
		printf("Bad image type\n");
		return;
	}

	/* get len of script and make sure cmd is null terminated */
	len_ptr = (ulong *)data;
	len = *len_ptr;
	if (len)
	{
		int i;

		cmd = malloc (len + 1);
		if (!cmd)
		{
			return;
		}
		while (*len_ptr++);
		memcpy(cmd,(char *)len_ptr,len);
		*(cmd + len) = 0;
		/* also (for now) replace \n with ; */
		for (i=0; i<len; i++)
		{
			if (cmd[i] == '\n')
			{
				cmd[i] = ';';
			}
		}
	}
	run_command(cmd, bd, 0);
	free(cmd);
}

#endif

#if (CONFIG_COMMANDS & CFG_CMD_AUTOSCRIPT)

void 
do_autoscript (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	ulong addr;

	if (argc < 2)
	{
		addr = CFG_LOAD_ADDR;
	}
	else
	{
		addr = simple_strtoul(argv[1],0,16);
	}

	printf("## Executing script at %08lx\n",addr);
		
	autoscript(bd,addr);

}
#endif

