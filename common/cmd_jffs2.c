/*
 * (C) Copyright 2002
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
 * Boot support
 */
#include <armboot.h>
#include <command.h>
#include <cmd_boot.h>
#include <cmd_autoscript.h>
#include <s_record.h>
#include <net.h>

#if (CONFIG_COMMANDS & CFG_CMD_JFFS2)

#include <jffs2/jffs2.h>
#include <jffs2/load_kernel.h>

static void
get_part_info(struct part_info *part)
{
	extern flash_info_t flash_info[];	/* info for FLASH chips */
	int i;

	memset(part, 0, sizeof(*part));
#if defined(CFG_JFFS2_FIRST_SECTOR)
	part->offset = (unsigned char *) flash_info[CFG_JFFS2_FIRST_BANK].start[CFG_JFFS2_FIRST_SECTOR];
#else
	part->offset = (unsigned char *) flash_info[CFG_JFFS2_FIRST_BANK].start[0];
#endif

	/* Figure out flash partition size */
	for (i = CFG_JFFS2_FIRST_BANK; i < CFG_JFFS2_NUM_BANKS+CFG_JFFS2_FIRST_BANK; i++)
	    part->size += flash_info[i].size;

#if defined(CFG_JFFS2_FIRST_SECTOR) && (CFG_JFFS2_FIRST_SECTOR > 0)
	part->size -=
            flash_info[CFG_JFFS2_FIRST_BANK].start[CFG_JFFS2_FIRST_SECTOR] -
            flash_info[CFG_JFFS2_FIRST_BANK].start[0];
#endif

	/* unused in current jffs2 loader */
	part->erasesize = 0;
}

int
do_jffs2_fsload(cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	struct part_info part;
	char *filename = "vmlinuz";
	ulong offset = CFG_LOAD_ADDR;
	int size;

	if (argc >= 2) {
		offset = simple_strtoul(argv[1], NULL, 16);
	}
	if (argc == 3) {
		filename = argv[2];
	}

	get_part_info(&part);

	printf("### JFFS2 loading '%s' to 0x%lx\n", filename, offset);
	size = jffs2_1pass_load((char *)offset, &part, filename);

	if (size > 0) {
	    printf("### JFFS2 load compleate: %d bytes loaded to 0x%lx\n",
		    			size, offset);
	} else {
	    printf("### JFFS2 LOAD ERROR<%x> for %s!\n", size, filename);
	}
	return !(size > 0);
}

int
do_jffs2_ls(cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	struct part_info part;
	char *filename = "/";
	int ret;

	if (argc == 2)
	    filename = argv[1];

	get_part_info(&part);
	ret = jffs2_1pass_ls(&part, filename);

	return (ret == 1);
}

int
do_jffs2_fsinfo(cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	struct part_info part;
	int ret;

	get_part_info(&part);
	ret = jffs2_1pass_info(&part);

	return (ret == 1);
}

#endif /* CFG_CMD_JFFS2 */
