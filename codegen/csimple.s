.text

#-- ProcImpl --#
_aaa: 
### ready for program
push %ebp
mov %esp, %ebp
sub $0,%esp

#-- RETURN --#
#-- IntLit --#
pushl $10
popl %eax
#------------#

### restore stack position
mov %ebp, %esp 
pop %ebp
	ret
#-- ProcImpl --#
_bbb: 
### ready for program
push %ebp
mov %esp, %ebp
sub $4,%esp

#-- DeclImpl --#
#-- Call --#
#-- Variable --#
movl $4, %eax
subl %ebp, %eax
neg %eax
pushl %eax
call _aaa
movl %eax, -4(%ebp)
#-- Assignment --#
#-- Variable --#
movl $4, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- Div --#
#-- Ident --#
pushl -4(%ebp)
#-- IntLit --#
pushl $3
 popl %ebx
 popl %eax
 cdq
 idivl %ebx
 pushl %eax
popl %eax
mov %eax, -4(%ebp)
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
sub $12,%esp

#-- DeclImpl --#
#-- Assignment --#
#-- Variable --#
movl $4, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- IntLit --#
pushl $0
popl %eax
mov %eax, -4(%ebp)
#-- Assignment --#
#-- Variable --#
movl $8, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- IntLit --#
pushl $1
popl %eax
mov %eax, -8(%ebp)
#-- Assignment --#
#-- Variable --#
movl $12, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- IntLit --#
pushl $2
popl %eax
mov %eax, -12(%ebp)
#-- WhileLoop --#
while1:
#-- Lt --#
#-- Ident --#
pushl -8(%ebp)
#-- IntLit --#
pushl $10
popl %ebx
popl %eax
cmp %ebx,%eax
jl yes2 # less than
pushl $0
jmp next2
yes2:
pushl $1
next2:
popl %eax
movl $1, %ebx
cmp %eax, %ebx
jne next0
#-- Assignment --#
#-- Variable --#
movl $8, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- Plus --#
#-- Ident --#
pushl -8(%ebp)
#-- IntLit --#
pushl $1
 popl %ebx
 popl %eax
addl %ebx, %eax
pushl %eax
popl %eax
mov %eax, -8(%ebp)
jmp while1
next0:
#-- Call --#
#-- Variable --#
movl $12, %eax
subl %ebp, %eax
neg %eax
pushl %eax
call _bbb
movl %eax, -12(%ebp)
#-- Assignment --#
#-- Variable --#
movl $8, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- Div --#
#-- Ident --#
pushl -12(%ebp)
#-- IntLit --#
pushl $10
 popl %ebx
 popl %eax
 cdq
 idivl %ebx
 pushl %eax
popl %eax
mov %eax, -8(%ebp)
#-- RETURN --#
#-- Ident --#
pushl -12(%ebp)
popl %eax
#------------#

### restore stack position
mov %ebp, %esp 
pop %ebp
	ret
