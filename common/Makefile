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

LIB	= libcommon.a

ifneq ($(BOARD),cpci405)
AOBJS	= environment.o
endif
COBJS	= board.o main.o command.o \
	  cmd_boot.o cmd_bootm.o cmd_cache.o \
	  cmd_eeprom.o \
	  cmd_flash.o cmd_mem.o cmd_net.o \
	  cmd_nvedit.o cmd_autoscript.o cmd_misc.o cmd_jffs2.o \
	  s_record.o dlmalloc.o \
	  console.o lists.o devices.o flash.o \
	  string.o ctype.o vsprintf.o string.o \
	  display_options.o zlib.o crc32.o ldiv.o \
	  armlinux.o

OBJS	= $(AOBJS) $(COBJS)

ifneq ($(BOARD),epxa1db)
COBJS += cmd_ide.o cmd_pcmcia.o
endif

CPPFLAGS += -I..

all:	$(LIB) $(AOBJS)

$(LIB):	.depend $(COBJS)
	$(AR) crv $@ $(COBJS)

environment.o: environment.S ../tools/envcrc
	$(CC) $(AFLAGS) -DENV_CRC=$(shell ../tools/envcrc) -c -o $@ environment.S

#########################################################################

.depend:	Makefile $(AOBJS:.o=.S) $(COBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(AOBJS:.o=.S) $(COBJS:.o=.c) > $@

sinclude .depend

#########################################################################
