/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * 
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
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

#ifndef _ARMBOOT_H_
#define _ARMBOOT_H_	1

#undef	_LINUX_CONFIG_H
#define _LINUX_CONFIG_H 1	/* avoid reading Linux autoconf.h file	*/

#include "config.h"
#include "types.h"
#include <stdarg.h>

#include <flash.h>
#include <image.h>

#ifdef	DEBUG
#define debug(fmt,args...)	printf (fmt ,##args)
#else
#define debug(fmt,args...)
#endif

/************************************************************************
 *
 * The environment storages is simply a list of '\0'-terminated
 * "name=value" strings, the end of the list marked by a double '\0'.
 * New entries are always added at the end. Deleting an entry shifts
 * the remaining entries to the front. Replacing an entry is a
 * combination of deleting the old and adding the new value.
 *
 * The environment is preceeded by a 32 bit CRC over the data part.
 *
 ************************************************************************
 */

typedef	struct environment_s {
	ulong	crc;			/* CRC32 over data bytes	*/
	uchar	data[CFG_ENV_SIZE - sizeof(ulong)];
} env_t;

typedef struct bd_info {
    int			bi_baudrate;	/* serial console baudrate */
    unsigned long	bi_ip_addr;	/* IP Address */
    unsigned char	bi_enetaddr[6]; /* Ethernet adress */
    env_t	       *bi_env;
    ulong	        bi_arch_number;	/* unique id for this board */
    ulong	        bi_boot_params;	/* where this board expects params */
    struct				/* RAM configuration */
    { 
	ulong start;
	ulong size;
    } 			bi_dram[CONFIG_NR_DRAM_BANKS];
    struct bd_info_ext	bi_ext;		/* board specific extension */
} bd_t;

#define bi_env_data bi_env->data
#define bi_env_crc  bi_env->crc

/* arm/crc32.c */
ulong	crc32		(ulong, const unsigned char *, uint);
ulong	crc32_no_comp	(ulong crc, const unsigned char *, uint);

/*
 * Don't define some stuff private to armboot target code
 */
#ifndef HOST_TOOLS

/* board/.../env.c */
int 	board_env_getchar(bd_t * bd, int index, uchar *c);
int 	board_env_save	(bd_t * bd, env_t *data, int size);
int 	board_env_copy 	(bd_t * bd, env_t *data, int size);
uchar *	board_env_getaddr(bd_t * bd, int index);

/*
 * Function Prototypes
 */
void	hang	      	(void);
void	start_armboot	(void);
void	main_loop	(bd_t *);
int	run_command	(const char *cmd, bd_t *, int flag);
int	readline	(const char *const prompt);
void	reset_cmd_timeout(void);

/* board/.../... */
int board_init(bd_t *);
int dram_init(bd_t *bd);

/* arm/display_options.c */
void display_banner(bd_t *bd);
void display_dram_config(bd_t *bd);
void display_flash_config(bd_t *bd, ulong size);

/* common/cmd_nvedit.c */
void	env_init	(bd_t *bd);
void	env_relocate	(bd_t *bd);
char	*getenv		(bd_t *bd, uchar *);
void	setenv		(bd_t *bd, char *, char *);


/* $(CPU)/serial.c */
void	serial_init	(bd_t *);
void	serial_setbrg	(bd_t *, int);
void	serial_putc	(const char);
void	serial_puts	(const char *);
void	serial_addr	(unsigned int);
int	serial_getc	(void);
int	serial_tstc	(void);

/* arm/string.c */
char *	strcpy		(char * dest,const char *src);
char *	strncpy		(char * dest,const char *src, size_t count);
size_t	strlen		(const char *);
size_t	strnlen		(const char * s, size_t count);
int	strncmp		(const char * cs, const char * ct, size_t count);
int	strcmp		(const char * cs, const char * ct);
void *	memcpy		(void * dest, const void *src, size_t count);
int	memcmp		(const void * dest, const void *src, size_t count);
void *	memset		(void * s, char c, size_t count);
void *	memmove		(void * dest, const void *src, size_t count);
char *	strchr		(const char * s, int c);

/* arm/vsprintf.c */
ulong	simple_strtoul	(const char *cp,char **endp,unsigned int base);
long	simple_strtol	(const char *cp,char **endp,unsigned int base);
void	panic		(const char *fmt, ...);
int	sprintf		(char * buf, const char *fmt, ...);
int 	vsprintf	(char *buf, const char *fmt, va_list args);

void    udelay		(unsigned long);

int	ctrlc		(void);
int	had_ctrlc 	(void);
void	clear_ctrlc	(void);
int	disable_ctrlc	(int);

int	console_assign	(int file, char *devname);

/* */
int	icache_status (void);
void	icache_enable (void);
void	icache_disable(void);
int	dcache_status (void);
void	dcache_enable (void);
void	dcache_disable(void);

/* common/cmd_bootm.c */
void print_image_hdr (image_header_t *hdr);

/* cpu/.../interrupt.c */
void	enable_interrupts	(void);
int	disable_interrupts	(void);
void	interrupt_init		(bd_t *bd);
void	reset_timer		(void);
ulong	get_timer		(ulong base);
void	set_timer		(ulong t);
void	reset_timer_masked	(void);
ulong	get_timer_masked	(void);
void	udelay_masked		(unsigned long usec);

/* cpu/.../cpu.c */
void cpu_init(bd_t *bd);
void cleanup_before_linux(bd_t *bd);

extern ulong load_addr;

/* for the following variables, see start.S */
extern ulong _armboot_start; /* code start */
extern ulong _armboot_end;   /* code end */ 
extern ulong IRQ_STACK_START; /* top of IRQ stack */
extern ulong FIQ_STACK_START; /* top of FIQ stack */
extern ulong _armboot_real_end; /* first usable RAM address */

/*
 * STDIO based functions (can always be used)
 */

/* serial stuff */
void	serial_printf (const char *fmt, ...);

/* stdin */
int	getc(void);
int	tstc(void);

/* stdout */
void	putc(const char c);
void	puts(const char *s);
void	printf(const char *fmt, ...);

/* stderr */
#define eputc(c)		fputc(stderr, c)
#define eputs(s)		fputs(stderr, s)
#define eprintf(fmt,args...)	fprintf(stderr,fmt ,##args)

/*
 * FILE based functions (can only be used AFTER relocation!)
 */

#define stdin		0
#define stdout		1
#define stderr		2
#define MAX_FILES	3

void	fprintf(int file, const char *fmt, ...);
void	fputs(int file, const char *s);
void	fputc(int file, const char c);
int	ftstc(int file);
int	fgetc(int file);

/* Byte swapping stuff */
#define SWAP16(x)	((((x) & 0xff) << 8) | ((x) >> 8))
#define SWAP32(x)       ( \
	                (((x) >> 24) & 0x000000ff) | \
        		(((x) >>  8) & 0x0000ff00) | \
		        (((x) <<  8) & 0x00ff0000) | \
		        (((x) << 24) & 0xff000000) )

#define htons(x)	SWAP16(x)
#define ntohs(x)	SWAP16(x)
#define htonl(x)	SWAP32(x)
#define ntohl(x)	SWAP32(x)

#endif /* HOST_TOOLS */

#endif	/* _ARMBOOT_H_ */
