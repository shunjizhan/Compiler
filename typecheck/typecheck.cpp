#include <iostream>
#include <cstdio>
#include <cstring>

#include "ast.hpp"
#include "symtab.hpp"
#include "primitive.hpp"
#include "assert.h"

using namespace std;

#define default_rule(X) \
    (X)->m_attribute.m_scope = m_st->get_scope(); \
    (X)->visit_children(this); \

#define iterate(iter, list) \
    for(iter = list->begin(); iter != list->end(); iter++) \

#include <typeinfo>

class Typecheck : public Visitor
{
  private:
    FILE* m_errorfile;
    SymTab* m_st;

    // The set of recognized errors
    enum errortype
    {
        no_main,           // OK
        nonvoid_main,
        dup_proc_name,
        dup_var_name,
        proc_undef,
        call_type_mismatch,
        narg_mismatch,
        expr_type_err,
        var_undef,          // OK
        ifpred_err,         // OK
        whilepred_err,      // OK
        incompat_assign,
        who_knows,
        ret_type_mismatch,    // OK
        array_index_error,
        no_array_var,
        arg_type_mismatch,
        expr_pointer_arithmetic_err,
        expr_abs_error,
        expr_addressof_error,
        invalid_deref       // OK
    };

    // Print the error to file and exit
    void t_error(errortype e, Attribute a)
    {
        fprintf(m_errorfile,"on line number %d, ", a.lineno);

        switch(e)
        {
            case no_main:
                fprintf(m_errorfile, "error: no main\n");
                exit(2);
            case nonvoid_main:
                fprintf(m_errorfile, "error: the Main procedure has arguments\n");
                exit(3);
            case dup_proc_name:
                fprintf(m_errorfile, "error: duplicate procedure names in same scope\n");
                exit(4);
            case dup_var_name:
                fprintf(m_errorfile, "error: duplicate variable names in same scope\n");
                exit(5);
            case proc_undef:
                fprintf(m_errorfile, "error: call to undefined procedure\n");
                exit(6);
            case var_undef:
                fprintf(m_errorfile, "error: undefined variable\n");
                exit(7);
            case narg_mismatch:
                fprintf(m_errorfile, "error: procedure call has different number of args than declartion\n");
                exit(8);
            case arg_type_mismatch:
                fprintf(m_errorfile, "error: argument type mismatch\n");
                exit(9);
            case ret_type_mismatch:
                fprintf(m_errorfile, "error: type mismatch in return statement\n");
                exit(10);
            case call_type_mismatch:
                fprintf(m_errorfile, "error: type mismatch in procedure call args\n");
                exit(11);
            case ifpred_err:
                fprintf(m_errorfile, "error: predicate of if statement is not boolean\n");
                exit(12);
            case whilepred_err:
                fprintf(m_errorfile, "error: predicate of while statement is not boolean\n");
                exit(13);
            case array_index_error:
                fprintf(m_errorfile, "error: array index not integer\n");
                exit(14);
            case no_array_var:
                fprintf(m_errorfile, "error: attempt to index non-array variable\n");
                exit(15);
            case incompat_assign:
                fprintf(m_errorfile, "error: type of expr and var do not match in assignment\n");
                exit(16);
            case expr_type_err:
                fprintf(m_errorfile, "error: incompatible types used in expression\n");
                exit(17);
            case expr_abs_error:
                fprintf(m_errorfile, "error: absolute value can only be applied to integers and strings\n");
                exit(17);
            case expr_pointer_arithmetic_err:
                fprintf(m_errorfile, "error: invalid pointer arithmetic\n");
                exit(18);
            case expr_addressof_error:
                fprintf(m_errorfile, "error: AddressOf can only be applied to integers, chars, and indexed strings\n");
                exit(19);
            case invalid_deref:
                fprintf(m_errorfile, "error: Deref can only be applied to integer pointers and char pointers\n");
                exit(20);
            default:
                fprintf(m_errorfile, "error: no good reason\n");
                exit(21);
        }
    }




    // Create a symbol for the procedure and check there is none already
    // existing
    void add_proc_symbol(ProcImpl* p) { 
        cout << "add_proc_symbol" << endl;
        char* name;
        Symbol* s;

        name = strdup(p->m_symname->spelling());
        s = new Symbol();
        s->m_basetype = bt_procedure;   
        s->m_return_type = p->m_type->m_attribute.m_basetype;

        // add arguments to args list
        // DeclImpl_ptr* dec = dynamic_cast<DeclImpl_ptr*>(Decl_ptr);
        list<Decl_ptr>::iterator iter;
        iterate(iter, p->m_decl_list) {
             cout << dynamic_cast<DeclImpl*>(*iter)->m_attribute.basetype() << endl;
            s->m_arg_type.push_back(dynamic_cast<DeclImpl*>(*iter)->m_attribute.m_basetype);
        }

        if(!m_st->insert_in_parent_scope(name, s)) {
            this->t_error(dup_proc_name, p->m_attribute);        
        }
    }

    // Add symbol table information for all the declarations following
    void add_decl_symbol(DeclImpl* p) {
          cout << "add_decl_symbol ";
        list<SymName_ptr>::iterator iter;
        char* name;
        Symbol* s;

        iterate(iter, p->m_symname_list) {
            cout << (*iter)->spelling() <<  ' ' << p->m_type->m_attribute.basetype() << endl;
            name = strdup((*iter)->spelling());
            s = new Symbol();
            s->m_basetype = p->m_type->m_attribute.m_basetype;

            if(!m_st->insert(name, s)) {
                this->t_error(dup_var_name, p->m_attribute);        
            }
        }

    }

    // Check that there is one and only one main
    void check_for_one_main(ProgramImpl* p) {
        Symbol *s = m_st->lookup("Main");
        if(s == NULL)  // check one Main
            this->t_error(no_main, p->m_attribute);

        if(s->m_arg_type.size() > 0) // check no arguments
            this->t_error(nonvoid_main, p->m_attribute);
    }

    // Check that the return statement of a procedure has the appropriate type
    void check_return(ProcImpl *p) {
        cout << "check_return -- " << p->m_type->m_attribute.basetype() << " " << p->m_procedure_block->m_attribute.basetype() << endl;
        if(p->m_type->m_attribute.m_basetype != p->m_procedure_block->m_attribute.m_basetype) {
            this->t_error(ret_type_mismatch, p->m_attribute);    
        }
    }

    // Create a symbol for the procedure and check there is none already
    // existing
    void check_call(Call *p)
    {
    }


    void check_assignment(Assignment* p) {
        Symbol* lhs = m_st->lookup(lhs_to_id(p->m_lhs));
        if (lhs == NULL) {
            this -> t_error(var_undef, p -> m_attribute);
        }

        cout << "check_assignment -- " ;
         cout << p->m_expr->m_attribute.basetype() << " ";
         cout << p->m_lhs->m_attribute.basetype() << endl; 

        // check type match of two sides
        // if( p->m_expr->m_attribute.m_basetype != p->m_lhs->m_attribute.m_basetype) {
        //     this-> t_error(incompat_assign, p->m_attribute);
        // }
    }

    void check_string_assignment(StringAssignment* p)
    {
    }

    void check_array_access(ArrayAccess* p) {
        Symbol* s = m_st->lookup(p->m_symname->spelling());
        if (s == NULL) {
            this->t_error(var_undef, p -> m_attribute);
        }
        cout << "check_array_access -- " << s->basetype() << endl;

        Basetype bt = s->m_basetype;
        if(bt != bt_intptr && bt != bt_string) {
            this->t_error(incompat_assign, p->m_attribute);
        }
        if(p->m_expr->m_attribute.m_basetype != bt_integer) {
            this->t_error(array_index_error, p->m_attribute);
        }
        
        if(bt == bt_intptr) { p->m_attribute.m_basetype = bt_integer; }
        else { p->m_attribute.m_basetype = bt_char;}
    }

    void check_array_element(ArrayElement* p)
    {
    }

    // For checking boolean operations(and, or ...)
    void checkset_boolexpr(Expr* parent, Expr* child1, Expr* child2)
    {
    }

    // For checking arithmetic expressions(plus, times, ...)
    void checkset_arithexpr(Expr* parent, Expr* child1, Expr* child2)
    {
    }

    // Called by plus and minus: in these cases we allow pointer arithmetics
    void checkset_arithexpr_or_pointer(Expr* parent, Expr* child1, Expr* child2)
    {
    }

    // For checking relational(less than , greater than, ...)
    void checkset_relationalexpr(Expr* parent, Expr* child1, Expr* child2)
    {
    }

    // For checking equality ops(equal, not equal)
    void checkset_equalityexpr(Expr* parent, Expr* child1, Expr* child2)
    {
    }

    // For checking not
    void checkset_not(Expr* parent, Expr* child)
    {
    }

    // For checking unary minus
    void checkset_uminus(Expr* parent, Expr* child)
    {
    }

    void checkset_absolute_value(Expr* parent, Expr* child)
    {
    }

    void checkset_addressof(Expr* parent, Lhs* child)
    {
    }

    void checkset_deref_expr(Deref* parent,Expr* child)
    {
    }

    // Check that if the right-hand side is an lhs, such as in case of
    // addressof
    void checkset_deref_lhs(DerefVariable* p)
    {
    }



  public:

    Typecheck(FILE* errorfile, SymTab* st) {
        m_errorfile = errorfile;
        m_st = st;
    }

    void visitProgramImpl(ProgramImpl* p) {
        default_rule(p);
        check_for_one_main(p);
    }

    void visitProcImpl(ProcImpl* p) {
        m_st->open_scope();

        default_rule(p);
        add_proc_symbol(p);   // add precedure to sym_table
        check_return(p);    // check return type

        m_st->close_scope();
    }

    void visitNested_blockImpl(Nested_blockImpl* p) {
        m_st->open_scope();
        default_rule(p);
        m_st->close_scope();
    }

    void visitProcedure_blockImpl(Procedure_blockImpl* p) {
        default_rule(p);
        p->m_attribute.m_basetype = p->m_return_stat->m_attribute.m_basetype;
    }

    void visitDeclImpl(DeclImpl* p) {
        default_rule(p);
        add_decl_symbol(p);
    }

    void visitAssignment(Assignment* p) {
        default_rule(p);   
        check_assignment(p); 
        p->m_attribute.m_basetype = p->m_expr->m_attribute.m_basetype;
    }

    void visitCall(Call* p) {
        default_rule(p); 
        cout << "Call" << endl;

        Symbol* s = m_st->lookup(lhs_to_id(p->m_lhs));
        if (s == NULL) {
            this -> t_error(var_undef, p -> m_attribute);
        }

        // check the call name is indeed a call
        Symbol* f = m_st -> lookup(p->m_symname->spelling());
        if (f == NULL) {
            this->t_error(proc_undef, p->m_attribute);
        }
        if (f->m_basetype != bt_procedure) {
            this->t_error(call_type_mismatch, p->m_attribute);
        }

        // check procedure arguments
        f->print_args();

        list<Expr_ptr>::iterator iter;
        int count = 0;
        iterate(iter, p->m_expr_list) {
            if (count >= f->m_arg_type.size()) {
                this->t_error(narg_mismatch, p->m_attribute);
            }

            cout << (*iter)->m_attribute.basetype() << endl;
            // cout << f->m_arg_type[count++] << endl;
            if ((*iter)->m_attribute.m_basetype != f->m_arg_type[count++]) {
                this->t_error(arg_type_mismatch, p->m_attribute);
            }
        }
        if (count != f -> m_arg_type.size())
            this->t_error(narg_mismatch, p->m_attribute);

        // check precedure return type
        if (f->m_return_type != s->m_basetype)
            this->t_error(incompat_assign, p->m_attribute);
        
        p->m_attribute.m_basetype = s->m_basetype;
    }


    void visitStringAssignment(StringAssignment *p)
    {
    }

    void visitIdent(Ident* p) {
        cout << "Ident " << p->m_symname->spelling() << endl;
        default_rule(p);    
        Symbol* s = m_st->lookup(p->m_symname->spelling());
        if (s == NULL) {
            this -> t_error(var_undef, p -> m_attribute);
        }

        p->m_attribute.m_basetype = s->m_basetype;     
    }

    void visitReturn(Return* p) {
        default_rule(p);
        // cout << "!!!!" << p->m_expr->m_attribute.basetype() << endl;
        p->m_attribute.m_basetype = p->m_expr->m_attribute.m_basetype;
    }

    void visitIfNoElse(IfNoElse* p) {
        default_rule(p);
        if(p->m_expr->m_attribute.m_basetype != bt_boolean) {
            this->t_error(ifpred_err, p->m_attribute);
        }
    }

    void visitIfWithElse(IfWithElse* p) {
        default_rule(p);
        if(p->m_expr->m_attribute.m_basetype != bt_boolean) {
            this->t_error(whilepred_err, p->m_attribute);
        }
    }

    void visitWhileLoop(WhileLoop* p) {
        default_rule(p);
        if(p->m_expr->m_attribute.m_basetype != bt_boolean) {
            this->t_error(whilepred_err, p->m_attribute);
        }
    }

    void visitCodeBlock(CodeBlock *p) {
        default_rule(p);
    }

    void visitTInteger(TInteger* p) {
        default_rule(p);
        p->m_attribute.m_basetype = bt_integer;
    }

    void visitTBoolean(TBoolean* p) {
        default_rule(p);
        p->m_attribute.m_basetype = bt_boolean;
    }

    void visitTCharacter(TCharacter* p) {
        default_rule(p);
        p->m_attribute.m_basetype = bt_char;
    }

    void visitTString(TString* p) {
        default_rule(p);
        p->m_attribute.m_basetype = bt_string;
    }

    void visitTCharPtr(TCharPtr* p) {
        default_rule(p);
        p->m_attribute.m_basetype = bt_charptr;
    }

    void visitTIntPtr(TIntPtr* p) {
        default_rule(p);
        p->m_attribute.m_basetype = bt_intptr;
    }

    void visitAnd(And* p) {
        default_rule(p);
        if(p->m_expr_1->m_attribute.m_basetype != bt_boolean || p->m_expr_2->m_attribute.m_basetype != bt_boolean) {
            this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_boolean;
    }

    void visitOr(Or* p) {
        default_rule(p);
        if(p->m_expr_1->m_attribute.m_basetype != bt_boolean || p->m_expr_2->m_attribute.m_basetype != bt_boolean) {
            this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_boolean;
    }

    void visitDiv(Div* p) {
        default_rule(p);
        if(p->m_expr_1->m_attribute.m_basetype != bt_integer || p->m_expr_2->m_attribute.m_basetype != bt_integer) {
            this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_integer;
    }

    void visitCompare(Compare* p) {
        default_rule(p);
        if(!( (p->m_expr_1->m_attribute.m_basetype==bt_integer && p->m_expr_2->m_attribute.m_basetype==bt_integer) ||
              (p->m_expr_1->m_attribute.m_basetype==bt_boolean && p->m_expr_2->m_attribute.m_basetype==bt_boolean) ) ) {
                  this->t_error(expr_type_err, p->m_attribute);
              }
        p->m_attribute.m_basetype = bt_boolean;
    }

    void visitNoteq(Noteq* p) {
        default_rule(p);
        if(!( (p->m_expr_1->m_attribute.m_basetype==bt_integer && p->m_expr_2->m_attribute.m_basetype==bt_integer) ||
              (p->m_expr_1->m_attribute.m_basetype==bt_boolean && p->m_expr_2->m_attribute.m_basetype==bt_boolean) ) ) {
                  this->t_error(expr_type_err, p->m_attribute);
              }
        p->m_attribute.m_basetype = bt_boolean;
    }

    void visitGt(Gt* p) {
        default_rule(p);
        if(p->m_expr_1->m_attribute.m_basetype != bt_integer || p->m_expr_2->m_attribute.m_basetype != bt_integer) {
            this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_boolean;
    }

    void visitGteq(Gteq* p) {
        default_rule(p);
        if(p->m_expr_1->m_attribute.m_basetype != bt_integer || p->m_expr_2->m_attribute.m_basetype != bt_integer) {
            this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_boolean;
    }

    void visitLt(Lt* p) {
        default_rule(p);
        if(p->m_expr_1->m_attribute.m_basetype != bt_integer || p->m_expr_2->m_attribute.m_basetype != bt_integer) {
            this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_boolean;
    }

    void visitLteq(Lteq* p) {
        default_rule(p);
        if(p->m_expr_1->m_attribute.m_basetype != bt_integer || p->m_expr_2->m_attribute.m_basetype != bt_integer) {
            this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_boolean;     
    }

    void visitMinus(Minus* p) {
        default_rule(p);
        if(p->m_expr_1->m_attribute.m_basetype != bt_integer || p->m_expr_2->m_attribute.m_basetype != bt_integer) {
            this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_integer;
    }

    void visitPlus(Plus* p) {
        default_rule(p);
        if(p->m_expr_1->m_attribute.m_basetype != bt_integer || p->m_expr_2->m_attribute.m_basetype != bt_integer) {
            this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_integer;
    }

    void visitTimes(Times* p) {
        default_rule(p);
        if(p->m_expr_1->m_attribute.m_basetype != bt_integer || p->m_expr_2->m_attribute.m_basetype != bt_integer) {
            this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_integer;
    }

    void visitNot(Not* p) {
        default_rule(p);
        if(p->m_expr->m_attribute.m_basetype != bt_boolean) {
           this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_boolean;
    }

    void visitUminus(Uminus* p) {
        default_rule(p);
        if(p->m_expr->m_attribute.m_basetype != bt_integer)
           this->t_error(expr_type_err, p->m_attribute);
        p->m_attribute.m_basetype = bt_integer;
    }

    void visitIntLit(IntLit* p) {
        default_rule(p);
        p->m_attribute.m_basetype = bt_integer;
    }

    void visitCharLit(CharLit* p) {
        default_rule(p);
        p->m_attribute.m_basetype = bt_char;
    }

    void visitBoolLit(BoolLit* p) {
        default_rule(p);
        p->m_attribute.m_basetype = bt_boolean;
    }

    void visitNullLit(NullLit* p) {
        default_rule(p);
        p->m_attribute.m_basetype = bt_ptr;
    }

    void visitAbsoluteValue(AbsoluteValue* p) {
        default_rule(p);
        if(p->m_expr->m_attribute.m_basetype != bt_integer) {
           this->t_error(expr_type_err, p->m_attribute);
        }
        p->m_attribute.m_basetype = bt_integer;
    }

    void visitArrayAccess(ArrayAccess* p) {
        default_rule(p);
        check_array_access(p);
    }

    void visitVariable(Variable* p) {
        default_rule(p);
        cout << "variable ";
        cout << p->m_symname->spelling() << endl;

        Symbol* s = m_st->lookup(p->m_symname->spelling());
        if (s == NULL) {
            this->t_error(var_undef, p -> m_attribute);
        }
        p->m_attribute.m_basetype = s->m_basetype;;
    }

    void visitAddressOf(AddressOf* p) {
        default_rule(p);
        Symbol* s = m_st->lookup(lhs_to_id(p->m_lhs));
        if (s == NULL) {
            this->t_error(var_undef, p -> m_attribute);
        }
        Basetype bt = s->m_basetype;
                cout << "AddressOf " << s->basetype() << endl;
        if(bt != bt_char && bt != bt_integer && bt != bt_string) {
            this->t_error(expr_addressof_error, p->m_attribute);
        }
        p->m_attribute.m_basetype = s->m_basetype;
    }

    void visitDeref(Deref* p) {                     // right side
        default_rule(p);
        cout << "visitDeref " << p->m_expr->m_attribute.basetype() << endl;

        Basetype bt = p->m_expr->m_attribute.m_basetype;
        if(bt != bt_intptr && bt != bt_charptr) {
            this->t_error(invalid_deref, p->m_attribute);
        }
    }

    void visitDerefVariable(DerefVariable* p) {        // left side
        default_rule(p);
        Symbol* s = m_st->lookup(p->m_symname->spelling());
        if (s == NULL) {
            this->t_error(var_undef, p -> m_attribute);
        }

        cout << "visitDerefVariable " << s->basetype() << endl;
        Basetype bt = s->m_basetype;
        if(bt != bt_intptr && bt != bt_charptr) {
            this->t_error(invalid_deref, p->m_attribute);
        }
        p->m_attribute.m_basetype = s->m_basetype;
    }

    void visitArrayElement(ArrayElement* p) {
        default_rule(p);
        Symbol* s = m_st->lookup(p->m_symname->spelling());
        if (s == NULL) {
            this->t_error(var_undef, p -> m_attribute);
        }
        if(p->m_expr->m_attribute.m_basetype != bt_integer) {
            this->t_error(array_index_error, p->m_attribute);
        }
        p->m_attribute.m_basetype = s->m_basetype;
    }

    // Special cases
    void visitPrimitive(Primitive* p) {}
    void visitSymName(SymName* p) {}
    void visitStringPrimitive(StringPrimitive* p) {}

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
};


void dopass_typecheck(Program_ptr ast, SymTab* st)
{
    Typecheck* typecheck = new Typecheck(stderr, st);
    ast->accept(typecheck); // Walk the tree with the visitor above
    delete typecheck;
}
