#include <config.h>

#if defined(CFG_ENV_IS_IN_FLASH)
# ifndef  CFG_ENV_ADDR
#  define CFG_ENV_ADDR	(CFG_FLASH_BASE + CFG_ENV_OFFSET)
# endif
# ifndef  CFG_ENV_OFFSET
#  define CFG_ENV_OFFSET (CFG_ENV_ADDR - CFG_FLASH_BASE)
# endif
# ifndef  CFG_ENV_SIZE
#  define CFG_ENV_SIZE	CFG_ENV_SECT_SIZE
# endif
# if (CFG_ENV_ADDR >= CFG_MONITOR_BASE) && \
     (CFG_ENV_ADDR+CFG_ENV_SIZE) <= (CFG_MONITOR_BASE + CFG_MONITOR_LEN)
#  define ENV_IS_EMBEDDED
# endif
#endif	/* CFG_ENV_IS_IN_FLASH */

#ifdef ENV_IS_EMBEDDED

#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)

/*
 * don't put the environment in it's own section if we are building
 * the host based program "tools/envcrc" (ENV_CRC is only defined when
 * building armboot itself)
 */
	.text

	.globl	environment
environment:
	.globl	env_crc
env_crc:
#ifdef  ENV_CRC  		/* If we are building with computed CRC...	*/
	.long	ENV_CRC  	/* ...use it!					*/
#else
	.long	~0
#endif
	.globl	env_data
env_data:
#ifdef	CONFIG_BOOTARGS
	.ascii	"bootargs="
	.ascii		CONFIG_BOOTARGS
	.ascii		"\0"
#endif
#ifdef	CONFIG_BOOTCOMMAND
	.ascii	"bootcmd="
	.ascii		CONFIG_BOOTCOMMAND
	.ascii		"\0"
#endif
#ifdef	CONFIG_RAMBOOTCOMMAND
	.ascii	"ramboot="
	.ascii		CONFIG_RAMBOOTCOMMAND
	.ascii		"\0"
#endif
#ifdef	CONFIG_NFSBOOTCOMMAND
	.ascii	"nfsboot="
	.ascii		CONFIG_NFSBOOTCOMMAND
	.ascii				"\0"
#endif
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	.ascii	"bootdelay="
	.ascii		MK_STR(CONFIG_BOOTDELAY)
	.ascii		"\0"
#endif
#if defined(CONFIG_BAUDRATE) && (CONFIG_BAUDRATE >= 0)
	.ascii	"baudrate="
	.ascii		MK_STR(CONFIG_BAUDRATE)
	.ascii		"\0"
#endif
#ifdef	CONFIG_LOADS_ECHO
	.ascii	"loads_echo="
	.ascii		MK_STR(CONFIG_LOADS_ECHO)
	.ascii		"\0"
#endif
#ifdef	CONFIG_ETHADDR
	.ascii	"ethaddr="
	.ascii		MK_STR(CONFIG_ETHADDR)
	.ascii		"\0"
#endif
#ifdef	CONFIG_ETH2ADDR
	.ascii	"eth2addr="
	.ascii		MK_STR(CONFIG_ETH2ADDR)
	.ascii		"\0"
#endif
#ifdef	CONFIG_ETH3ADDR
	.ascii	"eth3addr="
	.ascii		MK_STR(CONFIG_ETH3ADDR)
	.ascii		"\0"
#endif
#ifdef	CONFIG_IPADDR
	.ascii	"ipaddr="
	.ascii		MK_STR(CONFIG_IPADDR)
	.ascii		"\0"
#endif
#ifdef	CONFIG_SERVERIP
	.ascii	"serverip="
	.ascii		MK_STR(CONFIG_SERVERIP)
	.ascii		"\0"
#endif
#ifdef	CFG_AUTOLOAD
	.ascii	"autoload="
	.ascii		CFG_AUTOLOAD
	.ascii		"\0"
#endif
#ifdef	CONFIG_PREBOOT
	.ascii	"preboot="
	.ascii		MK_STR(CONFIG_PREBOOT)
	.ascii		"\0"
#endif
	/* terminate list of environment strings */
	.ascii	"\0"
	/* make sure we have two '\0' bytes even when no vars are defined */
	.ascii	"\0"
	. = environment + CFG_ENV_SIZE
.L_end:

	.globl  env_size
env_size:
	.long	.L_end - environment
	.globl	env_offset
	.set	env_offset, CFG_ENV_OFFSET

#endif	/* ENV_IS_EMBEDDED */
