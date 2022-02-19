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

bits 64

org 0x100000000

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
..@code:
	rep   movsb
	jmp   rbx
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
	dq    payload.end - main_header + PAYLOAD_ADDR - main_header ; vmsize
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
	dq    0, PAYLOAD_ENTRY_POINT, payload.end - payload, 0 ; rax, rbx, rcx, rdx
	dq    PAYLOAD_ADDR + payload.end - payload - 1, payload.end - 1, 0, 0 ; rdi, rsi, rbp, rsp(must be zero)
	dq    0, 0, 0, 0                                       ; r8, r9, r10, r11
	dq    0, 0, 0, 0                                       ; r12, r13, r14, r15
	dq    ..@code, 0x400;, 0, 0, 0                         ; rip, rflags(DF), cs, fs, gs
.end:

commands.end:

payload:
	incbin PAYLOAD_FILENAME
.end:
