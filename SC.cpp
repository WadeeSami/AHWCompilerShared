#include "SC.h"
#define SYMANTIC_ERROR -1000

sc::sc(symbolTable * st)
{
	this->st = st;
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
	case ast_minus:
	{
		//leaf and right has to be integer
		AST * left = expr->f.a_binary_op.larg;
		AST * right = expr->f.a_binary_op.rarg;

		if (left->type == ast_boolean || left->type == ast_string || right->type == ast_boolean || right->type == ast_string) {
			cout << " This Language can't add booleans or strings" << endl;
			return type_none;
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
				return type_none;
			}
		}
	}
	/////////
	case ast_plus: {
		//leaf and right has to be integer
		AST * left = expr->f.a_binary_op.larg;
		AST * right = expr->f.a_binary_op.rarg;

		if (left->type == ast_boolean || right->type == ast_boolean) {
			cout << " This Language can't add booleans " << endl;
			return type_none;
		}
		else if (left->type == ast_integer && right->type == ast_integer) {
			return type_integer;
		}
		else if (left->type == ast_string && right->type == ast_string) {
			return type_string;
		}
		else {
			j_type leftRes = this->symanticallycheckExpression(left);
			j_type rightRes = this->symanticallycheckExpression(right);
			if (leftRes == rightRes) {
				return leftRes;
			}
			else {
				return type_none;
			}
		}
		break;
	}


	///////////
	
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
			return type_none;
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
	case ast_call: {
		if (this->compareFormalAndActualParameters(expr)) {
			return type_routine;
		}
		break;
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
		j_type expressionType = this->symanticallycheckExpression(stmt->f.a_assign.rhs);
		if (stmt->f.a_assign.lhs->entry_type == ste_var) {
			if (expressionType != stmt->f.a_assign.lhs->f.var.type) {
				cout << "symantic error, left handside and right handside must have the same type " << endl;
				return false;
			}
			return true;
		}
		cout << "Symantic error, left handside must be a variable " << endl;
		return false;
	}
	case ast_if: {
		//check predicate
		if (this->symanticallycheckExpression(stmt->f.a_if.predicate) != type_boolean) {
			cout << "symantic error in the if statement predicate " << endl;
		}
		//check consequence
		if (!this->symanticallyCheckStmt(stmt->f.a_if.conseq)) {
			cout << "symantic error in the if statement consequence " << endl;
		}
		if (!this->symanticallyCheckStmt(stmt->f.a_if.altern)) {
			cout << "symantic error in the if statement alternative " << endl;
		}
		return true;
	}
	case ast_while:{
		//check predicate
		if (this->symanticallycheckExpression(stmt->f.a_while.predicate) != type_boolean) {
			cout << "symantic error in the while statement predicate " << endl;
		}
		//check consequence
		if (!this->symanticallyCheckStmt(stmt->f.a_while.body)) {
			cout << "symantic error in the while statement body " << endl;
		}
		return true;
	}
	case ast_for: {
		//check if var is integer
		if (stmt->f.a_for.var->f.var.type != type_integer) {
			cout << "Symantic error , loop variable must be an integer" << endl;
		}
		//the expressions must return an integer
		if (this->symanticallycheckExpression(stmt->f.a_for.lower_bound) != type_integer) {
			cout << "Symantic error in the lower bound of the for loop, must result in an integer value" << endl;
		}
		if (this->symanticallycheckExpression(stmt->f.a_for.upper_bound) != type_integer) {
			cout << "Symantic error in the upper bound of the for loop, must result in an integer value" << endl;
		}
		if (!this->symanticallyCheckStmt(stmt->f.a_for.body)) {
			cout << "symantic error in the for statement body " << endl;
		}
		return true;
	}
	case ast_call: {
		//check matching formal and actual parameters
		/*ast_list * actualParameters = stmt->f.a_call.arg_list;
		Element * routinRecord = this->st->lookUp(stmt->f.a_routine_decl.name->name);
		element_list * stFormalsList = routinRecord->f.routine.formals;*/
		return this->compareFormalAndActualParameters(stmt);
	}
	default:
		break;
	}
	return false;
}

bool sc::isExpressionNode(AST * ast)
{
	switch (ast->type) {
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
	{
		return true;
	}

	default: {
		return false;
	}
	}
	return false;
}

bool sc::compareFormalAndActualParameters(AST * routineCall)
{
	Element * routinRecord = this->st->lookUp(routineCall->f.a_call.callee->name);
	element_list * stFormalParameters = routinRecord->f.routine.formals;
	ast_list * functionActualParametes = routineCall->f.a_call.arg_list;

	while (stFormalParameters && functionActualParametes) {
		if (stFormalParameters->head && functionActualParametes->head) {
			//check if equal types
			if (stFormalParameters->head->f.var.type == j_type(this->convertAstTypeToJType(functionActualParametes->head->type))) {
				stFormalParameters = stFormalParameters->tail;
				functionActualParametes = functionActualParametes->tail;
			}
			else if (functionActualParametes->head->type == ast_var) {
				if (stFormalParameters->head->f.var.type == functionActualParametes->head->f.a_var.var->f.var.type) {
					stFormalParameters = stFormalParameters->tail;
					functionActualParametes = functionActualParametes->tail;
				}
				else {
					cout << "Calling the function " << routinRecord->name << " with Invaid arguments/ differnet types " << endl;
					return false;
				}
			}
			else if (this->isExpressionNode(functionActualParametes->head)) {
				//case of an expression
				if (stFormalParameters->head->f.var.type == this->symanticallycheckExpression(functionActualParametes->head)) {
					stFormalParameters = stFormalParameters->tail;
					functionActualParametes = functionActualParametes->tail;
				}
				else {
					return false;
				}
			}
			else {
				cout << "Calling the function " << routinRecord->name << " with Invaid arguments/ differnet types " << endl;
				return false;
			}
		}
		else if (stFormalParameters->head || functionActualParametes->head) {
			//one of them does not have the same number of arguments
			cout << "Calling the function " << routinRecord->name << " with a different number of arguments " << endl;
			return false;
		}
		else {
			//equally likely
			return true;
		}
	}
	return false;
}



