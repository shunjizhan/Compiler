.text

#-- Main --#
.globl _Main

_Main:
### ready for program
push %ebp
mov %esp, %ebp
sub $12,%esp

#-- DeclImpl --#
#-- DeclImpl --#
#-- Assignment --#
#-- Variable --#
#-- Uminus --#
#-- IntLit --#
pushl $5
 popl %eax
 negl %eax
 pushl %eax
popl %eax
mov %eax, -4(%ebp)
#-- Assignment --#
#-- Variable --#
#-- Ident --#
pushl -4(%ebp)
popl %eax
mov %eax, -8(%ebp)
#-- Assignment --#
#-- Variable --#
#-- BoolLit --#
pushl $1
popl %eax
mov %eax, -12(%ebp)
#-- IfWithElse --#
#-- Gteq --#
#-- Ident --#
pushl -4(%ebp)
#-- Uminus --#
#-- IntLit --#
pushl $5
 popl %eax
 negl %eax
 pushl %eax
popl %ebx
popl %eax
cmp %ebx,%eax
jge yes0 # greater or equal
pushl $0
jmp next0
yes0:
pushl $1
next0:
popl %eax
movl $1, %ebx
cmp %eax, %ebx
jne next1
#-- Assignment --#
#-- Variable --#
#-- IntLit --#
pushl $1
popl %eax
mov %eax, -4(%ebp)
jmp end1
next1:
#-- Assignment --#
#-- Variable --#
#-- IntLit --#
pushl $2
popl %eax
mov %eax, -4(%ebp)
end1:
#-- RETURN --#
#-- Ident --#
pushl -4(%ebp)
popl %eax
#------------#

### restore stack position
mov %ebp, %esp 
pop %ebp
	ret
