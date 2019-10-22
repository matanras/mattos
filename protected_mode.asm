[bits 16]

enter_protected_mode:
	cli ; disable irqs
	lgdt [gdt_descriptor] ; load gdt
	mov eax, cr0
	or eax, 1
	mov cr0, eax ; turn on protected mode
	jmp CODE_SEG:_init_pm ; far jump to protected mode.
						 ; the far jump causes the CPU to flush its cache and pipeline
						 ; which may consist of invalid real-mode instructions

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
	jmp BEGIN_PM

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

gdt_descriptor:
	dw gdt_end - gdt_start - 1 ; always less than 1 of the true size
	dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start