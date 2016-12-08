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
#-- Variable_LHS --#
movl $8, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- IntLit --#
pushl $12345678
popl %eax
mov %eax, -8(%ebp)
#-- Assignment --#
#-- Variable_LHS --#
movl $4, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- AddressOf --#
#-- Variable_LHS --#
movl $8, %eax
subl %ebp, %eax
neg %eax
pushl %eax
popl %eax
mov %eax, -4(%ebp)
#-- Assignment --#
#-- Variable_LHS --#
movl $12, %eax
subl %ebp, %eax
neg %eax
pushl %eax
#-- Deref --#
#-- Ident --#
pushl -4(%ebp)
popl %eax
pushl 0(%eax)
popl %eax
mov %eax, -12(%ebp)
#-- RETURN --#
#-- Ident --#
pushl -12(%ebp)
popl %eax
#------------#

### restore stack position
mov %ebp, %esp 
pop %ebp
	ret
