#
# (C) Copyright 2000
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

BINS	= img2srec mkimage envcrc gen_eth_addr gen_env gen_charm_env

OBJS	= img2srec.o mkimage.o crc32.o envcrc.o gen_eth_addr.o

TOOLSUBDIRS =

#
# Use native tools and options
#
CPPFLAGS   = -Wall -I../include -I.. -DTEXT_BASE=$(TEXT_BASE) -m32
CFLAGS     = $(CPPFLAGS) -O -DHOST_TOOLS
AFLAGS	   = -D__ASSEMBLY__ $(CPPFLAGS)
CC	   = $(HOSTCC)
MAKEDEPEND = makedepend

all:	.depend $(BINS) subdirs

envcrc:		envcrc.o crc32.o environment.o
		$(CC) $(CFLAGS) -o $@ $^

gen_env:	gen_env.o crc32.o environment.o
		$(CC) $(CFLAGS) -o $@ $^

gen_charm_env:	gen_charm_env.o crc32.o environment.o
		$(CC) $(CFLAGS) -o $@ $^

environment.S:	../common/environment.S
		ln -sf $^ $@

img2srec:	img2srec.o
		$(CC) $(CFLAGS) -s -o $@ $^

mkimage:	mkimage.o crc32.o
		$(CC) -g $(CFLAGS) -o $@ $^

gen_eth_addr:	gen_eth_addr.o
		$(CC) -g $(CFLAGS) -o $@ $^

envcrc.o:	envcrc.c
		$(CC) -g $(CFLAGS) -c $<

crc32.o:	crc32.c
		$(CC) -g $(CFLAGS) -c $<

mkimage.o:	mkimage.c
		$(CC) -g $(CFLAGS) -c $<

gen_eth_addr.o:	gen_eth_addr.c
		$(CC) -g $(CFLAGS) -c $<

subdirs:
#		@for dir in $(TOOLSUBDIRS) ; do $(MAKE) -C $$dir || exit 1 ; done

crc32.c:
		ln -s ../common/crc32.c crc32.c

#########################################################################

.depend:	Makefile $(OBJS:.o=.c)
		$(CC) -M $(CPPFLAGS) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
