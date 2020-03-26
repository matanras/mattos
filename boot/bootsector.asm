[section .bootsector.text]
[bits 16]
extern __kernel_num_sectors__
extern __kernel_code_start__

global start
start:
    mov [boot_drive], dl
    xor ax, ax
    mov ds, ax
    mov es, ax
    cli
    mov ss, ax
    mov sp, 0x8000
    sti
    mov bp, sp ; set the stack safely away from us

    push BOOTLOADER_BANNER
    call print_string
    add sp, 2

    mov ah, 0x41
    mov dl, [boot_drive]
    mov bx, 0x55aa
    int 0x13
    jc _lba_unsupported
_disk_load:
    mov ah, 0x42 ; EDD read
    mov dl, [boot_drive]
    mov si, DAP
    int 0x13
    jc _disk_error
    push DISK_SUCCESS
    call print_string
    add sp, 2
_enter_protected_mode:
    cli ; disable irqs
    lgdt [GDT_DESCRIPTOR] ; load gdt
    mov eax, cr0
    or eax, 1
    mov cr0, eax ; turn on protected mode
    jmp CODE_SEG:_init_pm ; far jump to protected mode.
                         ; the far jump causes the CPU to flush its cache and pipeline
                         ; which may consist of invalid real-mode instructions

_lba_unsupported:
    push LBA_UNSUPPORTED
    call print_string
    add sp, 2
    jmp $
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
    call __kernel_code_start__
    jmp $

%include "string.asm"

section .bootsector.data

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

DAP:
dap_size: db 0x10
dap_padding: db 0
dap_num_sectors: dw __kernel_num_sectors__
dap_buffer_offset: dw __kernel_code_start__
dap_buffer_segment: dw 0
dap_lower_lba_dword: dd 1
dap_upper_lba_dword: dd 0

boot_drive db 0
BOOTLOADER_BANNER db `Mattos bootloader.\r\n`, 0
LBA_UNSUPPORTED db `LBA addressing mode unsupported.\r\n`, 0
DISK_ERROR db `Disk error.\r\n`, 0
DISK_SUCCESS db `Read kernel from disk.\r\n`, 0
