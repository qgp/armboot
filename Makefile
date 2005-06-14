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

CPU = 
BOARD =
VENDOR =

ifndef CROSS_COMPILE
ifeq ($(HOSTARCH),arm)
CROSS_COMPILE =
else
CROSS_COMPILE = /usr/local/arm/2.95.3/bin/arm-linux-
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

MKIMDIR = tools

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

all:		armboot.srec armboot.hex armboot.bin

install:	all
		cp armboot.bin /tftpboot/armboot.bin

armboot.srec:	armboot
		$(OBJCOPY) ${OBJCFLAGS} -O srec $< $@

armboot.hex:	armboot
		srec_cat armboot.srec -o armboot.hex -intel

armboot.bin:	armboot
		$(OBJCOPY) ${OBJCFLAGS} -O binary $< $@

armboot:	depend subdirs $(OBJS) $(LDSCRIPT)
		$(LD) $(LDFLAGS) $(LDFLAGS_EXTRA) $(OBJS) $(LIBGCC) $(EXTERN_LIB) -Map armboot.map -o armboot 

subdirs:
		@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir || exit 1 ; done

mkim:
		@for dir in $(MKIMDIR) ; do $(MAKE) -C $$dir || exit 1 ; done

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
	rm -f include/config.h include/config.mk include/epxa/excalibur.h


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

lubbock_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ; \
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = lubbock"	>>config.mk ;	\
	echo "CPU   = xscale"	>>config.mk ;	\
	echo "#include <configs/config_$(@:_config=).h>" >config.h

cradle_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ; \
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = cradle"	>>config.mk ;	\
	echo "CPU   = xscale"	>>config.mk ;	\
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

smdk2410_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ; \
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = smdk2410"	>>config.mk ;	\
	echo "CPU   = arm920t"	>>config.mk ;	\
	echo "#include <configs/config_$(@:_config=).h>" >config.h

#########################################################################
## ARM922T Systems
#########################################################################

epxa1db_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ; \
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = epxa1db"	>>config.mk ;	\
	echo "CPU   = epxa"	>>config.mk ;	\
	echo "#include <configs/config_$(@:_config=).h>" >config.h
	@ln -sf ../../board/$(@:_config=)/quartus/excalibur.h include/epxa/excalibur.h 
	@echo "** See the README in board/epxa1db for build instructions **"

kipdcs_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ; \
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = kipdcs"	>>config.mk ;	\
	echo "CPU   = epxa"	>>config.mk ;	\
	echo "#include <configs/config_$(@:_config=).h>" >config.h
	@ln -sf ../../board/$(@:_config=)/quartus/excalibur.h include/epxa/excalibur.h 
	@echo "** See the README in board/kipdcs for build instructions **"

kipcharm_config	:	unconfig
	@echo "Configuring for $(@:_config=) Board..." ; \
	cd include ; \
	echo "ARCH  = arm"	> config.mk ;	\
	echo "BOARD = kipcharm"	>>config.mk ;	\
	echo "CPU   = epxa"	>>config.mk ;	\
	echo "#include <configs/config_$(@:_config=).h>" >config.h
	@ln -sf ../../board/$(@:_config=)/quartus/excalibur.h include/epxa/excalibur.h 
	@echo "** See the README in board/kipcharm for build instructions **"

#########################################################################

clean:
	find .  -path './tools/quartus' -prune -o -type f \
		\( -name 'core' -o -name '*.bak' -o -name '*~' \
		-o -name '*.o'  -o -name '*.a' -o -name '.depend' \) -print \
		| xargs rm -f
	rm -f examples/hello_world examples/timer
	rm -f tools/img2srec tools/mkimage tools/envcrc tools/gen_eth_addr
	rm -f tools/easylogo/easylogo
	rm -f tools/gdb/astest tools/gdb/gdbcont tools/gdb/gdbsend
	rm -f tools/gen_env

clobber:	clean
	rm -f $(OBJS) *.bak tags TAGS
	rm -fr *.*~
	rm -f armboot armboot.bin armboot.elf armboot.srec armboot.map armboot.hex
	rm -f tools/crc32.c tools/environment.S

mrproper \
distclean:	clobber unconfig

backup:
	F=`basename $(TOPDIR)` ; cd .. ; \
	gtar --force-local -zcvf `date "+$$F-%Y-%m-%d-%T.tar.gz"` $$F

#########################################################################
