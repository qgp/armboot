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

#ifndef _CMD_BSP_H_
#define _CMD_BSP_H_

#include <armboot.h>
#include <command.h>

#if (CONFIG_COMMANDS & CFG_CMD_BSP)

#ifdef CONFIG_CMD_SETCLOCK

extern int do_setclock (cmd_tbl_t *, bd_t *, int , int, char **);

#define CMD_TBL_BSP    MK_CMD_TBL_ENTRY(                   \
    "setclock", 5,  2,  2,  do_setclock,            \
    "setclock- set processor clock\n",               \
    "N\n"                                   \
    "    - set processor clock to N MHz (N is _decimal_ !!!)\n"     \
),

#endif	/* CFG_CMD_BSP */

#else

#define CMD_TBL_BSP

#endif	/* CFG_CMD_BSP */


#endif	/* _CMD_BSP_H_ */
