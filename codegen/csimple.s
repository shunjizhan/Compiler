.text

.globl _Main
_Main:
push %ebp
mov %esp, %ebp
sub $0,%esp
#### RETURN
popl %eax
mov %ebp, %esp 
pop %ebp
	ret
