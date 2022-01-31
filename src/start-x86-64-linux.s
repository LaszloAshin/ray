	.text
	.globl _start
	.type _start,@function
	.section .text.exit

# _start stack:
# argc  %rsp + 0
# argv  %rsp + 8
# envp  %rsp + 8 + argc * 8
_start:
	pop    %rdi # argc
	mov    %rsp, %rsi # argv
	lea    8(%rsi, %rdi, 8), %rdx # envp
	and    $-16, %rsp # align stack
	call   main # x86_64 ABI function params: %rdi %rsi %rdx %rcx %r8 %r9
	mov    %eax, %edi # exit status
	xor    %eax, %eax
	mov    $60, %al # syscall number
	syscall
