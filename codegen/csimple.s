.text

#-- ProcImpl --#
_bbb: 
### ready for program
push %ebp
mov %esp, %ebp
sub $12,%esp

#-- DeclImpl --#
#-- DeclImpl --#
#-- Assignment --#
#-- Variable_LHS --#
movl $12, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- IntLit --#
pushl $10
popl %eax
mov %eax, -12(%ebp)
#-- Assignment --#
#-- Variable_LHS --#
movl $8, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- Plus --#
#-- Ident --#
pushl -8(%ebp)
#-- IntLit --#
pushl $10
 popl %ebx
 popl %eax
addl %ebx, %eax
pushl %eax
popl %eax
mov %eax, -8(%ebp)
#-- RETURN --#
#-- Ident --#
pushl -4(%ebp)
popl %eax
#------------#

### restore stack position
mov %ebp, %esp 
pop %ebp
	ret
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
#-- Variable_LHS --#
movl $4, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- Uminus --#
#-- IntLit --#
pushl $5
 popl %eax
 negl %eax
 pushl %eax
popl %eax
mov %eax, -4(%ebp)
#-- Assignment --#
#-- Variable_LHS --#
movl $8, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- Ident --#
pushl -4(%ebp)
popl %eax
mov %eax, -8(%ebp)
#-- Assignment --#
#-- Variable_LHS --#
movl $12, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- BoolLit --#
pushl $1
popl %eax
mov %eax, -12(%ebp)
#-- Assignment --#
#-- Variable_LHS --#
movl $16, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- BoolLit --#
pushl $0
popl %eax
mov %eax, -16(%ebp)
#-- IfWithElse --#
#-- BoolLit --#
pushl $0
popl %eax
movl $1, %ebx
cmp %eax, %ebx
jne next0
#-- Assignment --#
#-- Variable_LHS --#
movl $4, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- IntLit --#
pushl $1
popl %eax
mov %eax, -4(%ebp)
jmp end0
next0:
#-- Assignment --#
#-- Variable_LHS --#
movl $4, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- IntLit --#
pushl $2
popl %eax
mov %eax, -4(%ebp)
end0:
#-- Call --#
#-- Variable_LHS --#
movl $4, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- IntLit --#
pushl $3
call _bbb
movl %eax, -4(%ebp)
#-- Assignment --#
#-- Variable_LHS --#
movl $12, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- Ident --#
pushl -16(%ebp)
popl %eax
mov %eax, -12(%ebp)
#-- Assignment --#
#-- Variable_LHS --#
movl $16, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- BoolLit --#
pushl $1
popl %eax
mov %eax, -16(%ebp)
#-- RETURN --#
#-- Not --#
#-- Not --#
#-- Not --#
#-- Ident --#
pushl -16(%ebp)
 popl %eax
 xor $1,  %eax
 pushl %eax
 popl %eax
 xor $1,  %eax
 pushl %eax
 popl %eax
 xor $1,  %eax
 pushl %eax
popl %eax
#------------#

### restore stack position
mov %ebp, %esp 
pop %ebp
	ret
