#include <iostream>
#include "ast.h"
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

	//p->parse(fd,st);	
	AST *ast = new AST();
	ast = make_ast_node(0);
	cout << ast->type;
	//cout << ast->f.a_var_decl.name->entry_type;
	int x;
	cin >> x;
	return 0;
}