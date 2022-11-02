// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// Shell based on https://fundamental-code.com/interp/

#include "../include/shell.h"

typedef char *arg_t;

typedef struct {
    const char *name;

    int (*func)(int argc, char *argv[]);

    const int args;
    const char *doc;
} cmd_t;

void help();

void mem();

arg_t *args_parse(const char *name, int argc);

#define CMDS 17
cmd_t dsp_table[CMDS] = {
        {"help",         (int (*)(int, char **)) help,                0, "Show this help"},
        {"mem",          (int (*)(int, char **)) printmeminfo,        0, "Show memory status"},
        {"ps",           (int (*)(int, char **)) ps,                  0, "Show processes"},
        {"loop",         (int (*)(int, char **)) loop,                0, "Start a loop process"},
        {"kill",         (int (*)(int, char **)) kill,                1, "Kill a process"},
        {"nice",         (int (*)(int, char **)) sys_nice,            1, "Change priority of a process"},
        {"block",        block,                                       1, "Block a process"},
        {"sem",          semsinfo,                                    0, "Show semaphore info"},
        {"cat",          cat,                                         0, "Print input to output"},
        {"wc",           (int (*)(int, char **)) wordcount,           0, "Count lines in input"},
        {"filter",       filter,                                      0, "Filter vocals from input"},
        {"pipe",         (int (*)(int, char **)) sys_print_pipe_info, 0, "Show pipe status"},
        {"phylo",        (int (*)(int, char **)) philosophers,        0, "Interactive philosophers"},
        {"processtest",  (int (*)(int, char **)) test_processes,      2, "Test process creation, blocking and killing"},
        {"prioritytest", (int (*)(int, char **)) test_prio,           0, "Test priority"},
        {"synctest",     (int (*)(int, char **)) test_sync,           2, "Test synchronization primitives"},
        {"memtest",      (int (*)(int,
                                  char **)) test_mm,                  0, "Test memory allocation and freeing (preferably in background)"},
//        {"exit",         (int (*)(int, char **)) sys_exit,                  0, "Exit the shell"},
};

const char *delim = " ";

void freeArgs(arg_t *args, int argc) {
    for (int i = 0; i < argc; ++i) {
        sys_free(args[i]);
    }
    sys_free(args);
}

void *parse(char *cmd, char ***argv, int *argc) {
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
            freeArgs(args, i);      // Free all previous arguments
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

// pipe[0]: Lectura
// pipe[1]: Escritura
int pipe[2];
int pid[2];

static void createWriterProcessWithPipe(int argc, char **argv) {
    int writer = 1;
    sys_close_pipe(pipe[(writer + 1) % 2]);
    sys_dup2(pipe[writer], writer);
    sys_close_pipe(pipe[writer]);
    int i = CMDS;
    while (i--) {
        cmd_t cur = dsp_table[i];
        if (strcmp(cur.name, argv[0]) == 0) {
            pid[writer] = sys_createProcess((void (*)(int, char **)) cur.func, argc, argv);
            break;
        }
    }
}

static void createReaderProcessWithPipe(int argc, char **argv) {
    int writer = 0;
    sys_close_pipe(pipe[(writer + 1) % 2]);
    sys_dup2(pipe[writer], writer);
    sys_close_pipe(pipe[writer]);
    int i = CMDS;
    while (i--) {
        cmd_t cur = dsp_table[i];
        if (strcmp(cur.name, argv[0]) == 0) {
            pid[writer] = sys_createProcess((void (*)(int, char **)) cur.func, argc, argv);
            break;
        }
    }
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
        int (*f1)(int, char **) = parse(cmd, &argv1, &argc1);

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
            freeArgs(argv1, argc1);
            continue;
        }
        if (*next == '&') {
            sys_createProcess((void (*)(int, char **)) f1, argc1, argv1);
            putchar('\n');
            sys_free(cmd);
            freeArgs(argv1, argc1);
            continue;
        }
        if (*next == '|') {
            int (*f2)(int, char **) = parse(NULL, &argv2, &argc2);
            if (f2 == NULL) {
                puts("Error parsing command");
                sys_free(cmd);
                freeArgs(argv1, argc1);
                continue;
            }
            if (sys_create_pipe((uint64_t *) pipe)) {
                puts("Error creating pipe");
                sys_free(cmd);
                freeArgs(argv1, argc1);
                freeArgs(argv2, argc2);
                continue;
            }
            int aux1, aux2;
            aux1 = sys_createProcess(createWriterProcessWithPipe, argc1, argv1);
            aux2 = sys_createProcess(createReaderProcessWithPipe, argc2, argv2);
            sys_waitpid(aux1);
            sys_waitpid(aux2);
            sys_close_pipe(pipe[0]);
            sys_close_pipe(pipe[1]);
            sys_waitpid(pid[0]);
            sys_waitpid(pid[1]);
            putchar('\n');
            sys_free(cmd);
            freeArgs(argv1, argc1);
            freeArgs(argv2, argc2);
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
