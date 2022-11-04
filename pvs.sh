#!/bin/bash
make clean -C /root/Toolchain > /dev/null
make clean -C /root/ > /dev/null
make -C /root/Toolchain > /dev/null
pvs-studio-analyzer trace -- make MM=$1 gdb -C /root/ > /dev/null
pvs-studio-analyzer analyze > /dev/null 2> /dev/null
plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log  > /dev/null
rm -f strace_out
rm -f PVS-Studio.log