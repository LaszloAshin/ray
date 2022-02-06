	.text
	.globl create_thread
	.type create_thread,@function

.set STACK_SIZE,    4096 * 1024
.set PROT_READ,     1
.set PROT_WRITE,    2 
.set MAP_PRIVATE,   0x0002
.set MAP_ANONYMOUS, 0x0020
.set MAP_GROWSDOWN, 0x0100
.set CLONE_VM,      0x00000100
.set CLONE_FS,      0x00000200
.set CLONE_FILES,   0x00000400
.set CLONE_SIGHAND, 0x00000800
.set CLONE_IO,      0x80000000
#include <sys/syscall.h>

	.type exit,@function

# x86_64 ABI function params: %rdi %rsi %rdx %rcx %r8 %r9
create_thread:
	push   %rsi # user data arg to thread_func
	push   %rdi # thread_func
	xor    %edi, %edi
	mov    $STACK_SIZE, %esi
	xor    %edx, %edx
	mov    $(PROT_READ + PROT_WRITE), %dl
	mov    $(MAP_ANONYMOUS + MAP_PRIVATE + MAP_GROWSDOWN), %r10d
	xor    %r8d, %r8d
	xor    %r9d, %r9d
	push   $SYS_mmap
	pop    %rax
	syscall # XXX: %rax points to a stack that we don't munmap -> resource leak!
	lea    -24(%rax,%rsi), %rsi
	lea    exit(%rip), %rax
	mov    %rax, 16(%rsi)
	pop    8(%rsi)
	pop    (%rsi)
	push   %rax
	mov    $(CLONE_VM + CLONE_FS + CLONE_FILES + CLONE_SIGHAND + CLONE_IO), %edi
	push   $SYS_clone
	pop    %rax
	syscall
	pop    %rdi
	ret

exit:
	push   $SYS_exit
	pop    %rax
	syscall

