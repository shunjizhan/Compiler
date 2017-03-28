# Introduction
This is a compiler written in C++.  

It compiles a language called [CSimple](http://www.cs.ucsb.edu/~chris/teaching/cs160/projects/language.html) into 32-bit x86 machine code.

# Run
    cd codegen/
    make csimple
    ./csimple < test > csimple.s
    gcc -c -m32 -o csimple.o csimple.s
    gcc -c -m32 -o start.o start.c
    gcc -m32 -o start start.o csimple.o
    ./start

# Specification
In the compiling process, it will perform 5 actions in order: 

CSimple   
--> scan to tokens by scanner    
--> syntax check by LR parser  
--> build abstract syntax tree (AST)    
--> type check and semantic analysis    
--> generate x86 code

# Tools
lex (flex) and yacc (bison)

# Author
Shunji Zhan