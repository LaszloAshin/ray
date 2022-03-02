	.text
	.globl create_thread

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

# x86_32 ABI syscall params: %ebx %ecx %edx %esi %edi %ebp
create_thread: # arg1: thread_func; arg2: user data arg to thread_func
	push   %ebp
	mov    %esp, %ebp
	push   %edi
	push   %esi
	push   %ebx
	xor    %ebx, %ebx
	mov    $STACK_SIZE, %ecx
	mov    $(PROT_READ + PROT_WRITE), %edx
	mov    $(MAP_ANONYMOUS + MAP_PRIVATE + MAP_GROWSDOWN), %esi
	mov    $SYS_mmap2, %eax
	int    $0x80 # XXX: %eax points to a stack that we don't munmap -> resource leak!
	lea    -28(%eax,%ecx), %ecx
	lea    8(%ebp), %esi
	lea    20(%ecx), %edi
	movsd
	movsd
	movl   $callit, 16(%ecx)
	mov    $(CLONE_VM + CLONE_FS + CLONE_FILES + CLONE_SIGHAND + CLONE_IO), %ebx
	mov    $SYS_clone, %eax
	int    $0x80
	pop    %ebx
	pop    %esi
	pop    %edi
	pop    %ebp
	ret

callit:
	pop    %eax
	call   *%eax
	mov    $SYS_exit, %eax
	int    $0x80
