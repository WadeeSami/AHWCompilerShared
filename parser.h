#pragma once
#include "scanner.h"
#include "fd.h"
#include "symboltable.h"
#include <vector>
#include <iostream>

using namespace std;


class parser {
private:
	int index;
	vector <TOKEN*> *tokens;
public:
	FileDescriptor *fd;
	SCANNER *myS;
	symbolTable *st;
	TOKEN currToken;
	char * line;
	parser();
	void parse(FileDescriptor * fd, symbolTable * st);
	void scanAllTokens();
	TOKEN *getNextToken();
	bool arithOp();
	bool uniaryOp();
	bool star_divide();
	bool plus_minus();
	bool argList();
	bool A();
	bool expr();
	bool exprbar();
	bool D();
	bool F();
	bool Dbar();
	bool G();
	bool Fbar();
	bool H();
	bool I();
	bool J();
	bool Gbar();
	bool args();
	bool B();
	bool relConj();
	bool relOp();
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
	bool decl();
	bool declList();
	bool formalList();
	bool X();
	bool match(TOKEN *t);
	void cleanUpVector();
};