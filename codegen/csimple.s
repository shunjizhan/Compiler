.text

### Main
.globl _Main
_Main:
push %ebp
mov %esp, %ebp
sub $0,%esp
#-- SymName --#
#-- TInteger --#
#-- IntLit --#
pushl $5
#-- RETURN --#
popl %eax
#------------#
mov %ebp, %esp 
pop %ebp
	ret
