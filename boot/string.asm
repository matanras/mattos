[bits 16]
print_string:
	push bp
	mov bp, sp
	mov bx, [bp + 4]
	mov ah, 0xe
_print_string_loop:
	mov al, [bx]
	test al, al
	jz _print_string_done
_print_char:
	int 0x10
	inc bx
	jmp _print_string_loop
_print_string_done:
	pop bp
	ret

[bits 32]
print_string_pm:
	push ebp
	mov ebp, esp
	push eax
	push ebx
	push edx

	mov ebx, [ebp + 8]
	mov edx, VGA_BUFFER
	mov ah, WHITE_ON_BLACK
_print_string_pm_loop:
	mov al, [ebx]
	test al, al
	jz _print_string_pm_done
	mov [edx], al
	mov [edx + 1], ah
	inc ebx
	add edx, 2
	jmp _print_string_pm_loop

_print_string_pm_done:
	pop edx
	pop ebx
	pop eax
	pop ebp
	ret

VGA_BUFFER equ 0xb8000
WHITE_ON_BLACK equ 0x0f