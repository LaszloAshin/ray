bits 32

%define ORIGIN 0x08048000

org ORIGIN

ehdr:
	db 0x7F,"ELF"
start.2:
	lea esi, [byte eax + ecx + payload - start.1 - 1] ; 4 ; eax=0x8048014 esi=payload.end-1
	lea edi, [byte esi + edx + (PAYLOAD_ADDR - ORIGIN) - (payload - $$) - PAQ_OFFSET - 1] ; 4 ; edi=PAYLOAD_ADDR + PAYLOAD_SIZE + payload.end - payload
	rep movsb ; 2
	jmp short start.3 ; 2
	dw 2 ; e_type (ET_EXEC)
	dw 3 ; e_machine (EM_386)
start.1:
	std ; e_version byte 1
	mov dl, (PAYLOAD_SIZE + 1 + 1 + PAQ_OFFSET) % 256 ; 2 ; e_version byte 2 & 3
	db 0xb8 ; e_version byte 4: mov eax, ...
	dd start.1 ; e_entry
	dd phdr - $$ ; e_phoff ; sub 0, al; add [eax], al
	mov cx, payload.end - payload ; e_shoff
	mov dh, (PAYLOAD_SIZE + 1 + 1 + PAQ_OFFSET) / 256 ; e_flags byte 1 & 2
	jmp short start.2; e_flags byte 3 & 4
	dw 52 ; e_ehsize
	dw 32 ; e_phentsize
phdr:
	dd 1 ; e_phnum; e_shentsize; p_type = PT_LOAD
	dd 0 ; e_shnum; e_shstrndx; p_offset
	dd $$ ; p_vaddr
start.3:
	inc edi ; p_addr byte 1
	push edi ; p_addr byte 2
	jmp edi; p_addr byte 3 & 4; jump to relocated payload
	dd payload.end - $$ ; p_filesz
	dd 0x100000 ; memsz
;	dd 7 ; p_flags
;	dd 0 ; p_align

payload:
	xchg edx, ecx ; 2 ; p_flags RW
	xchg eax, edx ; 1 ; zero eax
	rep stosb ; 2
	cld ; 1

	pop ebx ; 1
	mov al, compressed - payload + PAQ_OFFSET ; 2
	add ebx, eax ; 2
	lea edi, [byte edi + 1 + PAQ_OFFSET] ; 3
	; onekpaq needs: ebx=src, edi=dest zeroed from start-13 to end+1 (PAQ_OFFSET=13)
%define ONEKPAQ_NO_SECTIONS
%include "onekpaq-prefix/src/onekpaq/onekpaq_decompressor32.asm"
%if PAYLOAD_ENTRY_POINT - PAYLOAD_ADDR
	lea edi, [byte edi + PAYLOAD_ENTRY_POINT - PAYLOAD_ADDR] ; 3
%endif
	jmp edi ; 2
compressed:
	incbin PAQ_FILENAME, 0, PAQ_SIZE
payload.end:
