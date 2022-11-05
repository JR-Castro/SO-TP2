// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/block.h"

int block(int argc, char ** argv) {
    if (argc < 2) {
        puts("Usage: block <pid>");
        return -1;
    }
    while (argc > 1) {
        int pid = satoi(argv[argc - 1]);
        if (pid > 0) {
            switch (sys_block(pid)) {
                case -1:
                    puts("Process doesn't exist");
                    break;
                case 0:
                    puts("Process blocked");
                    break;
                case 1:
                    sys_unblock(pid);
                    puts("Process unblocked");
                    break;
            }
        }
        argc--;
    }
    return 0;
}
