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

%ifidn ONEKPAQ_DECOMPRESSOR_MODE,1
%elifidn ONEKPAQ_DECOMPRESSOR_MODE,2
%define ONEKPAQ_DECOMPRESSOR_MULTI_SECTION 1
%elifidn ONEKPAQ_DECOMPRESSOR_MODE,3
%define ONEKPAQ_DECOMPRESSOR_FAST 1
%elifidn ONEKPAQ_DECOMPRESSOR_MODE,4
%define ONEKPAQ_DECOMPRESSOR_FAST 1
%define ONEKPAQ_DECOMPRESSOR_MULTI_SECTION 1
%else
%error "ONEKPAQ_DECOMPRESSOR_MODE is not valid (1 - 4)"
%endif

bits 64

org 0x10000

main_header:
	dd    MH_MAGIC_64                              ; magic
	dd    CPU_TYPE_X86_64                          ; cputype
	dd    CPU_SUBTYPE_LIB64 | CPU_SUBTYPE_I386_ALL ; cpusubtype
	dd    MH_EXECUTE                               ; filetype
	dd    3                                        ; ncmds
	dd    commands.end  - commands +3*8            ; sizeofcmds
	dd    MH_NOUNDEFS                              ; flags
	dd    0                                        ; reserved

commands:

pagezero:
	dd    LC_SEGMENT_64           ; cmd
	dd    pagezero.end - pagezero ; command size
.name:	db    '__PAGEZERO', 0
	times 16 - $ + .name db 0     ; segment name (pad to 16 bytes)
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
.name:	db    '__TEXT', 0               ; segment name (pad to 16 bytes)
	times 16 - $ + .name db 0
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
	dd    unixthread.end - unixthread + 3*8                ; cmdsize
	dd    x86_THREAD_STATE64                               ; flavor
	dd    x86_EXCEPTION_STATE64_COUNT                      ; count
	dq    0, 0, 0, 0 ; rax, rbx, rcx, rdx
	dq    0, 0, 0, 0 ; rdi, rsi, rbp, rsp(must be zero)
	dq    0, 0, 0, 0                                       ; r8, r9, r10, r11
	dq    0, 0, 0, 0                                       ; r12, r13, r14, r15
	dq    decompressor, 0;, 0, 0, 0                         ; rip, rflags(DF), cs, fs, gs
.end:

commands.end:

decompressor:
	mov edi, PAYLOAD_ADDR
	mov ebx, compressed + PAQ_OFFSET

onekpaq_decompressor:
	lea esi,[byte rdi-(9+4)]	; rsi=dest, rdi=window start
	lodsd
	inc eax
	mov ecx,eax

%ifdef ONEKPAQ_DECOMPRESSOR_MULTI_SECTION
	lea edx,[byte rbx+1]		; header=src-3 (src has -4 offset)
%else
	lea edx,[byte rbx+3]		; header=src-1 (src has -4 offset)
%endif
	; ebp unitialized, will be cleaned by following loop + first decode
	; which will result into 0 bit, before actual data

.normalize_loop:
	shl byte [byte rbx+4],1
	jnz short .src_byte_has_data
	inc ebx
	rcl byte [byte rbx+4],1		; CF==1
.src_byte_has_data:
	rcl ebp,1

.block_loop:
	; loop level 1
	; eax range
	; rbx src
	; ecx dest bit shift
	; rdx header
	; rsi dest
	; rdi window start
	; ebp value
.byte_loop:
.bit_loop:
	; loop level 2
	; eax range
	; rbx src
	; ecx dest bit shift
	; rdx header
	; rsi dest
	; rdi window start
	; ebp value
.normalize_start:
	add eax,eax
	jns short .normalize_loop

	; for subrange calculation
	fld1
	; p = 1
	fld1

	push rax
	push rcx
	push rdx

	mov al, 00h
.context_loop:
	; loop level 3
	; al 0
	; eax negative
	; rbx src
	; cl dest bit shift
 	; ch model
	; rdx header
	; rsi dest
	; rdi window start
	; ebp value
	; st0 p
	; [rsp] ad

	mov ch,[rdx]

	push rax
	push rcx
	push rdx
	push rdi
	cdq
	mov [rbx],edx			; c0 = c1 = -1

%ifdef ONEKPAQ_DECOMPRESSOR_FAST
	movq xmm0,[rsi]			; SSE
%endif

.model_loop:
	; loop level 4
	; al 0
	; [rbx] c1
	; cl dest bit shift
	; ch model
	; edx c0
	; rsi dest
	; rdi window start
	; st0 p
	; [rsp] ad
	; [rsp+32] ad

%ifdef ONEKPAQ_DECOMPRESSOR_FAST
	movq xmm1,[rdi]			; SSE
	pcmpeqb xmm1,xmm0		; SSE
	pmovmskb eax,xmm1		; SSE
	or al,ch
	inc ax
	jnz short .match_no_hit

	mov al,[byte rsi+8]
	rol al,cl
	xor al,[byte rdi+8]
	shr eax,cl
	jnz short .match_no_hit
%else
	; deepest stack usage 24+32+32 bytes = 88 bytes
	push rax
	push rcx
	push rsi
	push rdi

.match_byte_loop:
	; loop level 5
	cmpsb
	rcr ch,1			; ror would work as well
	ja short .match_mask_miss	; CF==0 && ZF==0
	add al,0x60			; any odd multiplier of 0x20 works
	jnz short .match_byte_loop

	lodsb
	rol al,cl
	xor al,[rdi]
	shr al,cl			; undefined CF when cl=8, still works though
					; To make this conform to Intel spec
					; add 'xor eax,eax' after 'pushad'
					; and replace 'shr al,cl' with 'shr eax,cl'
					; -> +2 bytes
.match_mask_miss:
	pop rdi
	pop rsi
	pop rcx
	pop rax
	jnz short .match_no_hit
%endif
	; modify c1 and c0
	dec edx
	dec dword [rbx]

	jc short .match_bit_set
	sar edx,1
%ifndef ONEKPAQ_DECOMPRESSOR_FAST
.match_no_hit:
%endif
	db 0xc0				; rcl cl,0x3b -> nop (0x3b&31=3*9)
.match_bit_set:
	sar dword [rbx],1

;	DEBUG "Model+bit: %hx, new weights %d/%d",ecx,dword [ebx],edx
%ifdef ONEKPAQ_DECOMPRESSOR_FAST
.match_no_hit:
%endif
	inc edi

	; matching done
	cmp edi,esi
%ifdef ONEKPAQ_DECOMPRESSOR_MULTI_SECTION
	ja short .model_early_start
	jnz short .model_loop
%else
	; will do underflow matching with zeros...
	; not ideal if data starts with lots of ones.
	; Usally impact is 1 or 2 bytes, thus mildly
	; better than +2 bytes of code
	jc short .model_loop
%endif
	; scale the probabilities before loading them to FPU
	; p *= c1/c0 =>  p = c1/(c0/p)
.weight_upload_loop:
.shift:	equ $+2
	rol dword [rbx],byte ONEKPAQ_DECOMPRESSOR_SHIFT
	fidivr dword [rbx]
	mov [rbx],edx

%ifdef ONEKPAQ_DECOMPRESSOR_FAST
	neg ecx
	js short .weight_upload_loop
%else
	dec eax
	jp short .weight_upload_loop
%endif

.model_early_start:
	pop rdi
	pop rdx
	pop rcx
	pop rax

.context_reload:
	dec edx
	cmp ch,[rdx]
	jc short .context_next
	fsqrt
	jbe short .context_reload

.context_next:
	cmp al,[rdx]
	jnz short .context_loop

	pop rdx
	pop rcx
	pop rax

	; restore range
	shr eax,1

	; subrange = range/(p+1)
	faddp st1
	mov [rbx],eax
	fidivr dword [rbx]
	fistp dword [rbx]

	; Arith decode
	sub eax,[rbx]
	cmp ebp,eax
%ifdef ONEKPAQ_DECOMPRESSOR_MULTI_SECTION
	jc .dest_bit_is_set;short .dest_bit_is_set
%else
	jbe .dest_bit_is_set;short .dest_bit_is_set
	inc eax
%endif
	sub ebp,eax
	mov eax,[rbx]
;	uncommenting the next command would make the single-section decompressor "correct"
;	i.e. under %ifndef ONEKPAQ_DECOMPRESSOR_MULTI_SECTION
;	does not seem to be a practical problem though
	;dec eax
.dest_bit_is_set:
	rcl byte [byte rsi+8],1

%ifndef ONEKPAQ_DECOMPRESSOR_MULTI_SECTION
	; preserves ZF when it matters i.e. on a non-byte boundary ...
	loop .no_full_byte
	inc esi
	mov cl,8
.no_full_byte:
	jnz .bit_loop;short .bit_loop

%else
.block_loop_trampoline:
	dec cl
	jnz .bit_loop
;	loop .bit_loop
	inc esi

	dec word [byte rdx+1]
	jnz .new_byte;short .new_byte

	; next header
.skip_header_loop:
	dec edx
	cmp ch,[rdx]
	jnz .skip_header_loop;short .skip_header_loop
	lea edx,[byte rdx-3]
	cmp cx,[byte rdx+1]
	lea edi,[byte rsi+8]
.new_byte:
	mov cl,9
	jnz .block_loop_trampoline;short .block_loop_trampoline
%endif

%if PAYLOAD_ENTRY_POINT - PAYLOAD_ADDR
	lea edi, [byte rdi + PAYLOAD_ENTRY_POINT - PAYLOAD_ADDR] ; 3
%endif
	jmp rdi ; 2
compressed:
	incbin PAQ_FILENAME, 0, PAQ_SIZE
payload.end:
