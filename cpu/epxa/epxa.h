/*
 * (C) Copyright 2015 Jochen Klein
 * All rights reserved.
 *
 * This program is free software ; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation ; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY ; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program ; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
*/

#define BOOT_CR_ADDR    (0x7fffc000)
#define BOOT_CR_VALUE   (0x00000001)

#define MMAP_EBI0_ADDR        (0x7fffc0c0)
#define MMAP_EBI0_VALUE       (0x40000b01)
#define MMAP_EBI0_BASE        (0x40000000)

#define MMAP_REGISTERS_ADDR   (0x7fffc080)
#define MMAP_REGISTERS_VALUE  (0x7fffc683)
#define MMAP_REGISTERS_BASE   (0x7fffc000)

#define MMAP_DPSRAM0_ADDR     (0x7fffc0a0)
#define MMAP_DPSRAM0_VALUE    (0x20000681)
#define MMAP_DPSRAM0_BASE     (0x20000000)
