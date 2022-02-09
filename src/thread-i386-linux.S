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
	pusha
	xor    %ebx, %ebx
	mov    $STACK_SIZE, %ecx
	push   $(PROT_READ + PROT_WRITE)
	pop    %edx
	mov    $(MAP_ANONYMOUS + MAP_PRIVATE + MAP_GROWSDOWN), %esi
	xor    %edi, %edi
	xor    %ebp, %ebp
	xor    %eax, %eax
	mov    $SYS_mmap2, %al
	int    $0x80 # XXX: %eax points to a stack that we don't munmap -> resource leak!
	lea    -44(%eax,%ecx), %ecx
	mov    40(%esp), %eax
	mov    %eax, 40(%ecx)
	mov    36(%esp), %eax
	mov    %eax, 36(%ecx)
	mov    $callit, %eax
	mov    %eax, 32(%ecx)
	mov    $(CLONE_VM + CLONE_FS + CLONE_FILES + CLONE_SIGHAND + CLONE_IO), %ebx
	push   $SYS_clone
	pop    %eax
	int    $0x80
	mov    %eax, 28(%esp)
	popa
	ret

callit:
	pop    %eax
	call   *%eax
	push   $SYS_exit
	pop    %eax
	int    $0x80

