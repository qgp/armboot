/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>

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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Support for persistent environment data
 */

#include "armboot.h"
#include "command.h"
#include "malloc.h"
#include "cmd_nvedit.h"

#if (CONFIG_COMMANDS & CFG_CMD_NET)
#include "net.h"
#endif

/*
 * Table with supported baudrates (defined in config_xyz.h)
 */
static const unsigned long baudrate_table[] = CFG_BAUDRATE_TABLE;
#define	N_BAUDRATES (sizeof(baudrate_table) / sizeof(baudrate_table[0]))

/*
 * Default settings to be used when no valid environment is found
 */
#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)

uchar default_environment[] = {
#ifdef	CONFIG_BOOTARGS
	"bootargs="	CONFIG_BOOTARGS			"\0"
#endif
#ifdef	CONFIG_BOOTCOMMAND
	"bootcmd="	CONFIG_BOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_RAMBOOTCOMMAND
	"ramboot="	CONFIG_RAMBOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_NFSBOOTCOMMAND
	"nfsboot="	CONFIG_NFSBOOTCOMMAND		"\0"
#endif
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	"bootdelay="	MK_STR(CONFIG_BOOTDELAY)	"\0"
#endif
#if defined(CONFIG_BAUDRATE) && (CONFIG_BAUDRATE >= 0)
	"baudrate="	MK_STR(CONFIG_BAUDRATE)		"\0"
#endif
#ifdef	CONFIG_LOADS_ECHO
	"loads_echo="	MK_STR(CONFIG_LOADS_ECHO)	"\0"
#endif
#ifdef	CONFIG_ETHADDR
	"ethaddr="	MK_STR(CONFIG_ETHADDR)		"\0"
#endif
#ifdef	CONFIG_ETH2ADDR
	"eth2addr="	MK_STR(CONFIG_ETH2ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH3ADDR
	"eth3addr="	MK_STR(CONFIG_ETH3ADDR)		"\0"
#endif
#ifdef	CONFIG_IPADDR
	"ipaddr="	MK_STR(CONFIG_IPADDR)		"\0"
#endif
#ifdef	CONFIG_SERVERIP
	"serverip="	MK_STR(CONFIG_SERVERIP)		"\0"
#endif
#ifdef	CFG_AUTOLOAD
	"autoload="	CFG_AUTOLOAD			"\0"
#endif
#ifdef	CONFIG_PREBOOT
	"preboot="	CONFIG_PREBOOT			"\0"
#endif
#ifdef	CONFIG_ROOTPATH
	"rootpath="	MK_STR(CONFIG_ROOTPATH)		"\0"
#endif
#ifdef	CONFIG_GATEWAYIP
	"gatewayip="	MK_STR(CONFIG_GATEWAYIP)	"\0"
#endif
#ifdef	CONFIG_NETMASK
	"netmask="	MK_STR(CONFIG_NETMASK)		"\0"
#endif
#ifdef	CONFIG_HOSTNAME
	"hostname="	MK_STR(CONFIG_HOSTNAME)		"\0"
#endif
#ifdef	CONFIG_BOOTFILE
	"bootfile="	MK_STR(CONFIG_BOOTFILE)		"\0"
#endif
#ifdef	CONFIG_LOADADDR
	"loadaddr="	MK_STR(CONFIG_LOADADDR)		"\0"
#endif
#ifdef  CONFIG_CLOCKS_IN_MHZ
	"clocks_in_mhz=1\0"
#endif
	"\0"
};

static int envmatch (bd_t *, uchar *, int);

/*
 * return one character from env
 */
static uchar get_env_char(bd_t *bd, int index)
{
    uchar c;
   
    /* use RAM copy, if possible */
    if (bd->bi_env)
    {
	if (index < sizeof(bd->bi_env_data))
	  c = bd->bi_env_data[index];
	else
	  panic("bad size for get_env_char!\n");
    }
    else
    {
	/* try a board specific lookup */
	if (board_env_getchar(bd, index, &c) < 0)
	{
	    if (index < sizeof(default_environment))
	      c = default_environment[index];
	    else 
	      panic("bad size for get_env_char!\n");
	}
    }
    return c;
}

/*
 * return address into environment
 */
static uchar *get_env_addr(bd_t *bd, int index)
{
    uchar *p = 0;
   
    /* use RAM copy, if possible */
    if (bd->bi_env)
    {
	if (index < sizeof(bd->bi_env_data))
	  p = &bd->bi_env_data[index];
	else
	  panic("bad size for get_env_char!\n");
    }
    else
    {
	/* try a board specific lookup */
	if ((p = board_env_getaddr(bd, index)) == 0)
	{
	    if (index < sizeof(default_environment))
	      p = &default_environment[index];
	    else 
	      panic("bad size for get_env_char!\n");
	}
    }
    return p;
}

/************************************************************************
 * Command interface: print one or all environment variables
 */

int do_printenv (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
    int i, j, k, nxt;
    
    if (argc == 1) {		/* Print all env variables	*/
	for (i=0; get_env_char(bd, i) != '\0'; i=nxt+1) {
	    for (nxt=i; get_env_char(bd, nxt) != '\0'; ++nxt)
	      ;
	    for (k=i; k<nxt; ++k)
	      putc(get_env_char(bd, k));
	    putc  ('\n');
	    
	    if (ctrlc()) {
		printf ("\n ** Abort\n");
		return 1;
	    }
	}
	
	printf("\nEnvironment size: %d/%d bytes\n", i, sizeof(bd->bi_env_data));
	
	return 0;
    }
    
    for (i=1; i<argc; ++i) {	/* print single env variables	*/
	char *name = argv[i];
	
	k = -1;
	
	for (j=0; get_env_char(bd, j) != '\0'; j=nxt+1) {
	    
	    for (nxt=j; get_env_char(bd, nxt) != '\0'; ++nxt)
	      ;
	    k = envmatch(bd, name, j);
	    if (k < 0) {
		continue;
	    }
	    puts (name);
	    putc ('=');
	    while (k < nxt)
	      putc(get_env_char(bd, k++));
	    putc ('\n');
	    break;
	}
	if (k < 0)
	{
	  printf ("## Error: \"%s\" not defined\n", name);
	  return 1;
	}
    }
    return 0;
}

/************************************************************************
 * Set a new environment variable,
 * or replace or delete an existing one.
 *
 * This function will ONLY work with a in-RAM copy of the environment
 */

int _do_setenv (bd_t *bd, int flag, int argc, char *argv[])
{
    int   i, len, oldval;
    uchar *env, *nxt = 0;
    uchar *name;
    
    /* need writable copy in RAM */
    if (!bd->bi_env_data)
      return 1;

    name = argv[1];
    
    /*
     * search if variable with this name already exists
     */
    oldval = -1;
    for (env = bd->bi_env_data; *env; env = nxt+1) {
	for (nxt = env; *nxt; ++nxt)
	  ;
	if ((oldval = envmatch(bd, name, (ulong)env - (ulong)bd->bi_env_data)) >= 0)
	  break;
    }
    
    /*
     * Delete any existing definition
     */
    if (oldval >= 0) {
#ifndef CONFIG_ENV_OVERWRITE
	
	/*
	 * Ethernet Address and serial# can be set only once
	 */
	if ( (strcmp (name, "serial#") == 0) ||
	    ((strcmp (name, "ethaddr") == 0)
# if defined(CONFIG_OVERWRITE_ETHADDR_ONCE) && defined(CONFIG_ETHADDR)
	     && (strcmp (get_env_addr(bd, oldval),MK_STR(CONFIG_ETHADDR)) != 0)
# endif	/* CONFIG_OVERWRITE_ETHADDR_ONCE && CONFIG_ETHADDR */
	     ) ) {
	    printf ("Can't overwrite \"%s\"\n", name);
	    return 1;
	}
#endif
	
	/*
	 * Switch to new baudrate if new baudrate is supported
	 */
	if (strcmp(argv[1],"baudrate") == 0) {
	    int baudrate = simple_strtoul(argv[2], NULL, 10);
	    int i;
	    for (i=0; i<N_BAUDRATES; ++i) {
		if (baudrate == baudrate_table[i])
		  break;
	    }
	    if (i == N_BAUDRATES) {
		printf ("## Baudrate %d bps not supported\n",
			baudrate);
		return 1;
	    }
	    printf ("## Switch baudrate to %d bps and press ENTER ...\n",
		    baudrate);
	    udelay(50000);
	    serial_setbrg (bd, baudrate);
	    udelay(50000);
	    for (;;) {
		if (getc() == '\r')
		  break;
	    }
	    bd->bi_baudrate = baudrate;
	}
	
	if (*++nxt == '\0') {
	    if ((ulong)env > (ulong)bd->bi_env_data) {
		env--;
	    } else {
		*env = '\0';
	    }
	} else {
	    for (;;) {
		*env = *nxt++;
		if ((*env == '\0') && (*nxt == '\0'))
		  break;
		++env;
	    }
	}
	*++env = '\0';
    }
    /* Delete only ? */
    if ((argc < 3) || argv[2] == NULL) {    
	/* Update CRC */
	bd->bi_env_crc = crc32(0, bd->bi_env_data, sizeof(bd->bi_env_data));
	return 0;
    }

    /*
     * Append new definition at the end
     */
    for (env = bd->bi_env_data; *env || *(env+1); ++env)
      ;
    if ((ulong)env > (ulong)bd->bi_env_data)
      ++env;
    /*
     * Overflow when:
     * "name" + "=" + "val" +"\0\0"  > 
     *      sizeof(bd->bi_env_data) - (env-bd->bi_env_data)
     */
    len = strlen(name) + 2;
    /* add '=' for first arg, ' ' for all others */
    for (i=2; i<argc; ++i) {
	len += strlen(argv[i]) + 1;
    }
    if (len > sizeof(bd->bi_env_data)) {
	printf ("## Error: environment overflow, \"%s\" deleted\n", name);
	return 1;
    }
    while ((*env = *name++) != '\0')
      env++;
    for (i=2; i<argc; ++i) {
	char *val = argv[i];
	
	*env = (i==2) ? '=' : ' ';
	while ((*++env = *val++) != '\0')
	  ;
    }
    
    /* end is marked with double '\0' */
    *++env = '\0';
    
    /* Update CRC */
    bd->bi_env_crc = crc32(0, bd->bi_env_data, sizeof(bd->bi_env_data));

    /*
     * Some variables should be updated when the corresponding
     * entry in the enviornment is changed
     */
    
    if (strcmp(argv[1],"ethaddr") == 0) {
	char *s = argv[2];	/* always use only one arg */
	char *e;
	for (i=0; i<6; ++i) {
	    bd->bi_enetaddr[i] = s ? simple_strtoul(s, &e, 16) : 0;
	    if (s) s = (*e) ? e+1 : e;
	}
	return 0;
    }

    if (strcmp(argv[1],"ipaddr") == 0) {
	char *s = argv[2];	/* always use only one arg */
	bd->bi_ip_addr = string_to_ip(s);
	return 0;
    }

    if (strcmp(argv[1],"loadaddr") == 0) {
	load_addr = simple_strtoul(argv[2], NULL, 16);
	return 0;
    }

#if (CONFIG_COMMANDS & CFG_CMD_NET)
    if (strcmp(argv[1],"bootfile") == 0) {
	copy_filename (BootFile, argv[2], sizeof(BootFile));
	return 0;
    }
#endif	/* CFG_CMD_NET */

#ifdef CONFIG_KEYBOARD
    if (strcmp(argv[1],"keymap") == 0) {
	kbd_mapping (argv[2]);
	return 0;
    }
#endif	/* CONFIG_KEYBOARD */

    return 0;
}

void setenv (bd_t * bd, char *varname, char *varvalue)
{
    char *argv[4] = { "setenv", varname, varvalue, NULL };
    _do_setenv (bd, 0, 3, argv);
}

int do_setenv (cmd_tbl_t * cmdtp, bd_t * bd, int flag, int argc,
		char *argv[])
{
    if (argc < 2) {
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }

    return _do_setenv (bd, flag, argc, argv);
}

/************************************************************************
 * Prompt for environment variable
 */

#if (CONFIG_COMMANDS & CFG_CMD_ASKENV)
int do_askenv (cmd_tbl_t * cmdtp, bd_t * bd, int flag, int argc,
		char *argv[])
{
    extern char console_buffer[CFG_CBSIZE];
    char message[CFG_CBSIZE];
    int size = CFG_CBSIZE - 1;
    int len;
    char *local_args[4];
    
    local_args[0] = argv[0];
    local_args[1] = argv[1];
    local_args[2] = NULL;
    local_args[3] = NULL;
    
    if (argc < 2) {
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }
    /* Check the syntax */
    switch (argc) {
    case 1:
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
	
    case 2:		/* askenv envname */
	sprintf (message, "Please enter '%s':", argv[1]);
	break;
	
    case 3:		/* askenv envname size */
	sprintf (message, "Please enter '%s':", argv[1]);
	size = simple_strtoul (argv[2], NULL, 10);
	break;
	
    default:	/* askenv envname message1 ... messagen size */
	{
	    int i;
	    int pos = 0;
	    
	    for (i = 2; i < argc - 1; i++) {
		if (pos) {
		    message[pos++] = ' ';
		}
		strcpy (message+pos, argv[i]);
		pos += strlen(argv[i]);
	    }
	    message[pos] = '\0';
	    size = simple_strtoul (argv[argc - 1], NULL, 10);
	}
    }
    
    if (size >= CFG_CBSIZE)
      size = CFG_CBSIZE - 1;

    if (size <= 0)
      return 1;

    /* prompt for input */
    len = readline (message);

    if (size < len)
      console_buffer[size] = '\0';

    len = 2;
    if (console_buffer[0] != '\0') {
	local_args[2] = console_buffer;
	len = 3;
    }
    
    // Continue calling setenv code
    return _do_setenv (bd, flag, len, local_args);
}
#endif	/* CFG_CMD_ASKENV */

/************************************************************************
 * Look up variable from environment,
 * return address of storage for that variable,
 * or NULL if not found
 */

char *getenv (bd_t * bd, uchar *name)
{
    int i, nxt;
    
    for (i=0; get_env_char(bd, i) != '\0'; i=nxt+1) {
	int val;
	
	for (nxt=i; get_env_char(bd, nxt) != '\0'; ++nxt) {
	    if (nxt >= sizeof(bd->bi_env_data)) {
		return (NULL);
	    }
	}
	if ((val=envmatch(bd, name, i)) < 0)
	  continue;
	return (get_env_addr(bd, val));
    }
    
    return (NULL);
}


/************************************************************************
 * Match a name / name=value pair
 *
 * s1 is either a simple 'name', or a 'name=value' pair.
 * i2 is the environment index for a 'name2=value2' pair.
 * If the names match, return the index for the value2, else NULL.
 */

static int envmatch (bd_t *bd, uchar *s1, int i2)
{

	while (*s1 == get_env_char(bd, i2++))
		if (*s1++ == '=')
			return(i2);
	if (*s1 == '\0' && get_env_char(bd, i2-1) == '=')
		return(i2);
	return(-1);
}


#if (CONFIG_COMMANDS & CFG_CMD_ENV)

int do_saveenv  (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
    return (board_env_save(bd, bd->bi_env, sizeof(env_t))) ? 1 : 0;
}

#endif	/* CFG_CMD_ENV */


void env_init(bd_t *bd)
{
    bd->bi_env = 0;
}

void env_relocate(bd_t *bd)
{
    char *s, *e;
    int reg;
   
    bd->bi_env = malloc(sizeof(env_t));

    if (board_env_copy(bd, bd->bi_env, sizeof(env_t)) < 0)
    {
	printf("*** Using default environment\n");
	memcpy(bd->bi_env_data, default_environment, 
	       sizeof(default_environment));
	bd->bi_env_crc = crc32(0, bd->bi_env_data, sizeof(bd->bi_env_data));
    }
    
    /* now initialise some variables */
    
    /* MAC address */
    s = getenv(bd, "ethaddr");
    for (reg=0; reg<6; reg++) 
    {
	bd->bi_enetaddr[reg] = s ? simple_strtoul(s, &e, 16) : 0;
	if (s)
	  s = (*e) ? e+1 : e;
    }

    /* IP address */
    bd->bi_ip_addr = 0;
    s = getenv(bd, "ipaddr");
    for (reg=0; reg<4; ++reg) {    
        ulong val = s ? simple_strtoul(s, &e, 10) : 0;
        bd->bi_ip_addr <<= 8;
        bd->bi_ip_addr  |= (val & 0xFF);
        if (s)
	  s = (*e) ? e+1 : e;
    }
    
    if ((s = getenv(bd, "loadaddr")) != NULL) {    
        load_addr = simple_strtoul(s, NULL, 16);
    }
    
#if (CONFIG_COMMANDS & CFG_CMD_NET)
    if ((s = getenv(bd, "bootfile")) != NULL) {    
        copy_filename (BootFile, s, sizeof(BootFile));
    }
#endif  /* CFG_CMD_NET */
}
