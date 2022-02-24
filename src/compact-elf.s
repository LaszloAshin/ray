bits 32

org 0x08048000

ehdr:
	db 0x7F,"ELF"
start.1:
	mov esi, payload.end - 1 ; 5
	mov edi, PAYLOAD_ADDR + PAYLOAD_SIZE + payload.end - payload ; 5
	jmp short start.2 ; 2
	dw 2 ; e_type (ET_EXEC)
	dw 3 ; e_machine (EM_386)
start.2:
	std ; e_version byte 1
	nop ; e_version byte 2
	jmp short start.3 ; e_version byte 3 & 4
	dd start.1 ; e_entry
	dd phdr - $$ ; e_phoff
start.3:
	mov cx, payload.end - payload ; e_shoff
	rep movsb ; e_flags byte 1 & 2
	jmp short start.4; e_flags byte 3 & 4
	dw 52 ; e_ehsize
	dw 32 ; e_phentsize
phdr:
	dd 1 ; e_phnum; e_shentsize; p_type = PT_LOAD
	dd 0 ; e_shnum; e_shstrndx; p_offset
	dd $$ ; p_vaddr
start.4:
	inc edi ; p_addr byte 1
	push edi ; p_addr byte 2
	jmp edi; p_addr byte 3 & 4
	dd payload.end - $$ ; p_filesz
	dd PAYLOAD_ADDR - $$ + PAYLOAD_SIZE + payload.end - payload + 1 ; memsz
;	dd 7 ; p_flags
;	dd 0 ; p_align

payload:
	mov cx, PAYLOAD_SIZE + 1 + 1 + PAQ_OFFSET ; 4 ; p_flags byte 1 = RW <- it works without the executable bit set ?!
;	xor al, al ; 2 ; eax is already zero at process start
	rep stosb ; 2
	cld ; 1

	pop ebx ; 1
	lea ebx, [ebx + compressed - payload + PAQ_OFFSET] ; 6
	lea edi, [byte edi + 1 + PAQ_OFFSET] ; 3
	; onekpaq needs: ebx=src, edi=dest zeroed from start-13 to end+1 (PAQ_OFFSET=13)
%define ONEKPAQ_NO_SECTIONS
%include "../src/onekpaq_decompressor32.asm"
%if PAYLOAD_ENTRY_POINT - PAYLOAD_ADDR
	lea edi, [byte edi + PAYLOAD_ENTRY_POINT - PAYLOAD_ADDR] ; 3
%endif
	jmp edi ; 2
compressed:
	incbin PAQ_FILENAME
payload.end:
