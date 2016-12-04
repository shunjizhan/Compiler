.text

#-- Main --#
.globl _Main

_Main:
### ready for program
push %ebp
mov %esp, %ebp
sub $16,%esp

#-- DeclImpl --#
#-- DeclImpl --#
#-- Assignment --#
#-- Variable --#
#-- Uminus --#
#-- Uminus --#
#-- IntLit --#
pushl $2
 popl %eax
 negl %eax
 pushl %eax
 popl %eax
 negl %eax
 pushl %eax
popl %eax
mov %eax, -4(%ebp)
#-- Assignment --#
#-- Variable --#
#-- Uminus --#
#-- Ident --#
pushl -4(%ebp)
 popl %eax
 negl %eax
 pushl %eax
popl %eax
mov %eax, -8(%ebp)
#-- Assignment --#
#-- Variable --#
#-- Or --#
#-- And --#
#-- BoolLit --#
pushl $1
#-- BoolLit --#
pushl $0
 popl %ebx
 popl %eax
 andl %ebx, %eax
 pushl %eax
#-- BoolLit --#
pushl $1
 popl %ebx
 popl %eax
 orl %ebx, %eax
 pushl %eax
popl %eax
mov %eax, -12(%ebp)
#-- Assignment --#
#-- Variable --#
#-- Not --#
#-- BoolLit --#
pushl $0
 popl %eax
 not  %eax
 pushl %eax
popl %eax
mov %eax, -16(%ebp)
#-- Uminus --#
#-- Uminus --#
#-- Uminus --#
#-- Uminus --#
#-- Uminus --#
#-- Uminus --#
#-- Uminus --#
#-- Uminus --#
#-- Ident --#
pushl -4(%ebp)
 popl %eax
 negl %eax
 pushl %eax
 popl %eax
 negl %eax
 pushl %eax
 popl %eax
 negl %eax
 pushl %eax
 popl %eax
 negl %eax
 pushl %eax
 popl %eax
 negl %eax
 pushl %eax
 popl %eax
 negl %eax
 pushl %eax
 popl %eax
 negl %eax
 pushl %eax
 popl %eax
 negl %eax
 pushl %eax
#-- RETURN --#
popl %eax
#------------#

### restore stack position
mov %ebp, %esp 
pop %ebp
	ret
