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
 * Memory Functions
 *
 * Copied from FADS ROM, Dan Malek (dmalek@jlc.net)
 */

#include <armboot.h>
#include <command.h>
#include <cmd_mem.h>

#if (CONFIG_COMMANDS & CFG_CMD_MEMORY)

static int mod_mem(cmd_tbl_t *, int, int, int, char *[]);

/* Display values from last command.
 * Memory modify remembered values are different from display memory.
 */
uint	dp_last_addr, dp_last_size;
uint	dp_last_length = 0x40;
uint	mm_last_addr, mm_last_size;

static	ulong	base_address = 0;

/* Memory Display
 *
 * Syntax:
 *	md{.b, .w, .l} {addr} {len}
 */
#define DISP_LINE_LEN	16

int do_mem_md    (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	ulong	addr, size, length;
	ulong	i, nbytes, linebytes;
	uchar	*cp;

	/* We use the last specified parameters, unless new ones are
	 * entered.
	 */
	addr = dp_last_addr;
	size = dp_last_size;
	length = dp_last_length;

	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if ((flag & CMD_FLAG_REPEAT) == 0) {
		/* New command specified.  Check for a size specification.
		 * Defaults to long if no or incorrect specification.
		 */
		size = 4;
		if (argv[0][2] == '.') {
			if (argv[0][3] == 'b') {
				size = 1;
			} else if (argv[0][3] == 'w') {
				size = 2;
			}
		}

		/* Address is specified since argc > 1
		*/
		addr = simple_strtoul(argv[1], NULL, 16);
		addr += base_address;

		/* If another parameter, it is the length to display.
		 * Length is the number of objects, not number of bytes.
		 */
		if (argc > 2)
			length = simple_strtoul(argv[2], NULL, 16);
	}

	/* Print the lines.
	 *
	 * We buffer all read data, so we can make sure data is read only
	 * once, and all accesses are with the specified bus width.
	 */
	nbytes = length * size;
	do {
		char	linebuf[DISP_LINE_LEN];
		uint	*uip = (uint   *)linebuf;
		ushort	*usp = (ushort *)linebuf;
		uchar	*ucp = (uchar *)linebuf;

		printf("%08lx:", addr);
		linebytes = (nbytes>DISP_LINE_LEN)?DISP_LINE_LEN:nbytes;
		for (i=0; i<linebytes; i+= size) {
			if (size == 4) {
				printf(" %08x", (*uip++ = *((uint *)addr)));
			} else if (size == 2) {
				printf(" %04x", (*usp++ = *((ushort *)addr)));
			} else {
				printf(" %02x", (*ucp++ = *((uchar *)addr)));
			}
			addr += size;
		}
		printf("    ");
		cp = linebuf;
		for (i=0; i<linebytes; i++) {
			if ((*cp < 0x20) || (*cp > 0x7e))
				printf(".");
			else
				printf("%c", *cp);
			cp++;
		}
		printf("\n");
		nbytes -= linebytes;
	} while (nbytes > 0);

	dp_last_addr = addr;
	dp_last_length = length;
	dp_last_size = size;

	return 0;
}


int do_mem_mm    (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	return mod_mem (cmdtp, 1, flag, argc, argv);
}


int do_mem_nm    (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	return mod_mem (cmdtp, 0, flag, argc, argv);
}


int do_mem_mw    (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	ulong	addr, size, writeval, count;

	if ((argc < 3) || (argc > 4)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	/* Check for size specification.
	*/
	size = 4;
	if (argv[0][2] == '.') {
		if (argv[0][3] == 'b') {
			size = 1;
		} else if (argv[0][3] == 'w') {
			size = 2;
		}
	}

	/* Address is specified since argc > 1
	*/
	addr = simple_strtoul(argv[1], NULL, 16);
	addr += base_address;

	/* Get the value to write.
	*/
	writeval = simple_strtoul(argv[2], NULL, 16);

	/* Count ? */
	if (argc == 4) {
		count = simple_strtoul(argv[3], NULL, 16);
	} else {
		count = 1;
	}

	while (count-- > 0) {
		if (size == 4)
			*((ulong  *)addr) = (ulong )writeval;
		else if (size == 2)
			*((ushort *)addr) = (ushort)writeval;
		else
			*((uchar *)addr) = (uchar)writeval;
		addr += size;
	}

	return 0;
}


int do_mem_cmp   (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	ulong	size, addr1, addr2, count, ngood;

	if (argc != 4) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	/* Check for size specification.
	*/
	size = 4;
	if (argv[0][3] == '.') {
		if (argv[0][4] == 'b') {
			size = 1;
		} else if (argv[0][4] == 'w') {
			size = 2;
		}
	}

	addr1 = simple_strtoul(argv[1], NULL, 16);
	addr1 += base_address;

	addr2 = simple_strtoul(argv[2], NULL, 16);
	addr2 += base_address;

	count = simple_strtoul(argv[3], NULL, 16);

	ngood = 0;

	while (count-- > 0) {
		if (size == 4) {
			ulong word1 = *(ulong *)addr1;
			ulong word2 = *(ulong *)addr2;
			if (word1 != word2) {
				printf("word at 0x%08lx (0x%08lx) "
					"!= word at 0x%08lx (0x%08lx)\n",
					addr1, word1, addr2, word2);
				break;
			}
		}
		else if (size == 2) {
			ushort hword1 = *(ushort *)addr1;
			ushort hword2 = *(ushort *)addr2;
			if (hword1 != hword2) {
				printf("halfword at 0x%08lx (0x%04x) "
					"!= halfword at 0x%08lx (0x%04x)\n",
					addr1, hword1, addr2, hword2);
				break;
			}
		}
		else {
			uchar byte1 = *(uchar *)addr1;
			uchar byte2 = *(uchar *)addr2;
			if (byte1 != byte2) {
				printf("byte at 0x%08lx (0x%02x) "
					"!= byte at 0x%08lx (0x%02x)\n",
					addr1, byte1, addr2, byte2);
				break;
			}
		}
		ngood++;
		addr1 += size;
		addr2 += size;
	}

	printf("Total of %ld %s%s were the same\n",
		ngood, size == 4 ? "word" : size == 2 ? "halfword" : "byte",
		ngood == 1 ? "" : "s");

	return 0;
}


int do_mem_cp    (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	ulong	addr, size, dest, count;

	if (argc != 4) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	/* Check for size specification.
	*/
	size = 4;
	if (argv[0][2] == '.') {
		if (argv[0][3] == 'b') {
			size = 1;
		} else if (argv[0][3] == 'w') {
			size = 2;
		}
	}

	addr = simple_strtoul(argv[1], NULL, 16);
	addr += base_address;

	dest = simple_strtoul(argv[2], NULL, 16);
	dest += base_address;

	count = simple_strtoul(argv[3], NULL, 16);

	/* check if we are copying to Flash */
	if (addr2info(dest) != NULL) {
		int rc;

		printf ("Copy to Flash... ");
		rc = flash_write ((uchar *)addr, dest, count*size);
	    	if (rc < 0) {
	      		flash_perror(rc);
			return 1;
		}
      		else
      			printf("done.\n");

                return 0;
	}

	while (count-- > 0) {
		if (size == 4)
			*((ulong  *)dest) = *((ulong  *)addr);
		else if (size == 2)
			*((ushort *)dest) = *((ushort *)addr);
		else
			*((uchar *)dest) = *((uchar *)addr);
		addr += size;
		dest += size;
	}

	return 0;
}


int do_mem_base  (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	if (argc > 1) {
		/* Set new base address.
		*/
		base_address = simple_strtoul(argv[1], NULL, 16);
	}
	/* Print the current base address.
	*/
	printf("Base Address: 0x%08lx\n", base_address);
	return 0;
}


int do_mem_loop  (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	ulong	addr, size, length, i, junk;
	volatile uint	*longp;
	volatile ushort *shortp;
	volatile uchar	*cp;

	if (argc < 3) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	/* Check for a size spefication.
	 * Defaults to long if no or incorrect specification.
	 */
	size = 4;
	if (argv[0][4] == '.') {
		if (argv[0][5] == 'b')
			size = 1;
		else if (argv[0][5] == 'w')
			size = 2;
	}

	/* Address is always specified.
	*/
	addr = simple_strtoul(argv[1], NULL, 16);

	/* Length is the number of objects, not number of bytes.
	*/
	length = simple_strtoul(argv[2], NULL, 16);

	/* We want to optimize the loops to run as fast as possible.
	 * If we have only one object, just run infinite loops.
	 */
	if (length == 1) {
		if (size == 4) {
			longp = (uint *)addr;
			for (;;)
				i = *longp;
		}
		if (size == 2) {
			shortp = (ushort *)addr;
			for (;;)
				i = *shortp;
		}
		cp = (uchar *)addr;
		for (;;)
			i = *cp;
	}

	if (size == 4) {
		for (;;) {
			longp = (uint *)addr;
			i = length;
			while (i-- > 0)
				junk = *longp++;
		}
	}
	if (size == 2) {
		for (;;) {
			shortp = (ushort *)addr;
			i = length;
			while (i-- > 0)
				junk = *shortp++;
		}
	}
	for (;;) {
		cp = (uchar *)addr;
		i = length;
		while (i-- > 0)
			junk = *cp++;
	}
	return 0;
}

/* Just a quickie to walk through some memory.
 */
uint baseval = 0;

int do_mem_mtest (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	int	*memaddr;
	int	memval;

	for (;;) {
		if (ctrlc()) {
			return 1;
		}

		memaddr = (uint *)CFG_MEMTEST_START;
		printf("\nWriting: %08x, ", baseval);
		memval = baseval;
		do {
			*memaddr++ = memval++;
		} while ((uint)memaddr < CFG_MEMTEST_END);

		memaddr = (uint *)CFG_MEMTEST_START;
		printf("Reading: %08x", baseval);
		memval = baseval;
		do {
			if (*memaddr != memval) {
				printf ("Mem error @ 0x%08x: "
					"found %08x, expected 0x%08x\n",
					(uint)memaddr, *memaddr, memval);
			}
			memaddr++;
			memval++;
		} while ((uint)memaddr < CFG_MEMTEST_END);

		baseval++;
	}
	return 0;
}




/* Modify memory.
 *
 * Syntax:
 *	mm{.b, .w, .l} {addr}
 *	nm{.b, .w, .l} {addr}
 */

static int
mod_mem(cmd_tbl_t *cmdtp, int incrflag, int flag, int argc, char *argv[])
{
	ulong	addr, size, i;
	int	nbytes;
	extern char console_buffer[];

	if (argc != 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

#ifdef CONFIG_BOOT_RETRY_TIME
	reset_cmd_timeout();	/* got a good command to get here */
#endif
	/* We use the last specified parameters, unless new ones are
	 * entered.
	 */
	addr = mm_last_addr;
	size = mm_last_size;

	if ((flag & CMD_FLAG_REPEAT) == 0) {
		/* New command specified.  Check for a size specification.
		 * Defaults to long if no or incorrect specification.
		 */
		size = 4;
		if (argv[0][2] == '.') {
			if (argv[0][3] == 'b') {
				size = 1;
			} else if (argv[0][3] == 'w') {
				size = 2;
			}
		}

		/* Address is specified since argc > 1
		*/
		addr = simple_strtoul(argv[1], NULL, 16);
		addr += base_address;
	}

	/* Print the address, followed by value.  Then accept input for
	 * the next value.  A non-converted value exits.
	 */
	do {
		printf("%08lx:", addr);
		if (size == 4)
			printf(" %08x", *((uint   *)addr));
		else if (size == 2)
			printf(" %04x", *((ushort *)addr));
		else
			printf(" %02x", *((uchar *)addr));

		nbytes = readline (" ? ");
		if (nbytes == 0) {
			/* <CR> pressed as only input, don't modify current
			 * location and move to next.
			 */
			if (incrflag)
				addr += size;
			nbytes = 1;
#ifdef CONFIG_BOOT_RETRY_TIME
			reset_cmd_timeout(); /* good enough to not time out */
#endif
		}
#ifdef CONFIG_BOOT_RETRY_TIME
		else if (nbytes == -2) {
			break;	/* timed out, exit the command	*/
		}
#endif
		else {
			char *endp;
			i = simple_strtoul(console_buffer, &endp, 16);
			nbytes = endp - console_buffer;
			if (nbytes) {
#ifdef CONFIG_BOOT_RETRY_TIME
				/* good enough to not time out
				 */
				reset_cmd_timeout();
#endif
				if (size == 4)
					*((uint   *)addr) = i;
				else if (size == 2)
					*((ushort *)addr) = i;
				else
					*((uchar *)addr) = i;
				if (incrflag)
					addr += size;
			}
		}
	} while (nbytes);

	mm_last_addr = addr;
	mm_last_size = size;

	return 0;
}

int do_mem_crc (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	ulong	addr, length;
	ulong	crc;

	if (argc < 3) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	addr = simple_strtoul(argv[1], NULL, 16);
	addr += base_address;

	length = simple_strtoul(argv[2], NULL, 16);

	crc = crc32 (0, (const uchar *)addr, length);

	printf ("CRC32 for %08lx ... %08lx ==> %08lx\n",
		addr, addr + length -1, crc);

	return 0;
}

#endif	/* CFG_CMD_MEMORY */
