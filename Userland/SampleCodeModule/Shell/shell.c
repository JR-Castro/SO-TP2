// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// Shell based on https://fundamental-code.com/interp/

#include "../include/shell.h"

typedef char* arg_t;

typedef struct {
    const char *name;
    int (*func)(int argc, char *argv[]);
    const int args;
    const char *doc;
} cmd_t;

void help();
void mem();

arg_t *args_parse(const char *name, int argc);

#define CMDS 10
cmd_t dsp_table[CMDS] = {
        {"help",         (int (*)(int, char **)) help,                      0, "Show this help"},
        {"mem",          (int (*)(int, char **)) printmeminfo,              0, "Show memory status"},
        {"ps",           (int (*)(int, char **)) sys_printSchedulerInfo,    0, "Show processes"},
        {"kill",         (int (*)(int, char **)) kill,                      1, "Kill a process"},
        {"nice",         (int (*)(int, char **)) sys_nice,                  1, "Change priority of a process"},
//        {"sem", , 0, "Show semaphore info"},
//        {"cat", cat, 0, "Print input to output"},
//        {"wc", wordcount, 0, "Count lines in input"},
//        {"filter", filtervocals, 0, "Filter vocals from input"},
        {"pipe",         (int (*)(int, char **)) sys_print_pipe_info,       0, "Show pipe status"},
//        {"phylo", phylo, 0, "Interactive philosophers"},
        {"processtest",  (int (*)(int, char **)) test_processes,            1, "Test process creation, blocking and killing"},
        {"prioritytest", (int (*)(int, char **)) test_prio,                 0, "Test priority"},
        {"synctest",     (int (*)(int, char **)) test_sync,                 2, "Test synchronization primitives"},
        {"memtest",      (int (*)(int, char **)) test_mm,                   0, "Test memory allocation and freeing (preferably in background)"},
//        {"exit",         (int (*)(int, char **)) sys_exit,                  0, "Exit the shell"},
};

const char *delim = " ";

void * parse(char *cmd, char ***argv, int *argc) {
    const char *tok = strtok(cmd, delim);
    if (!tok) return NULL;

    int i = CMDS;
    while (i--) {
        cmd_t cur = dsp_table[i];
        if (!strcmp(tok, cur.name)) {
            arg_t *args = args_parse(cur.name, cur.args);
            if (args == NULL && cur.args) {
                return NULL;
            }
            *argv = args;
            *argc = cur.args + 1;
            return cur.func;
        }
    }
    puts("Command not found");
    return NULL;
}

arg_t *args_parse(const char *name, int argc) {
    arg_t *args = sys_alloc(sizeof(arg_t) * (argc + 1));
    args[0] = sys_alloc(strlen(name + 1));
    if (args[0] == NULL) return NULL;
    strcpy(args[0], name);
    for (int i = 1; i < argc + 1; ++i) {
        char *newarg = strtok(NULL, delim);
        if (!newarg) {
            for (int j = 0; j < i; ++j) {   // Free all previous arguments
                sys_free(args[j]);
            }
            return NULL;
        }
        args[i] = sys_alloc(strlen(newarg) + 1);
        strcpy(args[i], newarg);
    }
    return args;
}

char prompt[100];

int getInput(char *s) {
    int i = 0, c = getchar();
    while (c != '\n' && c != EOF) {
        s[i] = c;
        if (s[i] == '\b' && i > 0) {
            i--;
            putchar(c);
        } else if (s[i] == '\b' && i == 0) {
            // do nothing
        } else {
            i++;
            putchar(c);
        }
        c = getchar();
    }
    s[i] = '\0';
    return i;
}

_Noreturn int shell() {
    char **argv1, **argv2;
    int argc1, argc2;

    strncpy(prompt, "Shell> ", 100);

    while (1) {
        fputs(prompt, STDOUT);
        char *cmd = sys_alloc(200);
        if (cmd == NULL) {
            puts("Error allocating memory for command");
            continue;
        }
        getInput(cmd);
        putchar('\n');

        // Get first function with argc and arguments
        int (*f1)(int, char**) = parse(cmd, &argv1, &argc1);

        if (f1 == NULL) {
            puts("Error parsing command");
            sys_free(cmd);
            continue;
        }

        char *next = strtok(NULL, delim);
        if (next == NULL) {
            int pid = sys_createProcess((void (*)(int, char **)) f1, argc1, argv1);
            sys_waitpid(pid);
            putchar('\n');
            sys_free(cmd);
            continue;
        }
        if (*next == '&') {
            sys_createProcess((void (*)(int, char **)) f1, argc1, argv1);
            putchar('\n');
            sys_free(cmd);
            continue;
        }
        if (*next == '|') {
            int (*f2)(int, char**) = parse(NULL, &argv2, &argc2);
            if (f2 == NULL) {
                puts("Error parsing command");
                sys_free(cmd);
                continue;
            }
            uint64_t fd[2];
            if (sys_create_pipe(fd)){
                puts("Error creating pipe");
                sys_free(cmd);
                continue;
            }
            sys_dup2(fd[1], 1);
            int pida = sys_createProcess((void (*)(int, char **)) f1, argc1, argv1);
            sys_dup2(fd[0], 0);
            int pidb = sys_createProcess((void (*)(int, char **)) f2, argc2, argv2);
            sys_close_pipe(fd[0]);
            sys_close_pipe(fd[1]);
            sys_close_pipe(0);
            sys_close_pipe(1);
            sys_waitpid(pida);
            sys_waitpid(pidb);
            putchar('\n');
            sys_free(cmd);
            continue;
        }
    }
}

void help() {
    puts("Available commands:");
    int i = CMDS;
    while (i--) {
        cmd_t cmd = dsp_table[i];
        fputs(cmd.name, STDOUT);
        fputs(" - ", STDOUT);
        fputs(cmd.doc, STDOUT);
        fputs("\n", STDOUT);
    }
}
