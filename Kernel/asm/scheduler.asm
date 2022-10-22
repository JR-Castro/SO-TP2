GLOBAL forceTimerTick
GLOBAL setupStack

section .text

forceTimerTick:
    int 0x20
    ret

;uint64_t setupStack(uint64_t startStack, uint64_t loader, uint64_t argc, uint64_t argv, uint64_t rip)
setupStack:
    push rbp
    mov rbp, rsp

    mov rsp, rdi
    push 0x00   ;ss
    push rdi    ;rsp
    push 0x202  ;default rflags
    push 0x8    ;cs
    push rsi    ;rip

;    push rax
    push 0x1
;    push rbx
    push 0x2
;    push rcx
    push 0x3
    push r8     ; function pointer goes in r8
;    push rbp
    push 0x4
    push rdx    ; argc goes into rdi
    push rcx    ; argv goes into rsi
;    push r8
    push 0x5
;    push r9
    push 0x6
;    push r10
    push 0x7
;    push r11
    push 0x8
;    push r12
    push 0x9
;    push r13
    push 0xa
;    push r14
    push 0xb
;    push r15
    push 0xc

    mov rax, rsp

    mov rsp, rbp
    pop rbp
    ret


