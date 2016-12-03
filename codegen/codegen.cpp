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

    void emit_epilogue()
    {
    }

  // WRITEME: more functions to emit code

  public:

    Codegen(FILE* outputfile, SymTab* st)
    {
        m_outputfile = outputfile;
        m_st = st;
        label_count = 0;
    }

    void visitProgramImpl(ProgramImpl* p) {
        set_text_mode();
        p -> visit_children(this);
    }

    void visitProcImpl(ProcImpl* p) {
        char *name = strdup(p -> m_symname -> spelling());
        if ( strcmp(name,"Main") == 0 ) {
            //inMain(true);
            fprintf( m_outputfile, ".globl _Main\n");
            fprintf( m_outputfile, "_Main:\n");
            fprintf(m_outputfile, "push %%ebp\n");
            fprintf(m_outputfile, "mov %%esp, %%ebp\n");
            //sub esp, 4 Make room for 4-byte local variables
            fprintf(m_outputfile,"sub $%i,%%esp\n",m_st->scopesize(p->m_procedure_block->m_attribute.m_scope));

            p->visit_children(this);
            fprintf(m_outputfile, "mov %%ebp, %%esp \n");
            fprintf(m_outputfile, "pop %%ebp\n");
            fprintf( m_outputfile, "\tret\n");
            //inMain(false);
            return;
        }
        else{
            //prologue
            fprintf(m_outputfile, "####INSIDE FUNCTION\n");
            fprintf(m_outputfile, "_%s: \n",  name);
            fprintf(m_outputfile, "push %%ebp\n");
            fprintf(m_outputfile, "mov %%esp, %%ebp\n");
            //sub esp, 4   ; Make room for one 4-byte local variable.
            fprintf(m_outputfile,"sub $%i,%%esp\n",m_st->scopesize(p->m_procedure_block->m_attribute.m_scope));
    //        fprintf(m_outputfile, "pop %%ebx\n");
    //        list<Param_ptr>::iterator iter;
    //        for (iter = p->m_param_list->begin(); iter != p->m_param_list->end(); iter++) {
    //            Symbol *s=m_st->lookup((*iter)->m_attribute.m_scope,strdup((*iter)->m_symname->spelling()));
    //            int offset=4+s->get_offset();
    //            fprintf(m_outputfile, "popl %%eax\n");
    //            fprintf(m_outputfile, "mov %%eax, -%d(%%ebp)\n", offset);
    //        }
    //        fprintf(m_outputfile, "push %%ebx\n");
            p->visit_children(this);
            //epilogue
            fprintf(m_outputfile, "mov %%ebp, %%esp \n");
            fprintf(m_outputfile, "pop %%ebp\n");
            fprintf(m_outputfile, "\tret\n\n"); //could return size of local vars
            return;
        }
    }

    void visitProcedure_blockImpl(Procedure_blockImpl* p) {
        p->visit_children(this);
    }

    void visitNested_blockImpl(Nested_blockImpl* p) {
        p->visit_children(this);
    }

    void visitAssignment(Assignment* p)
    {
    }

    void visitCall(Call* p)
    {
    }

    void visitReturn(Return* p) {
        p -> visit_children(this);
        fprintf( m_outputfile, "#### RETURN\n");
        fprintf( m_outputfile, "popl %%eax\n");
    }

    // Control flow
    void visitIfNoElse(IfNoElse* p)
    {
    }

    void visitIfWithElse(IfWithElse* p)
    {
    }

    void visitWhileLoop(WhileLoop* p)
    {
    }

    void visitCodeBlock(CodeBlock *p) 
    {
    }

    // Variable declarations (no code generation needed)
    void visitDeclImpl(DeclImpl* p)
    {
    }

    void visitTInteger(TInteger* p)
    {
    }

    void visitTIntPtr(TIntPtr* p)
    {
    }

    void visitTBoolean(TBoolean* p)
    {
    }

    void visitTCharacter(TCharacter* p)
    {
    }

    void visitTCharPtr(TCharPtr* p)
    {
    }

    void visitTString(TString* p)
    {
    }

    // Comparison operations
    void visitCompare(Compare* p)
    {
    }

    void visitNoteq(Noteq* p)
    {
    }

    void visitGt(Gt* p)
    {
    }

    void visitGteq(Gteq* p)
    {
    }

    void visitLt(Lt* p)
    {
    }

    void visitLteq(Lteq* p)
    {
    }

    // Arithmetic and logic operations
    void visitAnd(And* p)
    {
    }

    void visitOr(Or* p)
    {
    }

    void visitMinus(Minus* p)
    {
    }

    void visitPlus(Plus* p)
    {
    }

    void visitTimes(Times* p)
    {
    }

    void visitDiv(Div* p)
    {
    }

    void visitNot(Not* p)
    {
    }

    void visitUminus(Uminus* p)
    {
    }

    // Variable and constant access
    void visitIdent(Ident* p)
    {
    }

    void visitBoolLit(BoolLit* p)
    {
    }

    void visitCharLit(CharLit* p)
    {
    }

    void visitIntLit(IntLit* p)
    {
    }

    void visitNullLit(NullLit* p)
    {
    }

    void visitArrayAccess(ArrayAccess* p)
    {
    }

    // LHS
    void visitVariable(Variable* p)
    {
    }

    void visitDerefVariable(DerefVariable* p)
    {
    }

    void visitArrayElement(ArrayElement* p)
    {
    }

    // Special cases
    void visitSymName(SymName* p)
    {
    }

    void visitPrimitive(Primitive* p)
    {
    }

    // Strings
    void visitStringAssignment(StringAssignment* p)
    {
    }

    void visitStringPrimitive(StringPrimitive* p)
    {
    }

    void visitAbsoluteValue(AbsoluteValue* p)
    {
    }

    // Pointer
    void visitAddressOf(AddressOf* p)
    {
    }

    void visitDeref(Deref* p)
    {
    }
};


void dopass_codegen(Program_ptr ast, SymTab* st)
{
    Codegen* codegen = new Codegen(stdout, st);
    ast->accept(codegen);
    delete codegen;
}
