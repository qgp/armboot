/*
 * (C) Copyright 2000
 * Subodh Nijsure, SkyStream Networks, snijsure@skystream.com
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
#include <command.h>
#include <cmd_boot.h>
#if defined(CONFIG_8xx)
#include <mpc8xx.h>
#endif

#if defined(CONFIG_8xx) && (CONFIG_COMMANDS & CFG_CMD_REGINFO)

void do_reginfo(cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	volatile immap_t     *immap  = (immap_t *)CFG_IMMR;
	volatile memctl8xx_t *memctl = &immap->im_memctl;
	volatile sysconf8xx_t *sysconf = &immap->im_siu_conf;
	volatile sit8xx_t *timers = &immap->im_sit;

	/* Hopefully more PowerPC  knowledgable people will add code to display
	 * other useful registers
	 */

	printf("\nSystem Configuration registers\n");

	printf("\tIMMR\t0x%08X\n", get_immr(0));

	printf("\tSIUMCR\t0x%08X", sysconf->sc_siumcr);
	printf("\tSYPCR\t0x%08X\n",sysconf->sc_sypcr);

	printf("\tSWT\t0x%08X",    sysconf->sc_swt);
	printf("\tSWSR\t0x%04X\n", sysconf->sc_swsr);

	printf("\tSIPEND\t0x%08X\tSIMASK\t0x%08X\n",
		sysconf->sc_sipend, sysconf->sc_simask);
	printf("\tSIEL\t0x%08X\tSIVEC\t0x%08X\n",
		sysconf->sc_siel, sysconf->sc_sivec);
	printf("\tTESR\t0x%08X\tSDCR\t0x%08X\n",
		sysconf->sc_tesr, sysconf->sc_sdcr);

	printf("Memory Controller Registers\n");

	printf("\tBR0\t0x%08X\tOR0\t0x%08X \n", memctl->memc_br0, memctl->memc_or0);
	printf("\tBR1\t0x%08X\tOR1\t0x%08X \n", memctl->memc_br1, memctl->memc_or1);
	printf("\tBR2\t0x%08X\tOR2\t0x%08X \n", memctl->memc_br2, memctl->memc_or2);
	printf("\tBR3\t0x%08X\tOR3\t0x%08X \n", memctl->memc_br3, memctl->memc_or3);
	printf("\tBR4\t0x%08X\tOR4\t0x%08X \n", memctl->memc_br4, memctl->memc_or4);
	printf("\tBR5\t0x%08X\tOR5\t0x%08X \n", memctl->memc_br5, memctl->memc_or5);
	printf("\tBR6\t0x%08X\tOR6\t0x%08X \n", memctl->memc_br6, memctl->memc_or6);
	printf("\tBR7\t0x%08X\tOR7\t0x%08X \n", memctl->memc_br7, memctl->memc_or7);
	printf("\n");

	printf("\tmamr\t0x%08X\tmbmr\t0x%08X \n",
		memctl->memc_mamr, memctl->memc_mbmr );
	printf("\tmstat\t0x%08X\tmptpr\t0x%08X \n",
		memctl->memc_mstat, memctl->memc_mptpr );
	printf("\tmdr\t0x%08X \n", memctl->memc_mdr);

	printf("\nSystem Integration Timers\n");
	printf("\tTBSCR\t0x%08X\tRTCSC\t0x%08X \n",
		timers->sit_tbscr, timers->sit_rtcsc);
	printf("\tPISCR\t0x%08X \n", timers->sit_piscr);

	/*
	 * May be some CPM info here?
	 */

}

#endif	/* CONFIG_8xx && CFG_CMD_REGINFO */
