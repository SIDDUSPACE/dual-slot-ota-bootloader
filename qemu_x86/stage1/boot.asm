BITS 16
ORG 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl
    xor ax, ax
    mov es, ax
    mov bx, 0x8000

    mov ah, 0x02
    mov al, 4          
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13

    jc $

    jmp 0x0000:0x8000

boot_drive db 0
times 510-($-$$) db 0
dw 0xAA55
