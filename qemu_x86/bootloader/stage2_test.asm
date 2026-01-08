BITS 32
ORG 0x8000

start:
    ; Clear screen (80x25)
    mov edi, 0xB8000
    mov ecx, 80*25
    mov ax, 0x0720        ; space, gray
.clear:
    mov [edi], ax
    add edi, 2
    loop .clear

    ; Print OK at row 10
    mov edi, 0xB8000 + (10*160)
    mov eax, 0x0F4B0F4F   ; OK
    mov [edi], eax

.hang:
    jmp .hang

times (1536 - ($ - $$)) db 0
