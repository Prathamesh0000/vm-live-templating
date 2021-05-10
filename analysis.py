#! /usr/bin/env python
import re
import ctypes
import fcntl

import ioctl
import fcntl

BTRFS_IOCTL_MAGIC = 0x94
BTRFS_IOC_SYNC =  ioctl.IO(BTRFS_IOCTL_MAGIC, 8)
BTRFS_IOC_CLONE = ioctl.IOW(BTRFS_IOCTL_MAGIC, 9, INT)

maps_file = open("/proc/3896/maps", 'r')
for line in maps_file.readlines():  # for each mapped region
    m = re.match(r'([0-9A-Fa-f]+)-([0-9A-Fa-f]+) ([-r])', line)
    print(m.group(2) + "-" + m.group(2))


maps_file.close()