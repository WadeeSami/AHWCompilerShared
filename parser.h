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
	bool IDorFuncCall;
	parser();
	int findJType(int x);
	void parse(FileDescriptor * fd, symbolTable * st);
	void scanAllTokens();
	TOKEN *getNextToken();
	bool arithOp();
	TOKEN* uniaryOp();
	TOKEN* star_divide();
	TOKEN* plus_minus();
	ast_list* argList();
	ast_list* A();
	AST* expr();
	AST* exprbar();
	AST* D();
	AST* F();
	int findRelOp(int x);
	int findJTypeFromToken(int x);
	AST* Dbar();
	AST* G();
	AST* Fbar();
	AST* H();
	AST* I();
	ast_list* J();
	AST* Gbar();
	ast_list* args();
	ast_list* B();
	TOKEN* relConj();
	TOKEN* relOp();
	AST* stmt();
	AST* Z();
	AST* Y();
	ast_list* stmtList();
	bool varDecl();
	TOKEN* type();
	bool varDeclList();
	bool block();
	ast_list* formals();
	ast_list* formalsBar();
	AST * decl();
	ast_list * declList();
	ast_list* formalList();
	ast_list* X();
	bool match(TOKEN *t);
	void cleanUpVector();
};