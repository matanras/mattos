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

dump:
	push bp
	mov bp, sp
	push bx
	push cx
	push dx
	push si
	
	mov bx, [bp + 4] ; buf
	mov cx, [bp + 6] ; count
_dump_loop:
	test cx, cx
	jz _done
	mov dl, byte [bx]
	xor ax, ax
	mov al, dl
	shr al, 4
	mov si, ASCII_LUT
	add si, ax
	mov al, [si]
	mov ah, 0xe
	int 0x10
	xor ax, ax
	mov al, dl
	and al, 0x0f
	mov si, ASCII_LUT
	add si, ax
	mov al, byte [si]
	mov ah, 0xe
	int 0x10
	mov al, ' '
	int 0x10
	inc bx
	dec cx
	jmp _dump_loop

_done:
	mov ah, 0xe
	mov al, `\r`
	int 0x10
	mov al, `\n`
	int 0x10

	pop si
	pop dx
	pop cx
	pop bx
	pop bp
	ret

ASCII_LUT: db '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'