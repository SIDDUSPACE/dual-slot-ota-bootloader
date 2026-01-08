BITS 32
GLOBAL _start
EXTERN bootloader_main

SECTION .text

_start:
    cli
    mov esp, 0x90000
    call bootloader_main

.hang:
    hlt
    jmp .hang
