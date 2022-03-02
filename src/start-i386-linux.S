	.text
	.globl _start
	.type _start,@function
	.section .text.exit

#include <sys/syscall.h>

# _start stack:
# argc  %esp + 0
# argv  %esp + 4
# envp  %esp + 4 + argc * 4
_start:
	pop    %edx # argc
	mov    %esp, %ebp # argv
	lea    4(%ebp, %edx, 4), %eax # envp
	push   %eax
	push   %ebp
	push   %edx
	call   main
	mov    %eax, %ebx # exit status
	mov    $SYS_exit, %eax
	int    $0x80
