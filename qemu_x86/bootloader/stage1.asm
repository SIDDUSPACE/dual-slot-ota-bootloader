; ==================================================
; Stage-1 Bootloader (Floppy, QEMU-safe)
; Loads stage2.bin to 0x00100000
; ==================================================

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

    mov ah, 0x0E
    mov al, 'S'
    int 0x10

; -------------------------------
; Reset disk system (MANDATORY)
; -------------------------------
    mov ah, 0x00
    mov dl, 0x00        ; floppy
    int 0x13
    jc disk_error

; -------------------------------
; Load stage2 into 0000:8000
; -------------------------------
    mov ax, 0x0000
    mov es, ax
    mov bx, 0x8000      ; safe buffer

    mov ah, 0x02        ; read sectors
    mov al, 5           ; number of sectors
    mov ch, 0           ; cylinder
    mov cl, 2           ; sector (stage2 starts at sector 2)
    mov dh, 0           ; head
    mov dl, 0x00        ; floppy
    int 0x13
    jc disk_error

; -------------------------------
; Copy to 1MB (0x00100000)
; -------------------------------
    cli
    mov si, 0x8000
    mov di, 0x0000
    mov ax, 0x0010
    mov es, ax
    mov cx, 2560        ; 5 * 512 / 2
    rep movsw

; -------------------------------
; Enter protected mode
; -------------------------------
    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEL:pm_start

; -------------------------------
; Protected mode
; -------------------------------
BITS 32
pm_start:
    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    jmp 0x00100000

; -------------------------------
; Disk error handler
; -------------------------------
disk_error:
    mov si, err_msg
.print:
    lodsb
    or al, al
    jz $
    mov ah, 0x0E
    int 0x10
    jmp .print

err_msg db "Disk read error", 0

; -------------------------------
; GDT
; -------------------------------
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

; -------------------------------
; Boot signature
; -------------------------------
times 510 - ($ - $$) db 0
dw 0xAA55
