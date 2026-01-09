BITS 16
ORG 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov ax, 0x0000
    mov es, ax
    mov bx, 0x8000

    mov ah, 0x02          ; read sectors
    mov al, 8             ; number of sectors
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00
    mov dl, 0x00          ; floppy
    int 0x13
    jc disk_error

    cli
    lgdt [gdt_desc]
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEL:protected_start

disk_error:
    hlt
    jmp disk_error

BITS 32
protected_start:
    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x8000            ; jump to stage-2 entry

gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
gdt_end:

gdt_desc:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEL equ 0x08
DATA_SEL equ 0x10

times 510 - ($ - $$) db 0
dw 0xAA55
