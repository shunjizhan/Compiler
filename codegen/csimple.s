.text

#-- Main --#
.globl _Main

_Main:
### ready for program
push %ebp
mov %esp, %ebp
sub $8,%esp

#-- DeclImpl --#
#-- DeclImpl --#
#-- Assignment --#
#-- Variable --#
#-- IntLit --#
pushl $0
popl %eax
mov %eax, -4(%ebp)
#-- Assignment --#
#-- Variable --#
#-- BoolLit --#
pushl $1
popl %eax
mov %eax, -8(%ebp)
#-- IfWithElse --#
#-- Ident --#
pushl -4(%ebp)
#-- IntLit --#
pushl $1
popl %eax
movl $1, %ebx
cmp %eax, %ebx
jne next0
#-- Assignment --#
#-- Variable --#
#-- IntLit --#
pushl $1
popl %eax
mov %eax, -4(%ebp)
jmp end0
next0:
#-- Assignment --#
#-- Variable --#
#-- IntLit --#
pushl $0
popl %eax
mov %eax, -4(%ebp)
end0:
#-- RETURN --#
#-- Ident --#
pushl -4(%ebp)
popl %eax
#------------#

### restore stack position
mov %ebp, %esp 
pop %ebp
	ret
