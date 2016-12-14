#include "SC.h"

sc::sc()
{
}

bool sc::checkStatement(AST * n, j_type expectedType)
{
	return false;
}

j_type sc::expressionType(AST * n)
{

	return j_type();
}

void sc::symanticallyCheck(ast_list * program)
{
	ast_list * currentDeclaration = NULL;
	currentDeclaration = program;
	while (currentDeclaration) {
		//cout << "Hello from the other side" << endl;
		if (currentDeclaration->head) {
			if (currentDeclaration->head->type == ast_routine_decl) {
				cout << "This is a function" << endl;
				this->isroutineSymanticallyCorrect(currentDeclaration->head);
			}
		}
		currentDeclaration  = currentDeclaration->tail;
		
	}
}

bool sc::isroutineSymanticallyCorrect(AST * routineTree)
{
	//check return type
	/*
	the body of the function should contain a return ast
	*/
	cout << routineTree->f.a_routine_decl.result_type<<endl;
	AST * functionBody = routineTree->f.a_routine_decl.body;
	ast_list * bodyStatementList = functionBody->f.a_block.stmts;
	//check ther return ast
	AST * returnAST = NULL;
	while (bodyStatementList) {
		if (bodyStatementList->head) {
			if (bodyStatementList->head->type == ast_return) {
				returnAST = bodyStatementList->head;
				break;
			}
		}
		bodyStatementList = bodyStatementList->tail;
	}

	if (returnAST) {
		j_type formalReturnType = routineTree->f.a_routine_decl.result_type;
		//ast_type  actualReturnType =  returnAST->f.a_return.expr->type;
		ast_type  actualReturnType = this->findExpressionResultType(returnAST->f.a_return.expr);
		
		if (actualReturnType == ast_var) {
			j_type varJType = returnAST->f.a_return.expr->f.a_var.var->f.var.type;
			if (formalReturnType !=varJType) {
				cout << "This function  \' " << routineTree->f.a_routine_decl.name->name << " \' returns a strange variable type" << endl;;
			}
		}else if (formalReturnType != this->convertAstTypeToJType(actualReturnType)) {
			cout << "This function  \' "<< routineTree->f.a_routine_decl.name->name << " \' does not return value correcly"<<endl;;
		}
		
	}
	else {
		cout << "syntax error, a function must return a value, otherwise, use procedure" << endl;
	}
	//cout << "this is the funtion name" << endl;

	return false;
}

int sc::convertAstTypeToJType(ast_type type)
{
		if (type == ast_integer) {
			return type_integer;
		}
		else if (type == ast_string) {
			return type_string;
		}
		else if (type == ast_boolean) {
			return type_boolean;
		}
		else if (type == ast_float) {
			return type_float;
		}
		return type_none;
	
}

ast_type sc::findExpressionResultType(AST * expr)
{
	if (expr->type == ast_string || expr->type == ast_integer || expr->type == ast_boolean) {
		return expr->type;
	}
	switch (expr->type) {
	case ast_times:
	case ast_divide:
	case ast_plus:
	case ast_minus:
	{
		return ast_integer;
	}

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
	{
		return ast_boolean;
	}
	case ast_var: {
		return ast_var;
	}

	default: {
		cout << "This is strange";
	}

	}//switch
	return ast_type();
}

