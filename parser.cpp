#include "parser.h"

parser::parser() {
	tokens = new vector <TOKEN*>;
	index = 0;
}

int parser::findJType(int c)
{
	int typeIndex;
	if (c == 19)return 0;
	else if (c == 27)return 1;
	else if (c == 6)return 2;
	else if (c == 14)return 3;
	else return 4;
	return 0;
}

void parser::parse(FileDescriptor *fd, symbolTable *st) {
	this->fd = fd;
	this->st = st;
	program = new ast_list_cell();
	myS = new SCANNER(this->fd);
	scanAllTokens();
	cleanUpVector();

	cout<<eval_ast_expr(fd,expr());
}

void parser::scanAllTokens() {
	line = fd->getNextLine();
	TOKEN *T;
	while (line) {
		T = myS->Scan();
 		while (T) {
			tokens->push_back(T);
			T = myS->Scan();
		}
		line = fd->getNextLine();
	}
}
/*TOKEN*  parser::getNextToken(){
	line = fd->getCurrLine();
	TOKEN *T;
	if (line) {
		T = myS->Scan();
		if (T) {
			myS->print_token(T);
			cout << endl;
		}
		else
		{
			line = fd->getNextLine();
		}
	}
	else
	{
		line = fd->getNextLine();
	}
	return (new TOKEN());
}*/

ast_list * parser::declList() { 
	ast_list * list = new ast_list();
	TOKEN *t = new TOKEN();
	tempDeclNode = decl();
	ast_list_cell *temp = new ast_list_cell();
	if (tempDeclNode != NULL && tempDeclNode->f.a_var_decl.name != NULL) {
		t->type = lx_semicolon;
		if (match(t)) {
			temp->head = tempDeclNode;
			print_ast_node(fd->fpOUT,tempDeclNode);
			tempDeclNode = NULL;
			temp->tail = declList();
			return temp->tail;
		}
		else cout<<"Syntax Error\n";
	}	
	return NULL;
}
AST* parser::decl() { 
	TOKEN *t = new TOKEN();
	t->type = kw_var;
	if (match(t)) {
		t->type = lx_identifier;
		if (match(t)) {

			t->type = Ix_colon;
			if (match(t)) {
				if (type()) {
					
					AST* ast = new AST();
					ast = make_ast_node(ast_var_decl,
						st->makeElement(tokens->at(index-3)->str_ptr, *tokens->at(index - 3)),
						findJType(tokens->at(index - 1)->type));
					
					return ast;
				}
				else {
					cout << "errors in type in line "<<fd->GetLineNum();
					return new AST();
				}
			}
			else {
				cout << "error: no colon in line " << fd->GetLineNum();
				return new AST();
			}
		}
		else {
			cout << "no identefier found in line "<<fd->GetLineNum();
			return new AST();
		}
	}
	else{
		t->type = kw_constant;
		if (match(t)) {
			t->type = lx_identifier;
			if (match(t)) {
				t->type = lx_eq;
				if (match(t)) {
					AST* ast = new AST();
					Element *e = new Element();
					e = st->makeElement(tokens->at(index - 2)->str_ptr, *tokens->at(index - 2));
					e->f.constant.value = 8;
					/*if (expr()) {
						ast = make_ast_node(ast_const_decl,
							,
							type_integer);
						int evalued = eval_ast_expr(fd, ast);*/
						return ast;
					//}
				}
			}
		}
		else {
			t->type = kw_function;
			if (match(t)) {
				t->type = lx_identifier;
				if (match(t)) {
					if (formalList()) {
						t->type = Ix_colon;
						if (match(t)) {
							if (type()) {
								AST* ast = new AST();
								ast = make_ast_node(ast_const_decl,
									st->makeElement(tokens->at(index - 2)->str_ptr, *tokens->at(index - 2)),
									tokens->at(index)->type);
								if (block()) return ast;
							}
						}
					}
				}
			}
			else {
				t->type = kw_procedure;
				if (match(t)) {
					t->type = lx_identifier;
					if (match(t)) {
						if (formalList()) {
							AST* ast = new AST();
							ast = make_ast_node(ast_const_decl,
								st->makeElement(tokens->at(index - 2)->str_ptr, *tokens->at(index - 2)),
								tokens->at(index)->type);
							if (block()) return ast;	
						}
					}
				}
			}
		}
	}
	
	return new AST();
}
bool parser::type() { 
	TOKEN *t = new TOKEN();
	t->type = kw_integer;
	if (match(t)) {
		return true;
	}
	else {
		t->type = kw_boolean;
		if (match(t)) {
			return true;
		}
		else {
			t->type = kw_string;
			if (match(t)) {
				return true;
			}
		}
	}
	return false;
}
bool parser::formalList() { 
	TOKEN *t = new TOKEN();
	t->type = lx_lparen;
	if (match(t)) {
		if (X()) {
			return true;
		}
	}
	return false;
}
bool parser::X() {
	TOKEN *t = new TOKEN();
	if (formals()) {
		t->type = lx_rparen;
		if (match(t)) {
			return true;
		}
	}
	else {
		t->type = lx_rparen;
		if (match(t))return true;
	}
	return false;
}
bool parser::formals() {
	TOKEN *t = new TOKEN();
	t->type = lx_identifier;
	if (match(t)) {
		t->type = Ix_colon;
		if(match(t)){
			if (type()) {
				return (formalsBar());
			}
		}
	}
	return false;
}
bool parser::formalsBar() {
	TOKEN *t = new TOKEN();
	t->type = lx_comma;
	if (match(t)) {
		t->type = lx_identifier;
		if (match(t)) {
			t->type = Ix_colon;
			if (match(t)) {
				if (type()) {
					return(formalsBar()); 
				}
				else return false;

			}
			else return false;

		}
		else return false;

	}
	return true;
	
}
bool parser::stmt() { 
	TOKEN *t = new TOKEN();
	t->type = lx_identifier;
	if (match(t)) {
		return Y();
	}
	else {
		t->type = kw_if;
		if (match(t)) {
			if (expr()) {
				t->type = kw_then;
				if (match(t)) {
					if (stmt()) {
						return Z();
					}
				}
			}
		}
		else {
				t->type = kw_while;
				if (match(t)) {
					if (expr()) {
						t->type = kw_do;
						if (match(t)) {
							if (stmt()) {
								t->type = kw_od;
								return (match(t)); 
							}
						}
					}
				}
				else {
					t->type = kw_for;
					if (match(t)) {
						t->type = lx_identifier;
						if (match(t)) {
							t->type = Ix_colon_eq;
							if (match(t)) {
								if (expr()) {
									t->type = kw_to;
									if (match(t)) {
										if (expr()) {
											t->type = kw_do;
											if (match(t)) {
												if (stmt()) {
													t->type = kw_od;
													return (match(t)); 
												}
											}
										}
									}
								}
							}
						}
					}
					else {
						t->type = kw_read;
						if (match(t)) {
							t->type = lx_lparen;
							if (match(t)) {
								t->type = lx_identifier;
								if(match(t)) {
									t->type = lx_rparen;
									return (match(t));
								}
							}
						}

						else {
							t->type = kw_write;
							if (match(t)) {
								t->type = lx_lparen;
								if (match(t)) {
									t->type = lx_identifier;
									if (match(t)) {
										t->type = lx_rparen;
										return (match(t)); 
									}
								}
							}
							else {
								t->type = kw_return;
								if (match(t)) {
									t->type = lx_lparen;
									if (match(t)) {
										if (expr()) {
											t->type = lx_rparen;
											return (match(t));
										}
									}
								}
								else {
									return (block());
								}
							}
						}
					}
				}
			
		}
	}
	return false;
}
bool parser::Z() {
	TOKEN  * t = new TOKEN();
	t->type = kw_fi;
	if (match(t)) {
		return true;
	}
	else {
		t->type = kw_else;
		if (match(t)) {
			if (stmt()) {
				t->type = kw_fi;
				return (match(t));
			}
		}
	}
	return false;
}
bool parser::Y() {
	TOKEN * t = new TOKEN();
	t->type = Ix_colon_eq;
	if (match(t)) {
		return expr();
	}
	else if(argList()){
		return true;
	}
	return false;
}
bool parser::block() {
	TOKEN *t = new TOKEN();
	t->type = kw_begin;
	if (match(t)) {
		//enters new scope
		if (varDeclList()) {
			if (stmtList()) {
				t->type = kw_end;

				return (match(t));///exit scope
			}
		}
	}
	return false;
}
bool parser::varDeclList() { 
	TOKEN *t = new TOKEN();
	if (varDecl()) {
		t->type = lx_semicolon;
		if (match(t)) {
			return varDeclList();	
		}
		else return false;
	}
	return true;
	
}
bool parser::varDecl() { 
	TOKEN *t = new TOKEN();
	t->type = kw_var;
	if (match(t)) {
		t->type = lx_identifier;
		if (match(t)) {
			t->type = Ix_colon;
			return (type());//store in symbole table
			
		}
	}
	return false;
}
bool parser::stmtList() { 
	TOKEN *t = new TOKEN();
	if (stmt()) {
		t->type = lx_semicolon;
		if (match(t)) {
			return stmtList();
			
		}
		else return false;
	}
	 return true;
	
}
bool parser::argList() { 
	TOKEN *t = new TOKEN();
	t->type = lx_lparen;
	if (match(t)) {
		return A();
	}
	return false;
}
bool parser::A() {
	TOKEN *t = new TOKEN();
	t->type = lx_rparen;

	if (match(t)) 
		return true;
	else {
		if (args()) {
			if (match(t))
				return true;
			else 
				return false;
		}
		else 
			return false;
	}

}
AST* parser::expr() {
	AST* fromD = new AST();
	fromD = D();
	if (fromD) {
		AST* fromExprBar = new AST();
		fromExprBar = exprbar();
		if (fromExprBar) {
			if (fromExprBar->type) {
				//make fromD the larg of EXprBAR ast*
			}
			else {
				return fromD;
			}
		}
	}
	return NULL;
}
AST* parser::exprbar() {
	TOKEN * fromRelConj = new TOKEN();
	fromRelConj = relConj();
	if (fromRelConj) {
		if (D()) {
			return exprbar();			
		}
		else return NULL;
	}

	return new AST();

}
AST* parser::D() {
	AST * fromF = new AST();
	fromF = F();
	if (fromF) {
		AST * fromDbar = new AST();
		fromDbar = Dbar();
		if (fromDbar) {
			if (fromDbar->type != NULL) {//if not lambda make fromF larg in Dbar

			}
			else {
				return fromF;
			}
		}
		else {
			return NULL;
		}
	}
	return NULL;
}
AST* parser::F() {
	AST * temp = new AST();
	temp = G();
	if (temp) {
		AST * fromFbar = new AST();
		fromFbar = Fbar();
		if (fromFbar) {
			if (fromFbar->type != NULL) {
				fromFbar->f.a_binary_op.larg = temp; //making left of plus_minus = the thing came from G();
				return fromFbar;
			}
			else {
				return temp;
			}
		}
		//return Fbar();
	}
	else
		return NULL;
}
AST* parser::Dbar() {
	if (relOp()) {
		if (F()) {
			return Dbar();
		}
		else return NULL;
	}
	return new AST();

}
AST* parser::G() {
	AST* temp = new AST();
	temp = H();
	if (temp) {
		//H brought the first integer
		AST * fromGbar = new AST();
		fromGbar = Gbar();
		if (fromGbar) {
			if (fromGbar->type != NULL) {
				fromGbar->f.a_binary_op.larg = temp; //making left of plus_minus = the thing came from G();
				return fromGbar;
			}
			else return temp; // return integer came from I() from H() ie Gbar() returns lambda
		}

	}
	else return temp;
}
AST* parser::Fbar() {
	TOKEN* t = new TOKEN();
	t = plus_minus();
	if (t) {
		AST* fromG = new AST();
		fromG = G();
		if (fromG) {
			
			//pmNode = make_ast_node(t->type == lx_plus ? ast_plus : ast_minus, new AST(), fromG);
			AST * fromFbar = new AST();
			fromFbar = Fbar();
			if (fromFbar) {
				if (fromFbar->type != NULL) {
					fromFbar->f.a_binary_op.larg = fromG; //making left of plus_minus = the thing came from G();
					//
					return make_ast_node(t->type == lx_plus ? ast_plus : ast_minus, new AST(), fromFbar);
				}
				else {//lambda from FBar();
					AST *pmNode = new AST();
					pmNode = make_ast_node(t->type == lx_plus?ast_plus:ast_minus,new AST(),fromG);
					return pmNode;
				}
			}
		}
		else return NULL;
	}
	return new AST();
	
}
AST* parser::H() {
	if (uniaryOp()) {
		return H();
	}
	else return I();
	
}
AST* parser::I() {
	TOKEN *t = new TOKEN();
	AST* temp = new AST();
	t->type = lx_identifier;
	if (match(t)) {
		return J();
	}
	else {
		t->type = lx_integer;
		if (match(t)) {
			temp = make_ast_node(ast_integer,tokens->at(index-1)->value);
			return temp;
		}
		else {
			t->type = lx_string;
			if (match(t)) {
				temp = make_ast_node(ast_integer, tokens->at(index - 1)->str_ptr);
				return temp;
			}
			else {
				t->type = kw_true;
				if (match(t)) {
					return temp;
				}
				else {
					t->type = kw_false;
					if (match(t)) {
						return temp;
					}
					else {
						t->type = lx_lparen;
						if (match(t)) {
							if (expr()) {
								t->type = lx_rparen;
								if(match(t))
									return temp;
							}
						}
					}
				}
			}
		}
	}
	return false;
}
AST* parser::J() {
	AST* x = new AST();
	//x = argList();
	
	return x;
	
}
AST* parser::Gbar() {
	TOKEN* t = new TOKEN();
	t = star_divide();
	if(t){
		AST* fromH = new AST();
		fromH = H();
		if (fromH) {
			AST * fromGbar = new AST();
			fromGbar = Gbar();
			if (fromGbar) {
				if (fromGbar->type != NULL) {
					fromGbar->f.a_binary_op.larg = fromH; //making left of plus_minus = the thing came from G();
														  //
					return make_ast_node(t->type == lx_star ? ast_times : ast_divide, new AST(), fromGbar);
				}
				else {//lambda from FBar();
					AST *pmNode = new AST();
					pmNode = make_ast_node(t->type == lx_star ? ast_times : ast_divide, new AST(), fromH);
					return pmNode;
				}
			}
		}
	}
	if (star_divide()) {
		if (H()) {
			return Gbar();
		}
		return NULL;
	}
	return new AST();
	
}
bool parser::args() { 
	if (expr()) {
		return B();
	}
	return true;
}
bool parser::B() {
	TOKEN *t = new TOKEN();
	t->type = lx_comma;
	if (match(t)) {
		if (args())
			return true;
		else 
			return false;
	}	
	return true;
}
TOKEN* parser::relConj(){ 
	TOKEN *t = new TOKEN();
	t->type = kw_and;
	if (match(t))return t;
	else {
		t->type = kw_or;
		if (match(t))return t;
	}
	return false;
}
bool parser::relOp(){
	TOKEN *t = new TOKEN();
	t->type = lx_eq;
	if (match(t))return true;
	else {
		t->type = lx_neq;
		if (match(t))return true;
		else {
			t->type = lx_lt;
			if (match(t))return true;
			else {
				t->type = lx_le;
				if (match(t))return true;
				else {
					t->type = lx_gt;
					if (match(t))return true;
				}
			}
		}
	}
	return false;
}
bool parser::uniaryOp() {
	TOKEN *t = new TOKEN();
	t->type = kw_not;

	if (match(t))return true;
	else {
		t->type = lx_minus;
		if (match(t))return true;
	}
	return false;
}
TOKEN* parser::star_divide(){
	TOKEN *t = new TOKEN();
	t->type = lx_star;

	if (match(t))return t;
	else {
		t->type = lx_slash;
		if (match(t))return t;
	}
	return false;
}
TOKEN *parser::plus_minus(){
	TOKEN *t = new TOKEN();
	t->type = lx_plus;

	if (match(t))return t;
	else {
		t->type = lx_minus;
		if (match(t))return t;
	}
	return false;
}
bool parser::match(TOKEN *t) {
	if(index<tokens->size())
	if (tokens->at(index)->type == t->type /*&& tokens->at(index)->str_ptr == t->str_ptr 
		&& tokens->at(index)->float_value == t->float_value && tokens->at(index)->value == t->value*/) {
		index++;
		return true;
	}
		
	return false;
}
void parser::cleanUpVector() {
	for (int i = 0; i < tokens->size(); i++) {
		if (tokens->at(i)->type == lx_identifier && tokens->at(i)->str_ptr == NULL)
			tokens->erase(tokens->begin() + i);
	}
}