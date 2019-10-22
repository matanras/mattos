[org 0x7c00]
	xor ax, ax
	mov ds, ax
	mov es, ax
	cli
	mov ss, ax
    mov sp, 0x8000
    sti
    mov bp, sp ; set the stack safely away from us

_disk_load:
    mov ah, 0x02 ; disk read code
    mov al, 1   ; read 1 sector
    mov ch, 0x00 ; cylinder 0
    ; dl <- drive number. Our caller sets it as a parameter and gets it from BIOS
    ; (0 = floppy, 1 = floppy2, 0x80 = hdd, 0x81 = hdd2)
    mov dh, 0x00 ; head 0
    mov cl, 0x02 ; start from sector 2
    mov bx, KERNEL_OFFSET
    int 0x13
    jc _disk_error ; if error (stored in the carry bit)
    push BOOTLOADER_BANNER
    call print_string

enter_protected_mode:
	cli ; disable irqs
	lgdt [GDT_DESCRIPTOR] ; load gdt
	mov eax, cr0
	or eax, 1
	mov cr0, eax ; turn on protected mode
	jmp CODE_SEG:_init_pm ; far jump to protected mode.
						 ; the far jump causes the CPU to flush its cache and pipeline
						 ; which may consist of invalid real-mode instructions
_disk_error:
	push DISK_ERROR
	call print_string
	add sp, 2
	jmp $


[bits 32]
_init_pm:
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax

	mov ebp, 0x9000
	mov esp, ebp

	push PROTECTED_MODE_MSG
	call print_string_pm
	add esp, 4
	call KERNEL_OFFSET
	jmp $

%include "string.asm"


gdt_start:
gdt_null:
	; first segment descriptor is null
	dd 0x0
	dd 0x0

gdt_code:
	dw 0xffff ; bits 0:15 of limit
	dw 0x0 ; bits 0:15 of base
	db 0x0 ; bits 16:23 of base
	db 10011010b ; present, ring 0, not trap, code, non-conforming, readable, not accessed
	db 11001111b ; granularity(1), 32 bit, not 64 bit, no AVL, bits 16:19 of limit
	db 0x0 ; bits 24:31 of base

gdt_data:
	dw 0xffff ; bits 0:15 of limit
	dw 0x0 ; bits 0:15 of base
	db 0x0 ; bits 16:23 of base
	db 10010010b ; present, ring 0, not trap, data, no expand-down, writable, not accessed
	db 11001111b ; granularity(1), 32 bit, not 64 bit, no AVL, bits 16:19 of limit
	db 0x0 ; bits 24:31 of base
gdt_end:

GDT_DESCRIPTOR:
	dw gdt_end - gdt_start - 1 ; always less than 1 of the true size
	dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

KERNEL_OFFSET equ 0x1000
BOOTLOADER_BANNER: db "Mattos bootloader.", 0
PROTECTED_MODE_MSG: db "Switched to protected mode, loading kernel...", 0
DISK_ERROR: db "Disk error", 0

; Magic number
times 510 - ($-$$) db 0
dw 0xaa55

; boot sector = sector 1 of cyl 0 of head 0 of hdd 0
; from now on = sector 2 ...
; dw 0xbbbb ; sector 2 = 512 bytes