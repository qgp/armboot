/*
 * (C) Copyright 2000
 * Paolo Scaffardi, AIRVENT SAM s.p.a - RIMINI(ITALY), arsenio@tin.it
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
#include <stdarg.h>
#include <malloc.h>
#include <config.h>

#if defined(CONFIG_ANSI_CONSOLE)
#	define CONSOLE_PUTC	ansi_console_putc
#elif defined (CONFIG_CFB_CONSOLE)
#	define CONSOLE_PUTC	video_putc
#else
#	define CONSOLE_PUTC	serial_putc
#endif

#ifdef CONFIG_KEYBOARD
extern int	kbd_tstc(void);
extern int	kbd_getc(void);
#	define CONSOLE_GETC	kbd_getc
#	define CONSOLE_TSTC	kbd_tstc
#else
#	define CONSOLE_GETC	serial_getc
#	define CONSOLE_TSTC	serial_tstc
#endif



void serial_puts (const char *s)
{
	while (*s) {
		CONSOLE_PUTC (*s++);
	}
}

void serial_printf(const char *fmt, ...)
{
	va_list	args;
	uint	i;
	char	printbuffer[CFG_PBSIZE];

	va_start(args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf(printbuffer, fmt, args);
	va_end(args);

	serial_puts(printbuffer);
}

int	getc(void)
{
    /* Send directly to the handler */
	return CONSOLE_GETC();
}

int	tstc(void)
{
    /* Send directly to the handler */
	return CONSOLE_TSTC();
}

void	putc(const char c)
{
	/* Send directly to the handler */
	CONSOLE_PUTC (c);
}

void	puts(const char *s)
{
	/* Send directly to the handler */
	serial_puts(s);
}

void printf(const char *fmt, ...)
{
	va_list	args;
	uint	i;
	char	printbuffer[CFG_PBSIZE];

	va_start(args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf(printbuffer, fmt, args);
	va_end(args);

	/* Print the string */
	puts (printbuffer);
}

/* test if ctrl-c was pressed */
static int ctrlc_disabled = 0;		/* see disable_ctrl() */
static int ctrlc_was_pressed = 0;
int ctrlc(void)
{
	if (!ctrlc_disabled) {
		if (tstc()) {
			switch (getc()) {
			case 0x03:	/* ^C - Control C */
				ctrlc_was_pressed = 1;
				return 1;
			default:
				break;
			}
		}
	}
	return 0;
}

/* pass 1 to disable ctrlc() checking, 0 to enable.
 * returns previous state
 */
int disable_ctrlc(int disable)
{
	int prev = ctrlc_disabled;	/* save previous state */
	ctrlc_disabled = disable;
	return prev;
}

int had_ctrlc(void)
{
	return ctrlc_was_pressed;
}

void clear_ctrlc(void)
{
	ctrlc_was_pressed = 0;
}

