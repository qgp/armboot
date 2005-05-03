#
# (C) Copyright 2002
# Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
#
# SAMSUNG board with S3C2400X (arm920t) cpu
#
# see http://www.samsung.com/ for more information on SAMSUNG
#

#
# SAMSUNG has 1 bank of 32 MB DRAM
#
# 0c00'0000 to 0e00'0000
#
# Linux-Kernel is expected to be at 0cf0'0000, entry 0cf0'0000
# optionally with a ramdisk at 0c80'0000
#
# we load ourself to 0c17'0000
#
# download areas is 0c80'0000
#


TEXT_BASE = 0x0c170000
