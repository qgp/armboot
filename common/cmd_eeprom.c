/*
 * (C) Copyright 2000, 2001
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
 *
 */

#include <armboot.h>
#include <config.h>
#include <command.h>

#if (CONFIG_COMMANDS & CFG_CMD_EEPROM) || defined(CFG_ENV_IS_IN_EEPROM)

extern void eeprom_init  (void);
extern void eeprom_read  (unsigned offset, uchar *buffer, unsigned cnt);
extern void eeprom_write (unsigned offset, uchar *buffer, unsigned cnt);

#endif

/* ------------------------------------------------------------------------- */

#if (CONFIG_COMMANDS & CFG_CMD_EEPROM)

void do_eeprom (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
    switch (argc) {
    default:
	printf ("Usage:\n%s\n", cmdtp->usage);
	return;
    case 5:
	/* 4 args */

	if (strcmp(argv[1],"read") == 0) {
		ulong addr = simple_strtoul(argv[2], NULL, 16);
		ulong off  = simple_strtoul(argv[3], NULL, 16);
		ulong cnt  = simple_strtoul(argv[4], NULL, 16);

		printf ("\nEEPROM read: addr %08lx  off %04lx  count %ld ... ",
			addr, off, cnt);

#ifndef CONFIG_SPI
		eeprom_init ();
#endif
		eeprom_read (off, (uchar *)addr, cnt);

		printf ("done\n");
		return;

	} else if (strcmp(argv[1],"write") == 0) {
		ulong addr = simple_strtoul(argv[2], NULL, 16);
		ulong off  = simple_strtoul(argv[3], NULL, 16);
		ulong cnt  = simple_strtoul(argv[4], NULL, 16);

		printf ("\nEEPROM write: addr %08lx  off %04lx  count %ld ... ",
			addr, off, cnt);

#ifndef CONFIG_SPI
		eeprom_init ();
#endif
		eeprom_write(off, (uchar *)addr, cnt);

		printf ("done\n");
		return;

	} else {
		printf ("Usage:\n%s\n", cmdtp->usage);
	}

	return;
    }
}
#endif	/* CFG_CMD_EEPROM */

/*-----------------------------------------------------------------------
 *
 * for CONFIG_I2C_X defined (16-bit EEPROM address) offset is
 *   0x000nxxxx for EEPROM address selectors at n, offset xxxx in EEPROM.
 *
 * for CONFIG_I2C_X not defined (8-bit EEPROM page address) offset is
 *   0x00000nxx for EEPROM address selectors and page number at n.
 */

#if (CONFIG_COMMANDS & CFG_CMD_EEPROM) || defined(CFG_ENV_IS_IN_EEPROM)

void eeprom_read (unsigned offset, uchar *buffer, unsigned cnt)
{
	unsigned end = offset + cnt;
	unsigned blk_off;

	/* Read data until done or would cross a page boundary.
	 * We must write the address again when changing pages
	 * because the next page may be in a different device.
	 */
	while (offset < end) {
		unsigned alen, len, maxlen;
#if !defined(CONFIG_I2C_X) && !defined(CONFIG_SPI_X)
		uchar addr[2];

		blk_off = offset & 0xFF;	/* block offset */

		addr[0] = offset >> 8;		/* block number */
		addr[1] = blk_off;		/* block offset */
		alen	= 2;
#else
		uchar addr[3];

		blk_off = offset & 0xFF;	/* block offset */

		addr[0] = offset >> 16;		/* block number */
		addr[1] = offset >>  8;		/* upper address octet */
		addr[2] = blk_off;		/* lower address octet */
		alen	= 3;
#endif	/* CONFIG_I2C_X, CONFIG_SPI_X */

#ifdef CFG_I2C_EEPROM_ADDR
		addr[0] |= CFG_I2C_EEPROM_ADDR;	/* insert device address */
#endif

		maxlen = 0x100 - blk_off;
		if (maxlen > I2C_RXTX_LEN)
			maxlen = I2C_RXTX_LEN;
		len    = end - offset;
		if (len > maxlen)
			len = maxlen;

#ifdef CONFIG_SPI
		spi_read (addr, alen, buffer, len);
#else
		i2c_read (addr, alen, buffer, len);
#endif
		buffer += len;
		offset += len;
	}
}

/*-----------------------------------------------------------------------
 *
 * for CONFIG_I2C_X defined (16-bit EEPROM address) offset is
 *   0x000nxxxx for EEPROM address selectors at n, offset xxxx in EEPROM.
 *
 * for CONFIG_I2C_X not defined (8-bit EEPROM page address) offset is
 *   0x00000nxx for EEPROM address selectors and page number at n.
 */

void eeprom_write (unsigned offset, uchar *buffer, unsigned cnt)
{
	unsigned end = offset + cnt;
	unsigned blk_off;

	/* Write data until done or would cross a write page boundary.
	 * We must write the address again when changing pages
	 * because the address counter only increments within a page.
	 */

	while (offset < end) {
		unsigned alen, len, maxlen;
#if !defined(CONFIG_I2C_X) && !defined(CONFIG_SPI_X)
		uchar addr[2];

		blk_off = offset & 0xFF;	/* block offset */

		addr[0] = offset >> 8;		/* block number */
		addr[1] = blk_off;		/* block offset */
		alen	= 2;
#else
		uchar addr[3];

		blk_off = offset & 0xFF;	/* block offset */

		addr[0] = offset >> 16;		/* block number */
		addr[1] = offset >>  8;		/* upper address octet */
		addr[2] = blk_off;		/* lower address octet */
		alen	= 3;
#endif	/* CONFIG_I2C_X, CONFIG_SPI_X */

#ifdef CFG_I2C_EEPROM_ADDR
		addr[0] |= CFG_I2C_EEPROM_ADDR;	/* insert device address */
#endif

#if defined(CFG_EEPROM_PAGE_WRITE_BITS)

#define	EEPROM_PAGE_SIZE	(1 << CFG_EEPROM_PAGE_WRITE_BITS)
#define	EEPROM_PAGE_OFFSET(x)	((x) & (EEPROM_PAGE_SIZE - 1))

		maxlen = EEPROM_PAGE_SIZE - EEPROM_PAGE_OFFSET(blk_off);
#else
		maxlen = 0x100 - blk_off;
#endif
		if (maxlen > I2C_RXTX_LEN)
			maxlen = I2C_RXTX_LEN;

		len = end - offset;
		if (len > maxlen)
			len = maxlen;

#ifdef CONFIG_SPI
		spi_write (addr, alen, buffer, len);
#else
		i2c_write (addr, alen, buffer, len);
#endif
		buffer += len;
		offset += len;

#if defined(CFG_EEPROM_PAGE_WRITE_DELAY_MS)
		udelay(CFG_EEPROM_PAGE_WRITE_DELAY_MS * 1000);
#endif
	}
}

/*-----------------------------------------------------------------------
 * Set default values
 */
#ifndef	CFG_I2C_SPEED
#define	CFG_I2C_SPEED	50000
#endif

#ifndef	CFG_I2C_SLAVE
#define	CFG_I2C_SLAVE	0xFE
#endif

void eeprom_init  (void)
{
#if defined(CONFIG_8xx) || defined(CONFIG_8260)
# if defined(CONFIG_SPI)
	spi_init_f ();
# else
	i2c_init (CFG_I2C_SPEED, CFG_I2C_SLAVE);
# endif
#endif
}
/*-----------------------------------------------------------------------
 */
#endif	/* CFG_CMD_EEPROM */
