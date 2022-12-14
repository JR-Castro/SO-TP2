// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/test_processes.h"

enum State {RUNNING, BLOCKED, KILLED};

typedef struct P_rq{
    int32_t pid;
    enum State state;
}p_rq;

int test_processes(int argc, char *argv[]){
    uint64_t rq;
    uint64_t alive = 0;
    uint64_t action;
    uint64_t max_processes;
    uint64_t max_cycles;
    uint8_t infinite = 0;
    char * argvAux[] = {"endlessLoop"};

    if (argc < 3) return -1;

    if ((max_processes = satoi(argv[1])) <= 0) return -1;

    if ((max_cycles = satoi(argv[2])) <= 0) infinite = 1;


    p_rq *p_rqs = (p_rq *) sys_alloc(sizeof(p_rq)*max_processes);

    while (infinite || max_cycles--){

        // Create max_processes processes
        for(rq = 0; rq < max_processes; rq++){
            p_rqs[rq].pid = sys_createProcess((int (*)(int, char **)) endless_loop, 1, argvAux);

            if (p_rqs[rq].pid == -1){
                puts("test_processes: ERROR creating process\n");
                return -1;
            }else{
                p_rqs[rq].state = RUNNING;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0){

            for(rq = 0; rq < max_processes; rq++){
                action = GetUniform(100) % 2;

                switch(action){
                    case 0:
                        if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED){
                            if (sys_kill(p_rqs[rq].pid) == -1){
                                puts("test_processes: ERROR killing process\n");
                                return -1;
                            }
                            p_rqs[rq].state = KILLED;
                            alive--;
                        }
                        break;

                    case 1:
                        if (p_rqs[rq].state == RUNNING){
                            if(sys_block(p_rqs[rq].pid) == -1){
                                puts("test_processes: ERROR blocking process\n");
                                return -1;
                            }
                            p_rqs[rq].state = BLOCKED;
                        }
                        break;
                }
            }

            // Randomly unblocks processes
            for(rq = 0; rq < max_processes; rq++)
                if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2){
                    if(sys_unblock(p_rqs[rq].pid) == -1){
                        puts("test_processes: ERROR unblocking process\n");
                        return -1;
                    }
                    p_rqs[rq].state = RUNNING;
                }
        }
    }
    return 0;
}
