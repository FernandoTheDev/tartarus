; x86_64-linux

section .text
    global _start

_start:
    mov rax, 60
    xor rdi, rdi
    syscall


; my _version (NoTimeDev)
; .arch x86_64 
; .global _start
; section .text
; _start:
;     movq rax, 60 
;     xorq rdi, rdi 
;     syscall 
