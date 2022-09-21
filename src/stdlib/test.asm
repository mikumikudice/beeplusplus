segment .data
segment .rodata
    msg dd 12 
        dd 12
        db 'hello world!', 10

    arw db '> '
segment .bss

segment .text
    global _start
    extern getc, putc, puts, exit

_start:
    mov   eax ,  msg
    call  puts

    mov   eax ,  arw
    call  putc

    call  getc
    call  putc

    mov   eax ,  0
    call  exit
