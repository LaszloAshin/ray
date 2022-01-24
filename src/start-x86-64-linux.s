	.text
	.globl _start
	.type _start,@function

# _start stack:
# argc  %rsp + 0
# argv  %rsp + 8
# envp  %rsp + 8 + argc * 8
_start:
	pop    %rdi # argc
	mov    %rsp, %rsi # argv
	and    $-16, %rsp # align stack
	callq  main # x86_64 ABI function params: %rdi %rsi %rdx %rcx %r8 %r9
	mov    %rax, %rdi # exit status
	xor    %rax, %rax
	mov    $60, %al # syscall number
	syscall
