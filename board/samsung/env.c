/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
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

#include "armboot.h"

static int check_crc(bd_t *bd)
{
    /* need to calculate crc? */
    if (bd->bi_ext.env_crc_valid == 0)
    {
	env_t *env = (env_t *)CFG_ENV_ADDR;
    	
	if (crc32(0, env->data, sizeof(env->data)) == env->crc)
	  bd->bi_ext.env_crc_valid = 1;
	else
	  bd->bi_ext.env_crc_valid = -1;
    }
    return bd->bi_ext.env_crc_valid > 0;
}


/*
 * save environment buffer back to flash
 * returns -1 on error, 0 if ok
 */
int board_env_save(bd_t *bd, env_t *env, int size)
{
    int rc;
    ulong start_addr, end_addr;

#if CFG_ENV_SIZE <= CFG_ENV_SECT_SIZE
#error Make sure that CFG_ENV_SIZE <= CFG_ENV_SECT_SIZE
#endif    

    start_addr = CFG_ENV_ADDR;
    end_addr   = start_addr + CFG_ENV_SIZE - 1;

    rc = flash_sect_protect(0, CFG_ENV_ADDR, end_addr);
    if (rc < 0)
      return rc;

    rc = flash_sect_erase(start_addr, end_addr);
    if (rc < 0)
    {
	flash_sect_protect(1, start_addr, end_addr);
	flash_perror(rc);
	return rc;
    }
    
    printf("Saving Environment to Flash...");
    rc = flash_write((uchar*)env, start_addr, size);
    if (rc < 0)
      flash_perror(rc);
    else 
      printf("done.\n");

    (void)flash_sect_protect(1, start_addr, end_addr);

    return 0;
}

/*
 * copy environment to memory
 * returns -1 on error, 0 if ok
 */
int board_env_copy(bd_t *bd, env_t *data, int size)
{
    env_t *env = (env_t *)CFG_ENV_ADDR;

    if (check_crc(bd))
    {
	memcpy(data, env, sizeof(env_t));
	return 0;
    } 

    return -1;
}

/* 
 * try to read env character at offset #index
 * 
 * called before the environment is copied to ram
 * returns -1 on error, 0 if ok
 */
int board_env_getchar(bd_t * bd, int index, uchar *c)
{
    env_t *env = (env_t *)CFG_ENV_ADDR; 
   
    /* check environment crc */
    if (index < sizeof(env->data) && check_crc(bd))
    {
	*c = env->data[index];
	return 0;
    }

    return -1;
}

/* 
 * try to read env character at offset #index
 * 
 * called before the environment is copied to ram
 * returns -1 on error, 0 if ok
 */
uchar *board_env_getaddr(bd_t * bd, int index)
{
    env_t *env = (env_t *)CFG_ENV_ADDR; 
   
    /* check environment crc */
    if (index < sizeof(env->data) && check_crc(bd))
      return &env->data[index];
   
    return 0;
}
