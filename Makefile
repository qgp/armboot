#
# (C) Copyright 2000, 2002
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# (C) Copyright 2002
# Sysgo Real-Time Solutions, GmbH <www.elinos.com>
# Marius Groeger <mgroeger@sysgo.de>
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

HOSTARCH := $(shell uname -m | \
	sed -e s/i.86/i386/ \
	    -e s/sun4u/sparc64/ \
	    -e s/arm.*/arm/ \
	    -e s/sa110/arm/)

ifndef CROSS_COMPILE
ifeq ($(HOSTARCH),arm)
CROSS_COMPILE =
else
CROSS_COMPILE = arm-linux-
endif
endif

export	CROSS_COMPILE HOSTARCH

#########################################################################

TOPDIR	:= $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
export	TOPDIR

ifeq (include/config.mk,$(wildcard include/config.mk))
# load ARCH, BOARD, and CPU configuration
include include/config.mk
export	ARCH CPU BOARD VENDOR
# load other configuration
include $(TOPDIR)/config.mk

SUBDIRS	= tools \
	  cpu/$(CPU) \
	  board/$(BOARDDIR) \
	  drivers \
	  common \
	  fs \
	  net \
	  disk

#########################################################################
# armboot objects....order is important (i.e. start must be first)

OBJS  =	cpu/$(CPU)/start.o common/libcommon.a

OBJS +=	fs/jffs2/libjffs2.a net/libnet.a disk/libdisk.a

OBJS +=	board/$(BOARDDIR)/lib$(BOARD).a	cpu/$(CPU)/lib$(CPU).a

OBJS +=	drivers/libdrivers.a

# include libcommon last for common low level functions referenced
# by the other libraries
OBJS +=	common/libcommon.a

#########################################################################

all:		armboot.srec armboot.bin

install:	all
		cp armboot.bin /tftpboot/armboot.bin

armboot.srec:	armboot
		$(OBJCOPY) ${OBJCFLAGS} -O srec $< $@

armboot.bin:	armboot
		$(OBJCOPY) ${OBJCFLAGS} -O binary $< $@

armboot:	depend subdirs $(OBJS) $(LDSCRIPT)
		$(LD) $(LDFLAGS) $(OBJS) -Map armboot.map -o armboot $(LIBGCC)

subdirs:
		@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir || exit 1 ; done

depend dep:
		@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir .depend ; done

tags:
		ctags -w `find $(SUBDIRS) include \
			\( -name CVS -prune \) -o \( -name '*.[ch]' -print \)`

#########################################################################
else
all install armboot armboot.srec depend dep:
	@echo "System not configured - see README" >&2
	@ exit 1
endif

#########################################################################

unconfig:
	rm -f include/config.h include/config.mk

#########################################################################
## ARMv4 Systems
#########################################################################

impa7_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ;				\
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = impa7"	>>config.mk ;	\
	echo "CPU   = arm720t"	>>config.mk ;	\
	echo "#include <configs/config_$(@:_config=).h>" >config.h

ep7312_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ;				\
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = ep7312"	>>config.mk ;	\
	echo "CPU   = arm720t"	>>config.mk ;	\
	echo "#include <configs/config_$(@:_config=).h>" >config.h

lart_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ; \
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = lart"	>>config.mk ;	\
	echo "CPU   = sa1100"	>>config.mk ;	\
	echo "#include <configs/config_$(@:_config=).h>" >config.h

dnp1110_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ; \
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = dnp1110"	>>config.mk ;	\
	echo "CPU   = sa1100"	>>config.mk ;	\
	echo "#include <configs/config_$(@:_config=).h>" >config.h


shannon_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ; \
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = shannon"	>>config.mk ;	\
	echo "CPU   = sa1100"	>>config.mk ;	\
	echo "#include <configs/config_$(@:_config=).h>" >config.h

#########################################################################
## ARM920T Systems
#########################################################################

samsung_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ; \
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = samsung"	>>config.mk ;	\
	echo "CPU   = arm920t"	>>config.mk ;	\
	echo "#include <configs/config_$(@:_config=).h>" >config.h

#########################################################################

clean:
	find . -type f \
		\( -name 'core' -o -name '*.bak' -o -name '*~' \
		-o -name '*.o'  -o -name '*.a'  \) -print \
		| xargs rm -f
	rm -f examples/hello_world examples/timer
	rm -f tools/img2srec tools/mkimage tools/envcrc tools/gen_eth_addr
	rm -f tools/easylogo/easylogo
	rm -f tools/gdb/astest tools/gdb/gdbcont tools/gdb/gdbsend

clobber:	clean
	rm -f $(OBJS) *.bak tags TAGS
	rm -fr *.*~
	rm -f armboot armboot.bin armboot.elf armboot.srec armboot.map
	rm -f tools/crc32.c tools/environment.S

mrproper \
distclean:	clobber unconfig

backup:
	F=`basename $(TOPDIR)` ; cd .. ; \
	gtar --force-local -zcvf `date "+$$F-%Y-%m-%d-%T.tar.gz"` $$F

#########################################################################
