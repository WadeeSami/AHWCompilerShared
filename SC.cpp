#include "SC.h"
#define SYMANTIC_ERROR -1000
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
				this->isroutineSymanticallyCorrect(currentDeclaration->head);
				//check statemnts in the block
				this->symanicallyCheckfunctionBody(currentDeclaration->head->f.a_routine_decl.body);
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
		//if procedure, it should return nothin'

		if (routineTree->f.a_routine_decl.result_type == type_none) {
			cout << "a procedure must return nothin\'" << endl;
		}
		///////////
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
		if (routineTree->f.a_routine_decl.result_type == type_none) {
			return true;
		}
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

j_type sc::symanticallycheckExpression(AST * expr)
{
	//left and right side
	switch (expr->type)
	{
	case ast_times:
	case ast_divide:
	case ast_plus:
	case ast_minus:
	{
		//leaf and right has to be integer
		AST * left = expr->f.a_binary_op.larg;
		AST * right = expr->f.a_binary_op.rarg;

		if (left->type == ast_boolean || left->type == ast_string || right->type == ast_boolean || right->type == ast_string) {
			cout << " This Language can't add booleans or strings" << endl;
		}
		if (left->type == ast_integer && right->type == ast_integer) {
			return type_integer;
		}
		else {
			j_type leftRes = this->symanticallycheckExpression(left);
			j_type rightRes = this->symanticallycheckExpression(right);
			if (leftRes == rightRes) {
				return type_integer;
			}
			else {
				return j_type();
			}
		}
	}

	case ast_lt:
	case ast_le:
	case ast_gt:
	case ast_ge:
	case ast_neq:
	{
		//leaf and right has to be integer
		AST * left = expr->f.a_binary_op.larg;
		AST * right = expr->f.a_binary_op.rarg;

		if (left->type == ast_boolean || left->type == ast_string || right->type == ast_boolean || right->type == ast_string) {
			cout << " This Language can't add booleans or strings" << endl;
		}
		if (left->type == ast_integer && right->type == ast_integer) {
			return type_boolean;
		}
		else {
			j_type leftRes = this->symanticallycheckExpression(left);
			j_type rightRes = this->symanticallycheckExpression(right);
			if (leftRes == rightRes) {
				return type_boolean;
			}
			else {
				cout << "syntax error , trying to make relational operations on 2 diffrernet types " << endl;
				return type_none;
			}
		}

	}
	case ast_integer:
	case ast_string:
	case ast_boolean:
	{
		return j_type(this->convertAstTypeToJType(expr->type));
	}

	case ast_eq:
	{
		AST * left = expr->f.a_binary_op.larg;
		AST * right = expr->f.a_binary_op.rarg;

		if (left->type == ast_integer && right->type == ast_integer) {
			return type_boolean;
		}
		else if (left->type == ast_string && right->type == ast_string) {
			return type_boolean;
		}
		else if (left->type == ast_boolean && right->type == ast_boolean) {
			return type_boolean;
		}
		else {
			j_type leftRes = this->symanticallycheckExpression(left);
			j_type rightRes = this->symanticallycheckExpression(right);
			if (leftRes == rightRes) {
				return type_boolean;
			}
			else {
				cout << "symantic error , trying to compare 2 different types " << endl;
				return type_none;
			}
		}
	}//equal
	
	case ast_and:
	case ast_or:
	case ast_cand:
	case ast_cor: {
		//leaf and right has to be integer
		AST * left = expr->f.a_binary_op.larg;
		AST * right = expr->f.a_binary_op.rarg;

		if (left->type == ast_integer || left->type == ast_string || right->type == ast_integer || right->type == ast_string) {
			cout << "symantic error , trying to apply relational conjunction on not booleans" << endl;
		}
		if (left->type == ast_boolean && right->type == ast_boolean) {
			return type_boolean;
		}
		else {
			j_type leftRes = this->symanticallycheckExpression(left);
			j_type rightRes = this->symanticallycheckExpression(right);
			if (leftRes == rightRes) {
				return type_boolean;
			}
			else {
				cout << "symantic error , trying to apply relational conjunction on not booleans" << endl;
				return type_none;
			}
		}
	}
	default:
		return type_none;
	}
	return type_none;
}

bool sc::symanicallyCheckfunctionBody(AST * body)
{
	ast_list * stmts = body->f.a_block.stmts;
	if (!stmts) {
		//this function contains no statemnts, no checking
		return true;
	}
	
	while (stmts) {
		if (stmts->head) {
			this->symanticallyCheckStmt(stmts->head);
			stmts = stmts->tail;
		}
	}
	return true;
}

bool sc::symanticallyCheckStmt(AST * stmt)
{

	switch (stmt->type)
	{
	case ast_assign: {
		//check expression
		return this->symanticallycheckExpression(stmt->f.a_assign.rhs);
	}
	case ast_if: {
		//check predicate
		if (this->symanticallycheckExpression(stmt->f.a_if.predicate) != type_boolean) {
			cout << "symantic error in the if statement predicate " << endl;
		}
		//check consequence
		if (this->symanticallyCheckStmt(stmt->f.a_if.conseq) == type_none) {
			cout << "symantic error in the if statement consequence " << endl;
		}
		if (this->symanticallyCheckStmt(stmt->f.a_if.altern) == type_none) {
			cout << "symantic error in the if statement alternative " << endl;
		}
	}
	default:
		break;
	}
	return false;
}



