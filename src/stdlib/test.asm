segment .data
segment .rodata
    msg db '> '
segment .bss

segment .text
    global _start
    extern getc, putc, exit

_start:
    mov   eax ,  msg
    call  putc

    call  getc
    call  putc
    
    mov   eax ,  0
    call  exit
