#pragma once
#include "symboltable.h"
#include "ast.h"
#include <vector>
#include <iostream>

using namespace std;


class parser {
private:
	int index;
	vector <TOKEN*> *tokens;
public:
	ast_list_cell *program;
	AST * tempDeclNode;
	FileDescriptor *fd;
	SCANNER *myS;
	symbolTable *st;
	TOKEN currToken;
	char * line;
	parser();
	int findJType(int x);
	void parse(FileDescriptor * fd, symbolTable * st);
	void scanAllTokens();
	TOKEN *getNextToken();
	bool arithOp();
	TOKEN* uniaryOp();
	TOKEN* star_divide();
	TOKEN* plus_minus();
	bool argList();
	bool A();
	AST* expr();
	AST* exprbar();
	AST* D();
	AST* F();
	int findRelOp(int x);
	AST* Dbar();
	AST* G();
	AST* Fbar();
	AST* H();
	AST* I();
	AST* J();
	AST* Gbar();
	bool args();
	bool B();
	TOKEN* relConj();
	TOKEN* relOp();
	bool stmt();
	bool Z();
	bool Y();
	bool stmtList();
	bool varDecl();
	bool type();
	bool varDeclList();
	bool block();
	bool formals();
	bool formalsBar();
	AST * decl();
	ast_list * declList();
	bool formalList();
	bool X();
	bool match(TOKEN *t);
	void cleanUpVector();
};