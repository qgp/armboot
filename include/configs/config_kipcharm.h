/*
 * Copyright (c) 2000-2002 Altera Corporation, San Jose, California, USA.  
 * All rights reserved.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * Configuation settings for the EPXA1 Development Board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

#include "epxa/excalibur.h"

/*
 * If we are developing, we might want to start armboot from ram
 * so we MUST NOT initialize critical regs like mem-timing ...
 * However at the moment, we're booting from flash.
 */

#define CONFIG_INIT_CRITICAL		/* undef for developing */

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_ARM922		1	/* This is an ARM922 CPU	*/
#define CONFIG_EPXA1DB		1	/* epxa1 developemnt board      */

#undef CONFIG_USE_IRQ			/* we don't need IRQ/FIQ stuff */

/*
 * Size of malloc() pool
 */
#define CONFIG_MALLOC_SIZE	(CFG_ENV_SIZE + 128*1024)

#define CONFIG_NO_IDE
#define CONFIG_NO_PCMCIA

/*
 * Hardware drivers
 */


#define CONFIG_DRIVER_SMC91111
#define CONFIG_SMC91111_BASE (EXC_EBI_BLOCK3_BASE + 0x300)
#define CONFIG_EPXA1DB_MAC_ADDR

/*
 * select serial console configuration
 */

/* allow to overwrite serial and ethaddr */

#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE		38400

#define CONFIG_COMMANDS		CONFIG_CMD_DFL

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY	3

//#define CONFIG_BOOTARGS    	"root=/dev/mtdblock/0 mem=32M rootfstype=jffs2 console=ttyUA0,57600 ip=139.6.18.59:139.6.18.51:139.6.18.1:255.255.255.0:epxa1db:eth0:off"
//#define CONFIG_BOOTARGS    	"root=/dev/nfs mem=32M console=ttyUA0,57600 nfsroot=10.0.0.1:/tftpboot/dcsrootfs ip=10.0.0.10:10.0.0.1:10.0.0.1:255.255.255.0:dcs:eth0:off"
#define CONFIG_BOOTARGS    	"root=/dev/mtdblock3 mem=30M rootfstype=jffs2 console=ttyUA0,38400"
#define CONFIG_NETMASK          255.255.255.0
#define CONFIG_IPADDR		137.57.134.44
#define CONFIG_SERVERIP		137.57.134.151
#define CONFIG_BOOTFILE		"vmlinux.exe"
/* #define CONFIG_BOOTCOMMAND	"tftp;bootm" */
#define CONFIG_BOOTCOMMAND	"bootm 40050000"

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400		/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2		/* which serial port to use */
#endif

/*
 * Miscellaneous configurable options
 */

#define	CFG_LONGHELP				/* undef to save memory		*/
#define	CFG_PROMPT		"charm # "	/* Monitor Command Prompt	*/
#define	CFG_CBSIZE		256		/* Console I/O Buffer Size	*/
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define	CFG_MAXARGS		16		/* max number of command args	*/
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size	*/

#define CFG_MEMTEST_START	0x00400000	/* memtest works on	*/
#define CFG_MEMTEST_END		0x00600000	/* 4 ... 6 MB in DRAM	*/

#undef  CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define	CFG_LOAD_ADDR		0xA00000	/* default load address	*/

#define CFG_HZ                  EXC_AHB2_CLK_FREQUENCY
						/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CONFIG_DOS_PARTITION	0		/* DOS partitiion support */

#ifndef __ASSEMBLY__
/*-----------------------------------------------------------------------
 * Board specific extension for bd_info
 *
 * This structure is embedded in the global bd_info (bd_t) structure
 * and can be used by the board specific code (eg board/...)
 */

struct bd_info_ext
{
    /* helper variable for board environment handling
     *
     * env_crc_valid == 0    =>   uninitialised
     * env_crc_valid  > 0    =>   environment crc in flash is valid
     * env_crc_valid  < 0    =>   environment crc in flash is invalid
     */
     int	env_crc_valid;
};
#endif

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */

#define CONFIG_NR_DRAM_BANKS 1

#define PHYS_FLASH_1		EXC_EBI_BLOCK0_BASE /* Flash Bank #1 */
//#define PHYS_FLASH_2            EXC_EBI_BLOCK1_BASE /* FLASH Bank #2 */

#define CFG_FLASH_BASE		PHYS_FLASH_1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
//#define CFG_MAX_FLASH_BANKS	2	/* max number of memory banks		*/
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
//#define CFG_MAX_FLASH_SECT	(63+8)	/* max number of sectors on one chip	*/
#define CFG_MAX_FLASH_SECT	(127+8)	/* max number of sectors on one chip	*/

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(2*EXC_AHB2_CLK_FREQUENCY) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(2*EXC_AHB2_CLK_FREQUENCY) /* Timeout for Flash Write */

/* 
 * we put the env storage below offset 0x180000 so as to leave space for the linux flash
 * file system.
 */

#define CFG_ENV_SIZE            0x10000
#define CFG_ENV_ADDR		(EXC_EBI_BLOCK0_BASE + 0x50000 - CFG_ENV_SIZE) /* Addr of 
Environment Sector	*/

#endif	/* __CONFIG_H */




