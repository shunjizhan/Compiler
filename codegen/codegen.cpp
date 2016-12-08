#include <cassert>
#include <typeinfo>

#include "ast.hpp"
#include "symtab.hpp"
#include "primitive.hpp"


class Codegen : public Visitor
{
  private:
    FILE* m_outputfile;
    SymTab *m_st;

    // Basic size of a word (integers and booleans) in bytes
    static const int wordsize = 4;

    int label_count; // Access with new_label

    // Helpers
    // This is used to get new unique labels (cleverly names label1, label2, ...)
    int new_label() { return label_count++; }

    void set_text_mode() { fprintf(m_outputfile, ".text\n\n"); }

    void set_data_mode() { fprintf(m_outputfile, ".data\n\n"); }

    // PART 1:
    // 1) get arithmetic expressions on integers working:
    //  you wont really be able to run your code,
    //  but you can visually inspect it to see that the correct
    //  chains of opcodes are being generated.
    // 2) get procedure calls working:
    //  if you want to see at least a very simple program compile
    //  and link successfully against gcc-produced code, you
    //  need to get at least this far
    // 3) get boolean operation working
    //  before we can implement any of the conditional control flow
    //  stuff, we need to have booleans worked out.
    // 4) control flow:
    //  we need a way to have if-elses and while loops in our language.
    // 5) arrays: just like variables, but with an index

    // Hint: the symbol table has been augmented to track an offset
    //  with all of the symbols.  That offset can be used to figure
    //  out where in the activation record you should look for a particuar
    //  variable


    ///////////////////////////////////////////////////////////////////////////////
    //
    //  function_prologue
    //  function_epilogue
    //
    //  Together these two functions implement the callee-side of the calling
    //  convention.  A stack frame has the following layout:
    //
    //                         <- SP (before pre-call / after epilogue)
    //  high -----------------
    //       | actual arg 1  |
    //       |    ...        |
    //       | actual arg n  |
    //       -----------------
    //       |  Return Addr  |
    //       =================
    //       | temporary 1   | <- SP (when starting prologue)
    //       |    ...        |
    //       | temporary n   |
    //   low ----------------- <- SP (when done prologue)
    //
    //
    //              ||
    //              ||
    //             \  /
    //              \/
    //
    //
    //  The caller is responsible for placing the actual arguments
    //  and the return address on the stack. Actually, the return address
    //  is put automatically on the stack as part of the x86 call instruction.
    //
    //  On function entry, the callee
    //
    //  (1) allocates space for the callee's temporaries on the stack
    //
    //  (2) saves callee-saved registers (see below) - including the previous activation record pointer (%ebp)
    //
    //  (3) makes the activation record pointer (frmae pointer - %ebp) point to the start of the temporary region
    //
    //  (4) possibly copies the actual arguments into the temporary variables to allow easier access
    //
    //  On function exit, the callee:
    //
    //  (1) pops the callee's activation record (temporay area) off the stack
    //
    //  (2) restores the callee-saved registers, including the activation record of the caller (%ebp)
    //
    //  (3) jumps to the return address (using the x86 "ret" instruction, this automatically pops the
    //      return address off the stack
    //
    //////////////////////////////////////////////////////////////////////////////
    //
    // Since we are interfacing with code produced by GCC, we have to respect the
    // calling convention that GCC demands:
    //
    // Contract between caller and callee on x86:
    //    * after call instruction:
    //           o %eip points at first instruction of function
    //           o %esp+4 points at first argument
    //           o %esp points at return address
    //    * after ret instruction:
    //           o %eip contains return address
    //           o %esp points at arguments pushed by caller
    //           o called function may have trashed arguments
    //           o %eax contains return value (or trash if function is void)
    //           o %ecx, %edx may be trashed
    //           o %ebp, %ebx, %esi, %edi must contain contents from time of call
    //    * Terminology:
    //           o %eax, %ecx, %edx are "caller save" registers
    //           o %ebp, %ebx, %esi, %edi are "callee save" registers
    ////////////////////////////////////////////////////////////////////////////////


    void emit_prologue(SymName *name, unsigned int size_locals, unsigned int num_args)
    {
    }

    void emit_epilogue() {
        fprintf(m_outputfile, "\n### restore stack position\n");

        fprintf(m_outputfile, "mov %%ebp, %%esp \n");
        fprintf(m_outputfile, "pop %%ebp\n");
        fprintf(m_outputfile, "\tret\n");
    }

  public:

    Codegen(FILE* outputfile, SymTab* st)
    {
        m_outputfile = outputfile;
        m_st = st;
        label_count = 0;
    }

    void visitProgramImpl(ProgramImpl* p) {
        set_text_mode();
        p->visit_children(this);
    }

    void visitProcImpl(ProcImpl* p) {
        char* proc_name = strdup(p->m_symname->spelling());
        if (strcmp(proc_name,"Main") == 0 ) {
            fprintf(m_outputfile, "#-- Main --#\n");
            fprintf(m_outputfile, ".globl _Main\n\n");
            fprintf(m_outputfile, "_Main:\n");
        }
        else{
            fprintf(m_outputfile, "#-- ProcImpl --#\n");
            fprintf(m_outputfile, "_%s: \n", proc_name);
        }

        fprintf(m_outputfile, "### ready for program\n");
        fprintf(m_outputfile, "push %%ebp\n");
        fprintf(m_outputfile, "mov %%esp, %%ebp\n");
        fprintf(m_outputfile, "sub $%i,%%esp\n\n", m_st->scopesize(p->m_procedure_block->m_attribute.m_scope));

        p->visit_children(this);

        emit_epilogue();

        return;
    }

    void visitProcedure_blockImpl(Procedure_blockImpl* p) { p->visit_children(this); }
    void visitNested_blockImpl(Nested_blockImpl* p) { p->visit_children(this); }

    void visitAssignment(Assignment* p) {
        fprintf(m_outputfile, "#-- Assignment --#\n");
        p->visit_children(this);        // get lhs relative position to ebp 

        //    * after call instruction:
        //           o %eip points at first instruction of function
        //           o %esp+4 points at first argument
        //           o %esp points at return address

        Symbol* s = m_st->lookup(p->m_attribute.m_scope, lhs_to_id(p->m_lhs));
        fprintf(m_outputfile, "popl %%eax\n");                      // rhs value
        fprintf(m_outputfile, "mov %%eax, -%d(%%ebp)\n", s->get_offset()+4);    // lhs = rhs value
    }

    void visitCall(Call* p) {
        fprintf(m_outputfile, "#-- Call --#\n");
        p->visit_children(this);
       
        // call the function, the result will be at the top of stack
        fprintf(m_outputfile, "call _%s\n", strdup(p->m_symname->spelling()));  

        //    * after call instruction:
        //           o %eip points at first instruction of function
        //           o %esp+4 points at first argument
        //           o %esp points at return address

        Symbol* s = m_st->lookup(p->m_attribute.m_scope, lhs_to_id(p->m_lhs));

        // save the function call result to corresponsing position
        fprintf(m_outputfile, "movl %%eax, -%d(%%ebp)\n", s->get_offset()+4);
    }

    void visitReturn(Return* p) {
        fprintf(m_outputfile, "#-- RETURN --#\n");
        p->visit_children(this);

        fprintf(m_outputfile, "popl %%eax\n");
        fprintf(m_outputfile, "#------------#\n");
    }

    // Control flow
    void visitIfNoElse(IfNoElse* p) {
        fprintf(m_outputfile, "#-- IfNoElse --#\n");
        p->m_expr->accept(this);
      
       int num = new_label();
       fprintf(m_outputfile, "popl %%eax\n");
       fprintf(m_outputfile, "movl $1, %%ebx\n");

       fprintf(m_outputfile, "cmp %%eax, %%ebx\n");
       fprintf(m_outputfile, "jne next%i\n", num);

       p->m_nested_block->visit_children(this);

       fprintf( m_outputfile, "next%i:\n", num);
    }

    void visitIfWithElse(IfWithElse* p) {
        fprintf(m_outputfile, "#-- IfWithElse --#\n");
        p ->m_expr->accept(this);

        int num = new_label();
        fprintf(m_outputfile, "popl %%eax\n");
        fprintf(m_outputfile, "movl $1, %%ebx\n");

        fprintf(m_outputfile, "cmp %%eax, %%ebx\n");
        fprintf(m_outputfile, "jne next%i\n", num);

        p->m_nested_block_1->visit_children(this);
        fprintf( m_outputfile, "jmp end%i\n", num);

        fprintf( m_outputfile, "next%i:\n", num);
        p->m_nested_block_2->visit_children(this);

        fprintf( m_outputfile, "end%i:\n", num);
    }

    void visitWhileLoop(WhileLoop* p) {
        fprintf(m_outputfile, "#-- WhileLoop --#\n");
        
        int num = new_label();
        int num2 = new_label();

        fprintf(m_outputfile, "while%i:\n", num2);
        p->m_expr->accept(this);

       fprintf(m_outputfile, "popl %%eax\n");
       fprintf(m_outputfile, "movl $1, %%ebx\n");

       fprintf(m_outputfile, "cmp %%eax, %%ebx\n");
       fprintf(m_outputfile, "jne next%i\n", num);

       p->m_nested_block->visit_children(this);
        fprintf( m_outputfile, "jmp while%i\n", num2);

       fprintf( m_outputfile, "next%i:\n", num);
    }

    void visitCodeBlock(CodeBlock *p) {
        fprintf(m_outputfile, "#-- CodeBlock --#\n");
    }

    // Variable declarations (no code generation needed)
    void visitDeclImpl(DeclImpl* p)     { 
        fprintf(m_outputfile, "#-- DeclImpl --#\n"); 
        p -> visit_children(this);
    }

    void visitTInteger(TInteger* p)     {}
    void visitTIntPtr(TIntPtr* p)       {}
    void visitTBoolean(TBoolean* p)     {}
    void visitTCharacter(TCharacter* p) {}
    void visitTCharPtr(TCharPtr* p)     {}
    void visitTString(TString* p)       {}

    // Comparison operations
    void visitCompare(Compare* p) {                     // OK
        fprintf(m_outputfile, "#-- Compare --#\n");
        p -> visit_children(this);

        int num = new_label();
        fprintf(m_outputfile, "popl %%ebx\n");
        fprintf(m_outputfile, "popl %%eax\n");
        fprintf(m_outputfile, "cmp %%ebx,%%eax\n");
        fprintf(m_outputfile, "je yes%d # equal\n", num);      
        fprintf(m_outputfile, "pushl $0\n");
        fprintf(m_outputfile, "jmp next%d\n", num);    
        fprintf(m_outputfile, "yes%d:\n", num);
        fprintf(m_outputfile, "pushl $1\n");   
        fprintf(m_outputfile, "next%d:\n", num); 
    }

    void visitNoteq(Noteq* p) {                         // OK
        fprintf(m_outputfile, "#-- Noteq --#\n");
        p -> visit_children(this);

        int num = new_label();
        fprintf(m_outputfile, "popl %%ebx\n");
        fprintf(m_outputfile, "popl %%eax\n");
        fprintf(m_outputfile, "cmp %%ebx,%%eax\n");
        fprintf(m_outputfile, "jne yes%d # not equal\n", num);      
        fprintf(m_outputfile, "pushl $0\n");
        fprintf(m_outputfile, "jmp next%d\n", num);    
        fprintf(m_outputfile, "yes%d:\n", num);
        fprintf(m_outputfile, "pushl $1\n");   
        fprintf(m_outputfile, "next%d:\n", num); 
    }

    void visitGt(Gt* p) {                               // OK
        fprintf(m_outputfile, "#-- Gt --#\n");
        p -> visit_children(this);

        int num = new_label();
        fprintf(m_outputfile, "popl %%ebx\n");
        fprintf(m_outputfile, "popl %%eax\n");
        fprintf(m_outputfile, "cmp %%ebx,%%eax\n");
        fprintf(m_outputfile, "jg yes%d # greater\n", num);      
        fprintf(m_outputfile, "pushl $0\n");
        fprintf(m_outputfile, "jmp next%d\n", num);    
        fprintf(m_outputfile, "yes%d:\n", num);
        fprintf(m_outputfile, "pushl $1\n");   
        fprintf(m_outputfile, "next%d:\n", num); 
    }

    void visitGteq(Gteq* p) {                           // OK
        fprintf(m_outputfile, "#-- Gteq --#\n");
        p -> visit_children(this);

        int num = new_label();
        fprintf(m_outputfile, "popl %%ebx\n");
        fprintf(m_outputfile, "popl %%eax\n");
        fprintf(m_outputfile, "cmp %%ebx,%%eax\n");
        fprintf(m_outputfile, "jge yes%d # greater or equal\n", num);      
        fprintf(m_outputfile, "pushl $0\n");
        fprintf(m_outputfile, "jmp next%d\n", num);    
        fprintf(m_outputfile, "yes%d:\n", num);
        fprintf(m_outputfile, "pushl $1\n");   
        fprintf(m_outputfile, "next%d:\n", num); 
    }

    void visitLt(Lt* p) {                               // OK
        fprintf(m_outputfile, "#-- Lt --#\n");
        p -> visit_children(this);

        int num = new_label();
        fprintf(m_outputfile, "popl %%ebx\n");
        fprintf(m_outputfile, "popl %%eax\n");
        fprintf(m_outputfile, "cmp %%ebx,%%eax\n");
        fprintf(m_outputfile, "jl yes%d # less than\n", num);      
        fprintf(m_outputfile, "pushl $0\n");
        fprintf(m_outputfile, "jmp next%d\n", num);    
        fprintf(m_outputfile, "yes%d:\n", num);
        fprintf(m_outputfile, "pushl $1\n");   
        fprintf(m_outputfile, "next%d:\n", num);        
    }

    void visitLteq(Lteq* p) {                               // OK
        fprintf(m_outputfile, "#-- Lteq --#\n");
        p -> visit_children(this);

        int num = new_label();
        fprintf(m_outputfile, "popl %%ebx\n");
        fprintf(m_outputfile, "popl %%eax\n");
        fprintf(m_outputfile, "cmp %%ebx,%%eax\n");
        fprintf(m_outputfile, "jle yes%d # less or equal\n", num);      
        fprintf(m_outputfile, "pushl $0\n");
        fprintf(m_outputfile, "jmp next%d\n", num);    
        fprintf(m_outputfile, "yes%d:\n", num);
        fprintf(m_outputfile, "pushl $1\n");   
        fprintf(m_outputfile, "next%d:\n", num);
    }

    // Arithmetic and logic operations
    void visitAnd(And* p) {                             // OK                
        fprintf(m_outputfile, "#-- And --#\n");
        p -> visit_children(this);

        fprintf(m_outputfile, " popl %%ebx\n");
        fprintf(m_outputfile, " popl %%eax\n");
        fprintf(m_outputfile, " andl %%ebx, %%eax\n");
        fprintf(m_outputfile, " pushl %%eax\n");
    }

    void visitOr(Or* p) {                             // OK                   
        fprintf(m_outputfile, "#-- Or --#\n");
        p -> visit_children(this);

        pop_ebx_eax();
        fprintf(m_outputfile, " orl %%ebx, %%eax\n");
        fprintf(m_outputfile, " pushl %%eax\n");
    }

    void visitMinus(Minus* p) {                      // OK
        fprintf(m_outputfile, "#-- Minus --#\n");
        p -> visit_children(this);

        pop_ebx_eax();
        fprintf(m_outputfile, " subl %%ebx, %%eax\n");
        fprintf(m_outputfile, " pushl %%eax\n");        
    }

    void visitPlus(Plus* p) {                            // OK
        fprintf(m_outputfile, "#-- Plus --#\n");
        p -> visit_children(this);

        pop_ebx_eax();
        fprintf(m_outputfile, "addl %%ebx, %%eax\n");
        fprintf(m_outputfile, "pushl %%eax\n");        
    }

    void visitTimes(Times* p) {                         // OK
        fprintf(m_outputfile, "#-- Times --#\n");
        p -> visit_children(this);

        pop_ebx_eax();
        fprintf(m_outputfile, " imull %%ebx, %%eax\n");
        fprintf(m_outputfile, " pushl %%eax\n");
    }

    void visitDiv(Div* p) {                             // OK
        fprintf(m_outputfile, "#-- Div --#\n");
        p -> visit_children(this);

        pop_ebx_eax();
        fprintf(m_outputfile, " cdq\n");               // OK   
        fprintf(m_outputfile, " idivl %%ebx\n");
        fprintf(m_outputfile, " pushl %%eax\n");
    }

    void visitNot(Not* p) {                           // OK               
        fprintf(m_outputfile, "#-- Not --#\n");
        p -> visit_children(this);

        fprintf(m_outputfile, " popl %%eax\n");
        fprintf(m_outputfile, " xor $1,  %%eax\n");          
        fprintf(m_outputfile, " pushl %%eax\n");
    }

    void visitUminus(Uminus* p) {                           // OK
        fprintf(m_outputfile, "#-- Uminus --#\n");
        p -> visit_children(this);

        fprintf(m_outputfile, " popl %%eax\n");
        fprintf(m_outputfile, " negl %%eax\n");
        fprintf(m_outputfile, " pushl %%eax\n");
    }

    // Variable and constant access
    void visitIdent(Ident* p) {
        fprintf(m_outputfile, "#-- Ident --#\n");
        p -> visit_children(this);
    
        Symbol* s = m_st->lookup(p->m_attribute.m_scope, strdup(p->m_symname->spelling()));
        fprintf(m_outputfile, "pushl -%d(%%ebp)\n", s->get_offset()+4);
    }


    void visitArrayAccess(ArrayAccess* p) {
        fprintf(m_outputfile, "#-- ArrayAccess --#\n");
        p->m_expr->accept(this);

        Symbol* s = m_st->lookup(p->m_symname->spelling());
        int offset = s->get_offset() + 4;

        fprintf(m_outputfile, "popl %%eax");                // index
        fprintf(m_outputfile, "shl, $2, %%eax");            // real address
        fprintf(m_outputfile, "addl $%d, %%eax", offset);   // position
        fprintf(m_outputfile, "neg, %%eax");
        fprintf(m_outputfile, "addl %%ebp, %%eax");         // element address
        fprintf(m_outputfile, "pushl 0(%%eax)");            // get element

    }

    void visitCharLit(CharLit* p) {
        fprintf(m_outputfile, "#-- CharLit --#\n");
        fprintf(m_outputfile, "pushl $%d\n", p->m_primitive->m_data);
    }

    void visitNullLit(NullLit* p) {     
        fprintf(m_outputfile, "#-- NullLit --#\n");
        fprintf(m_outputfile, "pushl $0\n");        
    }    

    void visitBoolLit(BoolLit* p) {     // OK
        fprintf(m_outputfile, "#-- BoolLit --#\n");
        fprintf(m_outputfile, "pushl $%d\n", p->m_primitive->m_data);
    }

    void visitIntLit(IntLit* p) {    // OK
        fprintf(m_outputfile, "#-- IntLit --#\n");
        fprintf(m_outputfile, "pushl $%d\n", p->m_primitive->m_data);
    }





    // LHS
    void visitVariable(Variable* p) {                   // push the address the the variable 
          fprintf(m_outputfile, "#-- Variable --#\n");
        //  fprintf(m_outputfile, "%s", p->m_symname->spelling());

        // char* symbol_name = (char*)lhs_to_id(p);
        // printf("%s\n", symbol_name);
        Symbol* symbol = m_st->lookup(p->m_attribute.m_scope, strdup(p->m_symname->spelling()));
        // printf("symbol: %p\n", symbol);

        int offset = symbol->get_offset() + 4;
        fprintf(m_outputfile, "movl $%d, %%eax\n", offset);       // offset
        fprintf(m_outputfile, "subl %%ebp, %%eax\n");             // relative position
        fprintf(m_outputfile, "neg %%eax\n");                 
        fprintf(m_outputfile, "pushl %%eax\n");                   
    }

    void visitDerefVariable(DerefVariable* p) {
        fprintf(m_outputfile, "#-- DerefVariable --#\n");


        //  fprintf(m_outputfile, "%s", p->m_symname->spelling());

        // char* symbol_name = (char*)lhs_to_id(p);
        // printf("%s\n", symbol_name);
        // printf("symbol: %p\n", symbol);

        // Symbol* symbol = m_st->lookup(p->m_attribute.m_scope, strdup(p->m_symname->spelling()));
        // int offset = symbol->get_offset() + 4;
        // fprintf(m_outputfile, "movl $%d, %%eax\n", offset);       // offset
        // fprintf(m_outputfile, "subl %%ebp, %%eax\n");             // relative position
        // fprintf(m_outputfile, "neg %%eax\n");                 
        // fprintf(m_outputfile, "pushl %%eax\n");   

        Symbol* s = m_st->lookup(p->m_attribute.m_scope, strdup(p->m_symname->spelling()));
        fprintf(m_outputfile, "pushl -%d(%%ebp)\n", s->get_offset() + 4);


        // int offset = m_st->lookup(p->m_symname->spelling())->get_offset() + 4;
        // fprintf(m_outputfile, "pushl -%d(%%ebp)", offset);      // get the value at that position directly
    }

    void visitArrayElement(ArrayElement* p) {
        fprintf(m_outputfile, "#-- ArrayElement --#\n");

        p->m_expr->accept(this);

        Symbol* s = m_st->lookup(p->m_symname->spelling());
        int offset = s->get_offset() + 4;

        fprintf(m_outputfile, "popl %%eax");                // index
        fprintf(m_outputfile, "shl, $2, %%eax");            // real address
        fprintf(m_outputfile, "addl $%d, %%eax", offset);   // position
        fprintf(m_outputfile, "neg, %%eax");
        fprintf(m_outputfile, "addl %%ebp, %%eax");         // element address
        fprintf(m_outputfile, "pushl 0(%%eax)");            // get element
    }



    // Special cases
    void visitSymName(SymName* p) {}
    void visitPrimitive(Primitive* p) { fprintf(m_outputfile, "#-- StringPrimitive --#\n"); }



    // Strings
    void visitStringAssignment(StringAssignment* p) {
        fprintf(m_outputfile, "#-- StringAssignment --#\n");
    }

    void visitStringPrimitive(StringPrimitive* p) {
        fprintf(m_outputfile, "#-- StringPrimitive --#\n");
    }

    void visitAbsoluteValue(AbsoluteValue* p) {         
        fprintf(m_outputfile, "#-- AbsoluteValue --#\n");
        p -> visit_children(this);

        fprintf( m_outputfile, " popl %%eax\n");
        fprintf( m_outputfile, " cdq\n");
        fprintf( m_outputfile, " xorl %%edx, %%eax\n");
        fprintf( m_outputfile, " subl %%edx, %%eax\n");
        fprintf( m_outputfile, " pushl %%eax\n");
    }

    // Pointer
    void visitAddressOf(AddressOf* p) {             // OK
        fprintf(m_outputfile, "#-- AddressOf --#\n");
        p->visit_children(this);                    // visit the variable and push the address directly
    }

    void visitDeref(Deref* p) {                     // OK
        fprintf(m_outputfile, "#-- Deref --#\n");
        p->visit_children(this);                    // get the variable address

        fprintf( m_outputfile, "popl %%eax\n");       // pop address
        fprintf( m_outputfile, "pushl 0(%%eax)\n");   // push the value
    }

    const char* lhs_to_id(Lhs* lhs) {
        Variable* v = dynamic_cast<Variable*>(lhs);
        if(v) {
            return v->m_symname->spelling();
        }

        DerefVariable* dv = dynamic_cast<DerefVariable*>(lhs);
        if(dv) {
            return dv->m_symname->spelling();
        }

        ArrayElement* ae = dynamic_cast<ArrayElement*>(lhs);
        if(ae) {
            return ae->m_symname->spelling();
        }

        return nullptr;
    }    

    void pop_ebx_eax() {
        fprintf(m_outputfile, " popl %%ebx\n");
        fprintf(m_outputfile, " popl %%eax\n");
    }


};


void dopass_codegen(Program_ptr ast, SymTab* st)
{
    Codegen* codegen = new Codegen(stdout, st);
    ast->accept(codegen);
    delete codegen;
}
