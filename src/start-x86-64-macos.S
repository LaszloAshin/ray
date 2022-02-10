	.text
	.globl start

#include <sys/syscall.h>

# _start stack:
# argc  %rsp + 0
# argv  %rsp + 8
# envp  %rsp + 8 + argc * 8
start:
	pop    %rdi # argc
	mov    %rsp, %rsi # argv
	lea    8(%rsi, %rdi, 8), %rdx # envp
	and    $-16, %rsp # align stack
	call   _main # x86_64 ABI function params: %rdi %rsi %rdx %rcx %r8 %r9
	xchg   %eax, %edi # exit status to edi
	mov    $(0x2000000 + SYS_exit), %eax # syscall number
	syscall
