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
 * Boot support
 */
#include <armboot.h>
#include <command.h>
#include <cmd_net.h>
#include <net.h>

#if (CONFIG_COMMANDS & CFG_CMD_NET)

# if (CONFIG_COMMANDS & CFG_CMD_AUTOSCRIPT)
# include <cmd_autoscript.h>
# endif

extern int do_bootm (cmd_tbl_t *, bd_t *, int, int, char *[]);

static int netboot_common (int, cmd_tbl_t *, bd_t *, int , char *[]);

int do_bootp (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	return netboot_common (BOOTP, cmdtp, bd, argc, argv);
}

int do_tftpb (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	return netboot_common (TFTP, cmdtp, bd, argc, argv);
}

int do_rarpb (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	return netboot_common (RARP, cmdtp, bd, argc, argv);
}

#if (CONFIG_COMMANDS & CFG_CMD_DHCP)
int do_dhcp (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	return netboot_common(DHCP, cmdtp, bd, argc, argv);
}
#endif	/* CFG_CMD_DHCP */

static void netboot_update_env(void)
{
    char tmp[12] ;

    if (NetOurGatewayIP) {
	ip_to_string (NetOurGatewayIP, tmp);
	setenv(Net_bd, "gatewayip", tmp);
    }

    if (NetOurSubnetMask) {
	ip_to_string (NetOurSubnetMask, tmp);
	setenv(Net_bd, "netmask", tmp);
    }

    if (NetOurHostName[0])
	setenv(Net_bd, "hostname", NetOurHostName);

    if (NetOurRootPath[0])
	setenv(Net_bd, "rootpath", NetOurRootPath);

    if (NetOurIP) {
	ip_to_string (NetOurIP, tmp);
	setenv(Net_bd, "ipaddr", tmp);
    }

    if (NetServerIP) {
	ip_to_string (NetServerIP, tmp);
	setenv(Net_bd, "serverip", tmp);
    }

    if (NetOurDNSIP) {
	ip_to_string (NetOurDNSIP, tmp);
	setenv(Net_bd, "dnsip", tmp);
    }
}

static int
netboot_common (int proto, cmd_tbl_t *cmdtp, bd_t *bd, int argc, char *argv[])
{
	char *s;
	int rc = 0;
   
	switch (argc) {
	case 1:
		break;

	case 2:	/* only one arg - accept two forms:
		 * just load address, or just boot file name.
		 * The latter form must be written "filename" here.
		 */
		if (argv[1][0] == '"') {	/* just boot filename */
			copy_filename (BootFile, argv[1], sizeof(BootFile));
		} else {			/* load address	*/
			load_addr = simple_strtoul(argv[1], NULL, 16);
		}
		break;

	case 3:	load_addr = simple_strtoul(argv[1], NULL, 16);
		copy_filename (BootFile, argv[2], sizeof(BootFile));

		break;

	default: printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (NetLoop(bd, proto) == 0)
		return 0;

	/* NetLoop ok, update environment */
	netboot_update_env();

	/* Loading ok, check if we should attempt an auto-start */
	if (((s = getenv(Net_bd, "autostart")) != NULL) && (strcmp(s,"yes") == 0)) {
		char *local_args[2];
		local_args[0] = argv[0];
		local_args[1] = NULL;

		printf ("Automatic boot of image at addr 0x%08lX ...\n",
			load_addr);

		rc = do_bootm (cmdtp, bd, 0, 1, local_args);
	}

#ifdef CONFIG_AUTOSCRIPT
	if (rc == 0 && 
	    ((s = getenv(Net_bd, "autoscript")) != NULL) && (strcmp(s,"yes") == 0)) {
		printf("Running autoscript at addr 0x%08lX ...\n", load_addr);
		rc = autoscript (bd, load_addr);
	}
#endif

	return rc;
}

#endif	/* CFG_CMD_NET */
