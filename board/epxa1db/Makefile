#
# Copyright (c) 2000-2002 Altera Corporation, San Jose, California, USA.  
# All rights reserved.
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

# If a full installation of Quartus isn't available, use the tools 
# supplied with armboot.

ifndef QUARTUS_ROOTDIR
QUARTUS_ROOTDIR = $(TOPDIR)/tools/quartus
endif


MAKEPROGFILE := $(QUARTUS_ROOTDIR)/bin/makeprogfile

LIB	= lib$(BOARD).a

OBJS	:= epxa1db.o flash.o env.o mac_addr.o flash_int.o flash_amd.o

EXC_CFG := excalibur.o 

all: $(LIB) $(EXC_CFG)

$(LIB):	$(OBJS)
	$(AR) crv $@ $^

$(EXC_CFG): quartus/excalibur.sbd
	$(MAKEPROGFILE) -q -b $@ $<

clean:
	rm -f $(EXC_CFG) $(OBJS)

distclean:	clean
	rm -f $(LIB) core *.bak .depend


#########################################################################

.depend:	Makefile $(SOBJS:.o=.S) $(OBJS:.o=.c)
		$(CC) -M $(CPPFLAGS) $(SOBJS:.o=.S) $(OBJS:.o=.c) > $@

-include .depend

#########################################################################
