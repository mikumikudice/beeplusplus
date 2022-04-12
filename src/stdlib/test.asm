segment .data

segment .rodata

segment .bss
    buff resb 4

segment .text
global _start

%include "stdio.asm"

_start:
    call  getc
    call  putc
    
    mov   eax ,  0
    call  exit
