; usage:
; your code must have defined a variable in .bss
; segment as `resb 4` to be able to use this lib
; ==============================================
; your must include this file bellow the segment
; named `.text`
; ==============================================
; input order (max 6 arguments):
; EDI ESI EDX ECX EBX EAX
; ==============================================
; output order (max 6 return values):
; EAX EBX ECX EDX ESI EDI

; getc returns 4 byte char read from the input stream
; returns word char in EAX
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


; exit(code)
exit:
    mov   ebx ,  eax                ; sets exit code
    mov   eax ,  01h                ; system call (exit)
    int   80h                       ; calls it