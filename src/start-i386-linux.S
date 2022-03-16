	.text
	.globl _start
	.type _start,@function
	.section .text.exit
	.extern environ

#include <sys/syscall.h>

# _start stack:
# argc  %esp + 0
# argv  %esp + 4
# envp  %esp + 4 + argc * 4
_start:
	pop    %eax # argc
	mov    %esp, %edx # argv
	lea    4(%edx, %eax, 4), %ecx # envp
	mov    %ecx, environ
	call   main
	mov    %eax, %ebx # exit status
	mov    $SYS_exit, %eax
	int    $0x80
