	.text
	.globl _start
	.type _start,@function
	.section .text.exit

#include <sys/syscall.h>

# _start stack:
# argc  %esp + 0
# argv  %esp + 8
# envp  %esp + 8 + argc * 8
_start:
	xor    %ebp, %ebp
	pop    %esi # argc
	mov    %esp, %ecx # argv
	lea    4(%ecx, %esi, 4), %edx # envp
	and    $-16, %esp # align stack
	push   %edx
	push   %ecx
	push   %esi
	call   main
	mov    %eax, %edi # exit status
	push   $SYS_exit
	pop    %eax
	int    $0x80
