//ast.c
/* Create and return a new abstract syntax tree (ASTT) node. The first
argument is the type of the node (one of ast_type). The rest of the
arguments are the fields of that type of node, in order. */

#include <stdio.h>
#include <stdarg.h>
#include "ast.h"
//#include "exception.h"
#include <iostream>
/* Internal routines: */
static void nl_indent (FILE *, int);
static void p_a_n (FILE *, AST *, int);
//static void print_ast_list (FILE *, ast_list *, char *, int);
static void print_element_list (FILE *, element_list *, char *, char *, int);
char* OP_NAMES[] = {"*", "/", "+", "-", ":=", "=", "!=", ">", ">=", "<", "<=", "and", "or" };

///////////////////////////////////////////////////////////////////////
// page 2
AST * make_ast_node (int type,...)
{
    //AST *n = (AST *) malloc (sizeof (AST)); // change to new
    AST *n = new AST();
    va_list ap;

    if (n == NULL) {
        //fatal_error ("malloc failed in make_ast_node\n");
        cout << "AST  is NULL \n";
        exit(0);
    }

    va_start(ap , type);

    n->type = ast_type(type);
    switch (type) {
        case ast_var_decl:
            n->f.a_var_decl.name = va_arg (ap, Element *);
            n->f.a_var_decl.type = j_type(va_arg (ap, int));
            break;

        case ast_const_decl:
            n->f.a_const_decl.name = va_arg (ap, Element *);
            n->f.a_const_decl.value = va_arg (ap, int);
            break;

        case ast_routine_decl:
            n->f.a_routine_decl.name = va_arg (ap, Element *);
            n->f.a_routine_decl.formals = va_arg (ap, element_list *);
            n->f.a_routine_decl.result_type = j_type(va_arg (ap, int ));
            n->f.a_routine_decl.body = va_arg (ap, AST *);
            break;

        case ast_assign:
            n->f.a_assign.lhs = va_arg (ap, Element *);
            n->f.a_assign.rhs = va_arg (ap, AST *);
            break;

        case ast_if:
            n->f.a_if.predicate = va_arg (ap, AST *);
            n->f.a_if.conseq = va_arg (ap, AST *);
            n->f.a_if.altern = va_arg (ap, AST *);
            break;

        case ast_while:
            n->f.a_while.predicate = va_arg (ap, AST *);
            n->f.a_while.body = va_arg (ap, AST *);
            break;

        case ast_for:
            n->f.a_for.var = va_arg (ap, Element *);
            n->f.a_for.lower_bound = va_arg(ap, AST *);
            n->f.a_for.upper_bound = va_arg (ap, AST *);
            n->f.a_for.body = va_arg (ap, AST *);
            break;

        case ast_read: //page 3
            n->f.a_read.var = va_arg (ap, Element *);
            break;

        case ast_write:
            n->f.a_write.var = va_arg (ap, Element *);
            break;

        case ast_call:
            n->f.a_call.callee = va_arg (ap, Element *);
            n->f.a_call.arg_list = va_arg (ap, ast_list *);
            break;

        case ast_block:
            n->f.a_block.vars = va_arg (ap, element_list *);
            n->f.a_block.stmts = va_arg (ap, ast_list *);
        break;

        case ast_return:
            n->f.a_return.expr = va_arg (ap, AST *);
            break;

        case ast_var:
            n->f.a_var.var = va_arg (ap, Element *);
            break;

        case ast_integer:
            n->f.a_integer.value = va_arg (ap, int);
            break;

        case ast_float:
            n->f.a_float.value = va_arg (ap, double);
            break;
        case ast_string:
            n->f.a_string.str_value = va_arg (ap, char*);
            break;
        case ast_boolean:
            n->f.a_boolean.value = va_arg (ap, int);
            break;

        case ast_times:
        case ast_divide:
        case ast_plus:
        case ast_minus:
        case ast_eq:
        case ast_neq:
        case ast_lt:
        case ast_le:
        case ast_gt:
        case ast_ge:
        case ast_and:
        case ast_or:
        case ast_cand:
        case ast_cor:
            n->f.a_binary_op.larg = va_arg (ap, AST *);
            n->f.a_binary_op.rarg = va_arg (ap, AST *);
            break;

        case ast_not:
        case ast_uminus:
            n->f.a_unary_op.arg = va_arg (ap, AST *);
            break;
    // new page page 4
        case ast_itof:
            n->f.a_itof.arg = va_arg (ap, AST *);
            break;
        case ast_eof:
            break;
        default:
            cout << ("Unknown type of AST node in make_ast_node\n");
            //throw Exception("Unknown type of AST node");
    }
    return (n);
}
//////////////////////////////////////////////////////////////////////
/* Evaluate a constant expression represented as an AST. Return an
integer value. Errors are reported as errors in the user's program. */
int eval_ast_expr (FileDescriptor *fd, AST *n)
{
    if (n == NULL){
        cout << ("NULL AST in eval_ast_expr\n");
        //throw Exception("NULL AST in eval_ast_expr'n");
    }
    switch (n->type)
    {
    case ast_var:
        if (n->f.a_var.var->entry_type == ste_const)
            return (n->f.a_var.var->f.constant.value);
        else
            //report_error (fd, "Cannot use variables in constant expressions");
            cout << "Cannot use variables in constant expressions\n";
        return (0);
        case ast_integer:
            return (n->f.a_integer.value);
        case ast_string:
            //report_error (fd, "Cannot use strings in constant expressions");
            cout << "Cannot use strings in constant expressions\n";

            return (0);
        case ast_boolean:
            return (n->f.a_boolean.value);
        case ast_times:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) * eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_divide:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) / eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_plus:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) + eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_minus:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) - eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_eq:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) == eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_neq:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) != eval_ast_expr (fd, n->f.a_binary_op.rarg)); // page 5
        case ast_lt:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) < eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_le:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) <= eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_gt:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) > eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_ge:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) >= eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_and:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) & eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_or:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) | eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_cand:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) & eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_cor:
            return (eval_ast_expr (fd, n->f.a_binary_op.larg) | eval_ast_expr (fd, n->f.a_binary_op.rarg));
        case ast_not:
            return (~eval_ast_expr (fd, n->f.a_unary_op.arg));
        case ast_uminus:
            return (- eval_ast_expr (fd, n->f.a_unary_op.arg));
        default:
            cout << "Unknown AST node in eval ast expr \n";
            //throw Exception("Unknown AST node in eval ast expr \n");
            return (0);
        }
}
/////////////////////////////////////////////////////////////////////
/* Print (to the file F) the AST N. This routine doesn't try very hard
to format the output. */
void print_ast_node (FILE *f, AST *n)
{
      p_a_n (f, n, 0);
}
///////////////////////////////////////////////////////////////////////
static void p_a_n (FILE *f, AST *n, int d)
{
    if (n == NULL) return;
    switch (n->type)
    {
    // new page 6
        case ast_var_decl:
            fprintf (f,"var %s: %s;", n->f.a_var_decl.name->name,
                        type_names [n->f.a_var_decl.type]);
             nl_indent (f, d);
            break;
        case ast_const_decl:
            fprintf (f, "constant %s = %d;", n->f.a_const_decl.name->name,
                        n->f.a_const_decl.value);
            nl_indent (f, d);
            break;
        case ast_routine_decl:
                if (n->f.a_routine_decl.result_type == type_none)
                    fprintf (f, "procedure %s (",  (n->f.a_routine_decl.name->name));
                else
                    fprintf (f, "function %s (",  (n->f.a_routine_decl.name->name));
				print_element_list(f, n->f.a_routine_decl.formals, "", ", ", -1);
                if (n->f.a_routine_decl.result_type == type_none){
                    fprintf (f, ")");
                    nl_indent (f, d + 2);
                } else{
                        fprintf (f, ") : %s",
                            type_names [n->f.a_routine_decl.result_type]);
                        nl_indent (f, d + 2);
                }
                p_a_n (f, n->f.a_routine_decl.body, d + 2);
                fprintf (f, ";");
                nl_indent (f, d);
                break;
        case ast_assign:
            fprintf (f, "%s := ",  (n->f.a_assign.lhs->name));
            p_a_n (f, n->f.a_assign.rhs, d);
            break;
        case ast_if:
            fprintf (f, "if ");
            p_a_n (f, n->f.a_if.predicate, d);
            fprintf (f, " then");
            nl_indent (f, d + 2);
            p_a_n (f, n->f.a_if.conseq, d + 2);
            if (n->f.a_if.altern != NULL){
                nl_indent (f, d);
                fprintf (f, "else");
                nl_indent (f, d + 2);
                p_a_n (f, n->f.a_if.altern, d + 2);
            }
            break;
        case ast_while:
            fprintf (f, "while ");
            p_a_n (f, n->f.a_while.predicate, d);
            fprintf (f, " do");
            nl_indent (f, d + 2);
            p_a_n (f, n->f.a_while.body, d + 2);
            nl_indent (f, d);
            fprintf (f, "od");// new page
            break;
        case ast_for:
            fprintf (f, "for %s = ",  (n->f.a_for.var->name));
            p_a_n (f, n->f.a_for.lower_bound, d);
            fprintf (f, " to ");
            p_a_n (f, n->f.a_for.upper_bound, d);
            fprintf (f, " do");
            nl_indent (f, d + 2);
            p_a_n (f, n->f.a_for.body, d + 2);
            nl_indent (f, d);
            fprintf (f, "od");
            break;
        case ast_read:
            fprintf (f, "read (%s)",  (n->f.a_read.var->name));
            break;
        case ast_write:
            fprintf (f, "write (%s)",  (n->f.a_read.var->name));
            break;
        case ast_call:
            fprintf(f, "%s (",  (n->f.a_call.callee->name));
            print_ast_list (f, n->f.a_call.arg_list, ", ", -1);
            fprintf (f, ")");
            break;
        case ast_block:
            fprintf (f, "begin");
            nl_indent (f, d + 2);
			print_element_list(f, n->f.a_block.vars, "var ", "", d + 2);
            print_ast_list (f, n->f.a_block.stmts, ";", d + 2);
            nl_indent (f, d);
            fprintf (f, "end");
            break;
        case ast_return:
            fprintf(f, "(return ");
            p_a_n (f, n->f.a_return.expr, d);
            fprintf (f, ")");
            break;
        case ast_var:
            fprintf (f, "%s",  (n->f.a_var.var->name));
            break;
        case ast_integer:
            fprintf (f, "%d", n->f.a_integer.value);
            break;
        case ast_float:
            fprintf (f, "%f", n->f.a_float.value);
            break;
        case ast_string:
            fprintf (f, "%s", n->f.a_string.str_value);
            break;
        case ast_boolean:
            fprintf (f, n->f.a_boolean.value ? "true" : "false");
            break;
        case ast_times:
        case ast_divide:
        case ast_plus:
        case ast_minus:
        case ast_eq:
        case ast_beq:
        case ast_neq:
        case ast_gt:
        case ast_ge:
        case ast_lt:
        case ast_le:
        case ast_and:
        case ast_or:
            fprintf(f, "(");
            p_a_n (f, n->f.a_binary_op.larg, d);
            fprintf(f," %s ",OP_NAMES[n->type-16]);
            p_a_n (f, n->f.a_binary_op.rarg, d);
            fprintf (f, ")");
           break;
        case ast_not:
            fprintf (f, "(not ");
            p_a_n (f, n->f.a_unary_op.arg, d);
            fprintf (f, ")");
            break;
        case ast_uminus:
            fprintf (f, "( -");
            p_a_n (f, n->f.a_unary_op.arg, d);
            fprintf (f, ")");
            break;
        case ast_eof:
            fprintf (f, "EOF");
            break;
        default:
            cout << "Unknown AST node type\n";
            //throw Exception("Unknown AST node type");
      }
}
///////////////////////////////////////////////////
/* Print a list of AST nodes. */
void print_ast_list (FILE *f, ast_list *L, char *sep, int d)
{
    for ( ; L != NULL; L = L->tail){
        p_a_n (f, L->head, d);
        if (L->tail || d>0){
            fprintf (f, sep);
            if (L->tail && d >= 0) nl_indent (f, d);
        }
    }
}
////////////////////////////////////////////////////////////////
/* Print a list of symbol table entries along with their types. */
static void print_element_list (FILE *f, element_list *L, char *prefix, char *sep, int d)
{

    for ( ; L != NULL; L = L->tail) {
            fprintf (f, "%s%s : %s", prefix,  (L->head->name),
                        type_names [ (L->head->f.var.type)]);
            if (L->tail || d >= 0) fprintf (f, sep); // new page 10
            if (d >= 0) nl_indent (f, d);
   }
}
///////////////////////////////////////////////////
/* Print a newline and indent D space. */
static void nl_indent (FILE *f, int d)
{
    fprintf (f, "\n");
    while (d-- > 0) fprintf (f, " ");
}

/*//Return the value of the constant recorded in entry e.
int ste_const_value (Element *e ){
    return e->f.constant.value;
}
//Return the name of the object recordedin entry e.
const char* ste_name (Element *e){
    return e->name.c_str();
}

//Return the type of the variable recorded in entry e.
j_type ste_var_type (Element *e){
    return e->f.var.type;
}*/

Element * createSymbolTablEntry(element_type entry_type,...){

    va_list args;
    va_start(args,entry_type);

	Element* ste = new Element();
    ste->entry_type = entry_type;
    ste->name = va_arg(args, char*);
    switch (ste->entry_type) {
        case ste_const:
            ste->f.constant.value = va_arg(args, int);
            break;
        case ste_var:
            ste->f.var.type = j_type(va_arg(args, int));
            break;
        case ste_routine:
            ste->f.routine.formals = va_arg(args, element_list*);
            ste->f.routine.result_type = j_type(va_arg(args, int));
            break;
        default:
			cout << "haha";
			break;
           // throw Exception("Unkown ste type");
    }
    return ste;
}












