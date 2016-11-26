#include <iostream>
#include "parser.h"
#include <vector>
using namespace std;

int main() {
	/* working fine for scanner
	FileDescriptor *fd = new FileDescriptor("myFile.txt"); //constructing FILEDESCRIPTOR with a TXT file
	SCANNER *myS = new SCANNER(fd);

	char * line = fd->getNextLine();
	TOKEN *T;
	while (line) {
		cout << line<<endl;
		T = myS->Scan();
		while (T) {
			myS->print_token(T);
			cout << endl;
			T = myS->Scan();
		}
		line = fd->getNextLine();
	}*/
	/*
	TOKEN *t = new TOKEN();
	t->str_ptr = "ahmad";
	t->type = lx_identifier;

	symbolTable *st = new symbolTable();
	st->insertElement("ahmad11", *t);
	st->enterScope();
	st->insertElement("ahmad21", *t);
	st->enterScope();
	st->insertElement("ahmad22", *t);
	st->lookUp("ahmad11");*/
	
	/*TOKEN *t = new TOKEN();
	t->type = kw_integer;*/
	FileDescriptor *fd = new FileDescriptor("myFile.txt"); //constructing FILEDESCRIPTOR with a TXT file
	symbolTable *st = new symbolTable();
	parser *p = new parser();

	p->parse(fd,st);	
	
	/*Element *e = new Element();
	e->id = 0;
	e->name = "ahmad";
	e->scopeId = 0;
	AST *ast = new AST();
	ast = make_ast_node(0,e,0);
	cout << ast->type;
	cout << ast->f.a_var_decl.type;
	st->printElement(ast->f.a_var_decl.name);*/
	int x;
	cin >> x;
	return 0;
}