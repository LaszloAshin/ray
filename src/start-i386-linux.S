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
	pop    %edi # argc
	mov    %esp, %esi # argv
	lea    4(%esp, %edi, 4), %ebp # envp
	pusha
	call   main
	xchg   %eax, %edi # exit status
	push   $SYS_exit
	pop    %eax
	int    $0x80
