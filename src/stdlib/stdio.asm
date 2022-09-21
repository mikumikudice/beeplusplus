; usage:
; ==============================================
; input order (max 6 arguments):
; EDI ESI EDX ECX EBX EAX
; ==============================================
; output order (max 6 return values):
; EAX EBX ECX EDX ESI EDI

segment .bss
    buff resb 4

; getc returns 4 byte char read from the input stream
; returns word char in EAX

segment .text
global getc, putc, puts, exit

getc:
    mov   ecx ,  buff
    mov   edi ,  4
    .read:
    mov   edx ,  01h                ; number of to-be-read bytes
    mov   ebx ,  02h                ; set as an input call
    mov   eax ,  03h                ; syscall number
    int   80h

    mov   eax , [ecx]               ; check if return was pressed
    cmp   al  , 0ah
    jz   .endr

    inc   ecx                       ; set register for next interaction
    dec   edi

    cmp   edi ,  0                  ; loop if it's not the end
    jnz  .read
    .endr:
    mov   eax ,  buff               ; set return value to default output register
    ret

; putc(char)
; putc prints all the four bytes of eax
putc:
    mov   ebx ,  4                  ; number of bytes to be printed
    mov   ecx ,  eax                ; move the given argument to the required register
    .prnt:
    push  ebx                       ; save count for later use

    mov   edx ,  1                  ; set syscall to output operation
    mov   ebx ,  1                  ; number of to-be-printed bytes
    mov   eax ,  4                  ; syscall number
    int   80h

    pop   ebx                       ; restore count

    mov   edx , [ecx]               ; check if it's a line feed
    cmp   dl  ,  0ah
    je   .endp

    inc   ecx
    dec   ebx

    cmp   ebx ,  0
    jnz  .prnt
    .endp:
    ret

; puts(char[])
; puts prints a string. If it ends with newline, it doesn't print one at the end
; otherwise does
;
; the string type memory layout is
; [ 32bit | 32bit | ... ]
; [  cap  |  len  | cnt ]
puts:
    mov   esi ,  eax                ; get the address of the string itself
    add   esi ,  8

    mov   ebx ,  eax                ; get the length of the string
    add   ebx ,  4
    mov   ebx , [ebx]

    mov   eax ,  ebx                ; make the length multiple of 4 to avoid underflows
    xor   edx ,  edx
    mov   ecx ,  4
    div   ecx
    add   ebx ,  edx

    mov   eax ,  esi
    .prnt:                          ; print by char (4 bytes)
    push eax
    call putc
    pop  eax

    add  eax  ,  4
    sub  ebx  ,  4                  ; minus 4 bytes to print
    cmp  ebx  ,  0
    jne .prnt

    ret

; exit(code)
exit:
    mov   ebx ,  eax                ; sets exit code
    mov   eax ,  01h                ; system call (exit)
    int   80h                       ; calls it