/*
 * Copyright (c) 2002 Altera Corporation, San Jose, California, USA.  
 * All rights reserved.
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
extern ulong amd_flash_init(bd_t *bd);
extern void amd_flash_print_info(flash_info_t *info);
extern int amd_flash_erase(flash_info_t *info, int s_first, int s_last);
extern int amd_write_buff(flash_info_t *info, uchar *src, ulong addr, ulong cnt);
extern int amd_flash_read_user(int bank, int offset);
