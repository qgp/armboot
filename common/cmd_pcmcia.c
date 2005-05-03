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
 *
 ********************************************************************
 *
 * Lots of code copied from:
 *
 * m8xx_pcmcia.c - Linux PCMCIA socket driver for the mpc8xx series.
 * (C) 1999-2000 Magnus Damm <damm@bitsmart.com>
 *
 * "The ExCA standard specifies that socket controllers should provide
 * two IO and five memory windows per socket, which can be independently
 * configured and positioned in the host address space and mapped to
 * arbitrary segments of card address space. " - David A Hinds. 1999
 *
 * This controller does _not_ meet the ExCA standard.
 *
 * m8xx pcmcia controller brief info:
 * + 8 windows (attrib, mem, i/o)
 * + up to two slots (SLOT_A and SLOT_B)
 * + inputpins, outputpins, event and mask registers.
 * - no offset register. sigh.
 *
 * Because of the lacking offset register we must map the whole card.
 * We assign each memory window PCMCIA_MEM_WIN_SIZE address space.
 * Make sure there is (PCMCIA_MEM_WIN_SIZE * PCMCIA_MEM_WIN_NO
 * * PCMCIA_SOCKETS_NO) bytes at PCMCIA_MEM_WIN_BASE.
 * The i/o windows are dynamically allocated at PCMCIA_IO_WIN_BASE.
 * They are maximum 64KByte each...
 */

/*
 * PCMCIA support
 */
#include <armboot.h>
#include <command.h>
#include <config.h>
#include <pcmcia.h>
#include <cmd_pcmcia.h>

#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA) || \
    ((CONFIG_COMMANDS & CFG_CMD_IDE) && defined(CONFIG_IDE_PCCARD))

int pcmcia_on (void);

#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static void pcmcia_off (void);
static int  hardware_disable(int slot);
#endif
static int  hardware_enable (int slot);
static int  voltage_set(int slot, int vcc, int vpp);
#ifdef CONFIG_IDE_PCCARD
static void print_funcid (int func);
static void print_fixed  (volatile uchar *p);
static int  identify     (volatile uchar *p);
static int  check_ide_device (void);
#endif	/* CONFIG_IDE_PCCARD */

#define u_int uint
const char *indent = "\t   ";

/* ------------------------------------------------------------------------- */

#undef	DEBUG
//#define	DEBUG

#ifdef	DEBUG
#define	PCMCIA_DEBUG(fmt,args...)	do { printf (fmt ,##args); } while (0)
#else
#define PCMCIA_DEBUG(fmt,args...)
#endif

/* ------------------------------------------------------------------------- */

#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)

int do_pinit (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	if (argc != 2) {
		printf ("Usage: pinit {on | off}\n");
		return 1;
	}
	if (strcmp(argv[1],"on") == 0) {
		pcmcia_on ();
	} else if (strcmp(argv[1],"off") == 0) {
		pcmcia_off ();
	} else {
		printf ("Usage: pinit {on | off}\n");
		return 1;
	}

	return 0;
}
#endif	/* CFG_CMD_PCMCIA */

/* ------------------------------------------------------------------------- */

int pcmcia_on (void)
{
	PCMCIA_DEBUG ("Enable PCMCIA " PCMCIA_SLOT_MSG "\n");

	/* turn off voltage */
	if (voltage_set(_slot_, 0, 0))
		return (1);

	/* Enable external hardware */
	if (hardware_enable(_slot_))
		return (1);

#ifdef CONFIG_IDE_PCCARD
	if (check_ide_device())
		return (1);
#endif
	return (0);
}

/* ------------------------------------------------------------------------- */

#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)

static void pcmcia_off (void)
{
	printf ("Disable PCMCIA " PCMCIA_SLOT_MSG "\n");

	/* turn off voltage */
	voltage_set(_slot_, 0, 0);

	/* disable external hardware */
	printf ("Shutdown and Poweroff " PCMCIA_SLOT_MSG "\n");
	hardware_disable(_slot_);
}

#endif	/* CFG_CMD_PCMCIA */

/* ------------------------------------------------------------------------- */

#ifdef CONFIG_IDE_PCCARD

#define	MAX_TUPEL_SZ	512
#define MAX_FEATURES	4

static int check_ide_device (void)
{
	volatile uchar *ident = NULL;
	volatile uchar *feature_p[MAX_FEATURES];
	volatile uchar *p, *start;
	int n_features = 0;
	uchar func_id = ~0;
	uchar code, len;
	ushort config_base = 0;
	int found = 0;
	int i;

	PCMCIA_DEBUG ("PCMCIA MEM: %08X\n", CFG_PCMCIA_MEM_ADDR);

	start = p = (volatile uchar *) CFG_PCMCIA_MEM_ADDR;

	while ((p - start) < MAX_TUPEL_SZ) {

		code = *p; p += 2;

		if (code == 0xFF) { /* End of chain */
			break;
		}

		len = *p; p += 2;
#ifdef	DEBUG
		{ volatile uchar *q = p;
			printf ("\nTuple code %02x  length %d\n\tData:",
				code, len);

			for (i = 0; i < len; ++i) {
				printf (" %02x", *q);
				q+= 2;
			}
		}
#endif	/* DEBUG */
		switch (code) {
		case CISTPL_VERS_1:
			ident = p + 4;
			break;
		case CISTPL_FUNCID:
			func_id = *p;
			break;
		case CISTPL_FUNCE:
			if (n_features < MAX_FEATURES)
				feature_p[n_features++] = p;
			break;
		case CISTPL_CONFIG:
			config_base = (*(p+6) << 8) + (*(p+4));
			PCMCIA_DEBUG ("\n## Config_base = %04x ###\n", config_base);
		default:
			break;
		}
		p += 2 * len;
	}

	found = identify (ident);

	if (func_id != ((uchar)~0)) {
		print_funcid (func_id);

		if (func_id == CISTPL_FUNCID_FIXED)
			found = 1;
		else
			return (1);	/* no disk drive */
	}

	for (i=0; i<n_features; ++i) {
		print_fixed (feature_p[i]);
	}

	if (!found) {
		printf ("unknown card type\n");
		return (1);
	}


	/* set I/O area in config reg -> only valid for ARGOSY D5!!! */
	//*((uchar *)(CFG_PCMCIA_MEM_ADDR + config_base)) = 1;

	return (0);
}
#endif	/* CONFIG_IDE_PCCARD */

/* ------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/* board specific stuff:							*/
/* voltage_set(), hardware_enable() and hardware_disable()			*/
/* really, this stuff should be somewhere else!					*/
/* ---------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/* The Shannon-Board								*/
/* ---------------------------------------------------------------------------- */

#if defined(CONFIG_SHANNON)

/*
 * It's a SA1100, we actually do nothing :-)
 */

#define PCMCIA_BOARD_MSG "SA1100"

#undef PCMCIA_BMT_LIMIT
#define PCMCIA_BMT_LIMIT (6*8)

static int voltage_set(int slot, int vcc, int vpp)
{
	return 0;
}

#define socket_get(_slot_) PCMCIA_SOCKET_KEY_5V
static int hardware_enable (int slot)
{
	return 0;	/* No hardware to enable */
}
#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static int hardware_disable(int slot)
{
	return 0;	/* No hardware to disable */
}
#endif	/* CFG_CMD_PCMCIA */
#endif	/* CONFIG_RPXCLASSIC */


/* ---------------------------------------------------------------------------- */
/* End of Board Specific Stuff							*/
/* ---------------------------------------------------------------------------- */

#ifdef CONFIG_IDE_PCCARD
static void print_funcid (int func)
{
	puts (indent);
	switch (func) {
	case CISTPL_FUNCID_MULTI:
		puts (" Multi-Function");
		break;
	case CISTPL_FUNCID_MEMORY:
		puts (" Memory");
		break;
	case CISTPL_FUNCID_SERIAL:
		puts (" Serial Port");
		break;
	case CISTPL_FUNCID_PARALLEL:
		puts (" Parallel Port");
		break;
	case CISTPL_FUNCID_FIXED:
		puts (" Fixed Disk");
		break;
	case CISTPL_FUNCID_VIDEO:
		puts (" Video Adapter");
		break;
	case CISTPL_FUNCID_NETWORK:
		puts (" Network Adapter");
		break;
	case CISTPL_FUNCID_AIMS:
		puts (" AIMS Card");
		break;
	case CISTPL_FUNCID_SCSI:
		puts (" SCSI Adapter");
		break;
	default:
		puts (" Unknown");
		break;
	}
	puts (" Card\n");
}
#endif	/* CONFIG_IDE_PCCARD */

/* ------------------------------------------------------------------------- */

#ifdef CONFIG_IDE_PCCARD
static void print_fixed (volatile uchar *p)
{
	if (p == NULL)
		return;

	puts(indent);

	switch (*p) {
	case CISTPL_FUNCE_IDE_IFACE:
	    {   uchar iface = *(p+2);

		puts ((iface == CISTPL_IDE_INTERFACE) ? " IDE" : " unknown");
		puts (" interface ");
		break;
	    }
	case CISTPL_FUNCE_IDE_MASTER:
	case CISTPL_FUNCE_IDE_SLAVE:
	    {   uchar f1 = *(p+2);
		uchar f2 = *(p+4);

		puts ((f1 & CISTPL_IDE_SILICON) ? " [silicon]" : " [rotating]");

		if (f1 & CISTPL_IDE_UNIQUE)
			puts (" [unique]");

		puts ((f1 & CISTPL_IDE_DUAL) ? " [dual]" : " [single]");

		if (f2 & CISTPL_IDE_HAS_SLEEP)
			puts (" [sleep]");

		if (f2 & CISTPL_IDE_HAS_STANDBY)
			puts (" [standby]");

		if (f2 & CISTPL_IDE_HAS_IDLE)
			puts (" [idle]");

		if (f2 & CISTPL_IDE_LOW_POWER)
			puts (" [low power]");

		if (f2 & CISTPL_IDE_REG_INHIBIT)
			puts (" [reg inhibit]");

		if (f2 & CISTPL_IDE_HAS_INDEX)
			puts (" [index]");

		if (f2 & CISTPL_IDE_IOIS16)
			puts (" [IOis16]");

		break;
	    }
	}
	putc ('\n');
}
#endif	/* CONFIG_IDE_PCCARD */

/* ------------------------------------------------------------------------- */

#ifdef CONFIG_IDE_PCCARD

#define MAX_IDENT_CHARS		64
#define	MAX_IDENT_FIELDS	4

static uchar *known_cards[] = {
	"ARGOSY PnPIDE D5",
	NULL
};

static int identify  (volatile uchar *p)
{
	uchar id_str[MAX_IDENT_CHARS];
	uchar data;
	uchar *t;
	uchar **card;
	int i, done;

	if (p == NULL)
		return (0);	/* Don't know */

	t = id_str;
	done =0;

	for (i=0; i<=4 && !done; ++i, p+=2) {
		while ((data = *p) != '\0') {
			if (data == 0xFF) {
				done = 1;
				break;
			}
			*t++ = data;
			if (t == &id_str[MAX_IDENT_CHARS-1]) {
				done = 1;
				break;
			}
			p += 2;
		}
		if (!done)
			*t++ = ' ';
	}
	*t = '\0';
	while (--t > id_str) {
		if (*t == ' ')
			*t = '\0';
		else
			break;
	}
	puts (id_str);
	putc ('\n');

	for (card=known_cards; *card; ++card) {
		PCMCIA_DEBUG ("## Compare against \"%s\"\n", *card);
		if (strcmp(*card, id_str) == 0) {	/* found! */
			PCMCIA_DEBUG ("## CARD FOUND ##\n");
			return (1);
		}
	}

	return (0);	/* don't know */
}
#endif	/* CONFIG_IDE_PCCARD */

/* ------------------------------------------------------------------------- */

#endif /* CFG_CMD_PCMCIA || (CFG_CMD_IDE && CONFIG_IDE_PCCARD) */
