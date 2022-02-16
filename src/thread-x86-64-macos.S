	.text

#include <sys/syscall.h>

bsdthread_start:
	push   %rdi # "pthread" which we use for passing the write end of the pipe

	mov    %rcx, %rdi # "arg"
	call   *%rdx # "func"

	pop    %rdi
	mov    $(0x2000000 | SYS_close), %eax
	syscall

	xor    %edi, %edi
	mov    $(0x2000000 | SYS_bsdthread_terminate), %eax
	syscall

.globl _threads_init
_threads_init:
	lea    bsdthread_start(%rip), %rdi
	xor    %esi, %esi
	xor    %edx, %edx
	xor    %r10d, %r10d
	xor    %r8d, %r8d
	xor    %r9d, %r9d
	mov    $(0x2000000 | SYS_bsdthread_register), %eax
	syscall
	ret

#define PROT_READ       0x01
#define PROT_WRITE      0x02
#define MAP_PRIVATE     0x0002
#define MAP_ANON        0x1000
#define PTHREAD_START_CUSTOM 0x01000000

.globl _create_thread
_create_thread:
	push   %rsi
	push   %rdi

	xor    %edi, %edi # address
	mov    $(4096 * 1024), %esi # length
	mov    $(PROT_READ | PROT_WRITE), %edx
	mov    $(MAP_ANON | MAP_PRIVATE), %r10d
	xor    %r8d, %r8d # fd
	xor    %r9d, %r9d # offset
	mov    $(0x2000000 | SYS_mmap), %eax
	syscall
	lea    -8(%rax, %rsi), %rax
	push   %rax

	mov    $(0x2000000 | SYS_pipe), %eax
	syscall

	mov    %edx, %r10d
	pop    %rdx
	pop    %rdi
	pop    %rsi
	mov    $(PTHREAD_START_CUSTOM), %r8d
	push   %rax
	mov    $(0x2000000 | SYS_bsdthread_create), %eax
	syscall

	pop    %rax
	ret

.globl _join_thread
_join_thread:
	lea    -8(%rsp), %rsi
	push   $1
	pop    %rdx
	mov    $(0x2000000 | SYS_read), %rax
	syscall
	ret
