GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_getregs
GLOBAL sys_createProcess
GLOBAL sys_waitpid
GLOBAL sys_time
GLOBAL sys_copymem
GLOBAL sys_getpid
GLOBAL sys_block
GLOBAL sys_unblock
GLOBAL sys_kill
GLOBAL sys_yield
GLOBAL sys_nice
GLOBAL sys_printSchedulerInfo
GLOBAL sys_alloc
GLOBAL sys_free
GLOBAL sys_memInfo
GLOBAL sys_sem_open
GLOBAL sys_sem_wait
GLOBAL sys_sem_post
GLOBAL sys_sem_close
GLOBAL sys_create_pipe
GLOBAL sys_connect_named_pipe
GLOBAL sys_dup2
GLOBAL sys_print_pipe_info
GLOBAL sys_close_pipe
GLOBAL sys_exit
GLOBAL sys_get_sems

GLOBAL inv_opcode
GLOBAL div_zero

section .text

%macro syscallHandler 1
    push rbp
    mov rbp, rsp
    mov rax, %1
    int 0x80
    mov rsp, rbp
    pop rbp
    ret
%endmacro

sys_read:
    syscallHandler 0

sys_write:
    syscallHandler 1

sys_getregs:
    syscallHandler 2

sys_createProcess:
    syscallHandler 3

sys_waitpid:
    syscallHandler 4

sys_time:
    syscallHandler 5

sys_copymem:
    syscallHandler 6

sys_getpid:
    syscallHandler 7

sys_block:
    syscallHandler 8

sys_unblock:
    syscallHandler 9

sys_kill:
    syscallHandler 10

sys_yield:
    syscallHandler 11

sys_nice:
    syscallHandler 12

sys_printSchedulerInfo:
    syscallHandler 13

sys_alloc:
    syscallHandler 14

sys_free:
    syscallHandler 15

sys_memInfo:
    syscallHandler 16

sys_sem_open:
    syscallHandler 17

sys_sem_wait:
    syscallHandler 18

sys_sem_post:
    syscallHandler 19

sys_sem_close:
    syscallHandler 20

sys_create_pipe:
    syscallHandler 21

sys_connect_named_pipe:
    syscallHandler 22

sys_dup2:
    syscallHandler 23

sys_print_pipe_info:
    syscallHandler 24

sys_close_pipe:
    syscallHandler 25

sys_exit:
    syscallHandler 26

sys_get_sems:
    syscallHandler 27

inv_opcode:
    ud2
    ret

div_zero:
    mov ecx, 0
    div ecx
    ret
