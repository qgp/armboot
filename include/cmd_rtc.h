/*
 * (C) Copyright 2001
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

#ifndef _CMD_RTC_H_
#define _CMD_RTC_H_

#include <armboot.h>
#include <command.h>

#if (CONFIG_COMMANDS & CFG_CMD_DATE)

#define	CMD_TBL_DATE	MK_CMD_TBL_ENTRY(					\
	"date",	3,	2,	1,	do_date,				\
	"date    - get/set date & time\n",					\
	"[MMDDhhmm[[CC]YY][.ss]]\n"						\
	"  - without arguments: print date & time\n"				\
	"  - with argument: set the system date & time\n"			\
),

int do_date (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[]);

#else

#define CMD_TBL_DATE

#endif	/* CFG_CMD_DATE */


#endif	/* _CMD_RTC_H_ */
