#pragma once
#include "symboltable.h"
#include "ast.h";

class sc {
private:
	

public:
	sc();
	bool checkStatement(AST * n, j_type expectedType);
	j_type expressionType(AST * n);
	void symanticallyCheck(ast_list * program);
	bool isroutineSymanticallyCorrect(AST * routineTree);
	int convertAstTypeToJType(ast_type type);
	ast_type findExpressionResultType(AST * expr);
	j_type symanticallycheckExpression(AST * expr);
	bool symanicallyCheckfunctionBody(AST * body);
	bool symanticallyCheckStmt(AST * stmt);

};