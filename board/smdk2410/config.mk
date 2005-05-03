#
# (C) Copyright 2002
# Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
# David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
#
# SAMSUNG SMDK2410 board with S3C2410X (ARM920T) cpu
#
# see http://www.samsung.com/ for more information on SAMSUNG
#

#
# SMDK2410 has 1 bank of 32 MB DRAM
#
# 3000'0000 to 3200'0000
#
# Linux-Kernel is expected to be at 3000'0000, entry 3000'0000
# optionally with a ramdisk at 3080'0000
#
# we load ourself to 31F0'0000
#
# download areas is 3100'0000
#


TEXT_BASE = 0x31F00000
