%define MH_MAGIC_64                 0xfeedfacf
%define CPU_ARCH_ABI64              0x01000000
%define CPU_TYPE_I386               0x00000007
%define CPU_TYPE_X86_64             CPU_ARCH_ABI64 | CPU_TYPE_I386
%define CPU_SUBTYPE_LIB64           0x80000000
%define CPU_SUBTYPE_I386_ALL        0x00000003
%define MH_EXECUTE                  0x2
%define MH_NOUNDEFS                 0x1
%define LC_SEGMENT_64               0x19
%define LC_UNIXTHREAD               0x5
%define VM_PROT_READ                0x1
%define VM_PROT_WRITE               0x2
%define VM_PROT_EXECUTE             0x4
%define VM_PROT_RX                  VM_PROT_READ | VM_PROT_EXECUTE
%define VM_PROT_RWX                 VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE
%define x86_THREAD_STATE64          0x4
%define x86_EXCEPTION_STATE64_COUNT 42

%ifnidn ONEKPAQ_DECOMPRESSOR_MODE,3
%error "Sorry, this oneKpaq version is hand tuned for mode 3"
%endif

bits 64

org 0x10000

main_header:
	dd    MH_MAGIC_64                              ; magic
	dd    CPU_TYPE_X86_64                          ; cputype
	dd    CPU_SUBTYPE_LIB64 | CPU_SUBTYPE_I386_ALL ; cpusubtype
	dd    MH_EXECUTE                               ; filetype
	dd    3                                        ; ncmds
	dd    commands.end  - commands                 ; sizeofcmds
	dd    MH_NOUNDEFS                              ; flags
	dd    0                                        ; reserved

commands:

pagezero:
	dd    LC_SEGMENT_64           ; cmd
	dd    pagezero.end - pagezero ; command size
..@decomp.1: ; segment name (pad to 16 bytes)
	mov edi, PAYLOAD_ADDR ; 5
	mov ebx, compressed + PAQ_OFFSET ; 5
	lea esi, [byte rdi - (9+4)] ; 3 ; rsi=dest, rdi=window start
	lodsd ; 1
	jmp ..@decomp.2 ; 2
	dq    0                       ; vmaddr
	dq    main_header             ; vmsize
	dq    0                       ; fileoff
	dq    0                       ; filesize
	dd    0                       ; maxprot
	dd    0                       ; initprot
	dd    0                       ; nsects
	dd    0                       ; flags
.end:

text:
	dd    LC_SEGMENT_64             ; cmd
	dd    text.end - text           ; command size
..@decomp.2: ; segment name (pad to 16 bytes)
	inc eax ; 2
	mov ecx, eax ; 2
	lea edx, [byte rbx + 3] ; 3 ; header=src-1 (src has -4 offset)
..@normalize_loop:
	shl byte [byte rbx + 4], 1 ; 3
	jnz short ..@src_byte_has_data ; 2
	inc ebx ; 2
	jmp ..@decomp.3 ; 2
	dq    main_header               ; vmaddr
	dq    0x100000 ; vmsize
	dq    0                         ; fileoff
	dq    payload.end - main_header ; filesize
	dd    VM_PROT_RWX               ; maxprot
	dd    VM_PROT_RWX               ; initprot
	dd    0                         ; nsects
	dd    0                         ; flags
.end:

unixthread:
	dd    LC_UNIXTHREAD                                    ; cmd
	dd    unixthread.end - unixthread                      ; cmdsize
	dd    x86_THREAD_STATE64                               ; flavor
	dd    x86_EXCEPTION_STATE64_COUNT                      ; count
..@decomp.3: ; rax, rbx, rcx, rdx, rdi, rsi, rbp
	rcl byte [byte rbx + 4], 1 ; 3 ; CF==1
..@src_byte_has_data:
	rcl ebp, 1 ; 2
..@bit_loop:
	add eax, eax ; 2
	jns short ..@normalize_loop ; 2
	fld1 ; 2
	fld1 ; 2

	push rax ; 1
	push rcx ; 1
	push rdx ; 1

	mov al, 00h ; 2
..@context_loop:
	mov ch, [rdx] ; 2

	push rax ; 1
	push rcx ; 1
	push rdx ; 1
	push rdi ; 1
	cdq ; 1
	mov [rbx], edx ; 2 ; c0 = c1 = -1

	movq xmm0, [rsi] ; 4 ; SSE

..@model_loop:
	movq xmm1, [rdi] ; 4 ; SSE
	pcmpeqb xmm1, xmm0 ; 4 ; SSE
	pmovmskb eax, xmm1 ; 4 ; SSE
	or al, ch ; 2
	inc ax ; 3
	jnz short ..@match_no_hit ; 2
	mov al, [byte rsi+8] ; 3

	nop
	jmp ..@decomp.4 ; 2
	dq 0 ; rsp(must be zero)
..@decomp.4: ; r8, r9, r10, r11, r12, r13, r14, r15
	rol al, cl ; 2
	xor al, [byte rdi+8] ; 3
	shr eax, cl ; 2
	jnz short ..@match_no_hit ; 2
	dec edx ; 2
	dec dword [rbx] ; 2

	jc short .match_bit_set ; 2
	sar edx, 1 ; 2
	db 0xc0				; rcl cl,0x3b -> nop (0x3b&31=3*9)
.match_bit_set:
	sar dword [rbx], 1 ; 2

..@match_no_hit:
	inc edi ; 2
	cmp edi, esi ; 2
	jc short ..@model_loop ; 2
.weight_upload_loop:
	rol dword [rbx],byte ONEKPAQ_DECOMPRESSOR_SHIFT ; 3
	fidivr dword [rbx] ; 2
	mov [rbx],edx ; 2

	neg ecx ; 2
	js short .weight_upload_loop ; 2

	pop rdi ; 1
	pop rdx ; 1
	pop rcx ; 1
	pop rax ; 1

.context_reload:
	dec edx ; 2
	cmp ch,[rdx] ; 2
	jc short .context_next ; 2
	fsqrt ; 2
	jbe short .context_reload ; 2

.context_next:
	cmp al,[rdx] ; 2
	jnz short ..@context_loop ; 2

	pop rdx ; 1
	pop rcx ; 1
	pop rax ; 1

	shr eax,1 ; 2

	faddp st1 ; 2
	jmp ..@decomp.5 ; 2
	dq ..@decomp.1; rip
	db 0, 0 ; at least the two lower bytes of rflags needs to be zero
.end:	equ $ + 6 + 3*8 ; rflags remaining bytes, cs, fs, gs

commands.end: equ $ + 6 + 3*8

..@decomp.5:
	mov [rbx], eax ; 2
	fidivr dword [rbx] ; 2
	fistp dword [rbx] ; 2

	; Arith decode
	sub eax, [rbx] ; 2
	cmp ebp, eax ; 2
	jbe .dest_bit_is_set;short .dest_bit_is_set ; 2
	inc eax ; 2
	sub ebp, eax ; 2
	mov eax, [rbx] ; 2
.dest_bit_is_set:
	rcl byte [byte rsi+8], 1 ; 3
	loop .no_full_byte ; 2
	inc esi ; 2
	mov cl,8 ; 2
.no_full_byte:
	jnz ..@bit_loop;short .bit_loop ; 6

%if PAYLOAD_ENTRY_POINT - PAYLOAD_ADDR
	lea edi, [byte rdi + PAYLOAD_ENTRY_POINT - PAYLOAD_ADDR] ; 3
%endif
	jmp rdi ; 2
compressed:
	incbin PAQ_FILENAME, 0, PAQ_SIZE
payload.end:
%if $ - $$ < 4096
	times (4096 - ($ - $$)) db 0
%endif
