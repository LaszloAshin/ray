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

# x86_32 ABI syscall params: %ebx %ecx %edx %esi %edi %ebp
create_thread: # arg1: thread_func; arg2: user data arg to thread_func
	push   %ebp
	mov    %esp, %ebp
	push   %ebx
	push   %ecx
	push   %esi
	push   %edi
	xor    %ebx, %ebx
	xor    %edx, %edx
	xor    %eax, %eax
	mov    $STACK_SIZE, %ecx
	mov    $(PROT_READ + PROT_WRITE), %dl
	mov    $(MAP_ANONYMOUS + MAP_PRIVATE + MAP_GROWSDOWN), %esi
	xor    %edi, %edi
	push   %ebp
	xor    %ebp, %ebp
	mov    $SYS_mmap2, %al
	int    $0x80 # XXX: %eax points to a stack that we don't munmap -> resource leak!
	pop    %ebp
	pop    %edi
	pop    %esi
	lea    -24(%eax,%ecx), %ecx
	mov    12(%ebp), %eax
	mov    %eax, 20(%ecx)
	mov    8(%ebp), %eax
	mov    %eax, 16(%ecx)
	mov    $callit, %eax
	mov    %eax, 12(%ecx)
	mov    $(CLONE_VM + CLONE_FS + CLONE_FILES + CLONE_SIGHAND + CLONE_IO), %ebx
	xor    %eax, %eax
	mov    $SYS_clone, %al
	int    $0x80
	pop    %ecx
	pop    %ebx
	pop    %ebp
	ret

callit:
	pop    %eax
	call   *%eax
	xor    %eax, %eax
	mov    $SYS_exit, %al
	int    $0x80

