.text

#-- Main --#
.globl _Main

_Main:
### ready for program
push %ebp
mov %esp, %ebp
sub $0,%esp

#-- TBoolean --#
#-- BoolLit --#
pushl $0
#-- RETURN --#
popl %eax
#------------#

### restore stack position
mov %ebp, %esp 
pop %ebp
	ret
