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

	ast_list* a = new ast_list();
	a = formals();
	cout << 'a';
	//cout<<eval_ast_expr(fd,expr());
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
	if (tempDeclNode != NULL) {
		t->type = lx_semicolon;
		if (match(t)) {
			temp->head = NULL;
			temp->head = tempDeclNode;
			temp->head->type = tempDeclNode->type;
			cout << temp->head->type;
			ast_list * fromDL = new ast_list();
			fromDL = declList();

			if (fromDL) {

				temp->tail = fromDL;
				return temp;
			}
			else {
				temp->tail = NULL;
				return temp;
			}
		}
		//else cout<<"Syntax Error\n";
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
					AST * fromExpr = new AST();
					fromExpr = expr();
					if (fromExpr) {
						int evalued = eval_ast_expr(fd, fromExpr);
						ast = make_ast_node(ast_const_decl,	e, evalued);
						return ast;
					}
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
TOKEN* parser::type() { 
	TOKEN *t = new TOKEN();
	t->type = kw_integer;
	if (match(t)) {
		return t;
	}
	else {
		t->type = kw_boolean;
		if (match(t)) {
			return t;
		}
		else {
			t->type = kw_string;
			if (match(t)) {
				return t;
			}
		}
	}
	return NULL;
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
ast_list* parser::formals() {
	TOKEN *t = new TOKEN();
	t->type = lx_identifier;
	if (match(t)) {
		Element * eOfFunc = new Element();
		eOfFunc = st->makeElement(tokens->at(index - 1)->str_ptr, *tokens->at(index - 1));
		ast_list * listOfFormals = new ast_list();
		listOfFormals->head = new AST();
		t->type = Ix_colon;
		if(match(t)){
			TOKEN *t = new TOKEN();
			t = type();
			if (t) {
				ast_list *fromFormalsBar = new ast_list();
				fromFormalsBar = formalsBar();
				if (fromFormalsBar) {
					if (fromFormalsBar->head) {
						listOfFormals->tail = fromFormalsBar;
						return listOfFormals;
					}
					else {
						return listOfFormals;
					}
				}
				else return NULL;
			}
		}
	}
	return false;
}
ast_list* parser::formalsBar() {
	TOKEN *t = new TOKEN();
	t->type = lx_comma;
	if (match(t)) {
		t->type = lx_identifier;
		if (match(t)) {
			Element * eOfFunc = new Element();
			eOfFunc = st->makeElement(tokens->at(index - 1)->str_ptr, *tokens->at(index - 1));
			ast_list * listOfFormals = new ast_list();
			listOfFormals->head = new AST();
			t->type = Ix_colon;
			if (match(t)) {
				TOKEN *tt = new TOKEN();
				tt = type();
				if (tt) {
					listOfFormals->head = make_ast_node(ast_var_decl, eOfFunc->name, findJTypeFromToken(tt->type));
					ast_list *fromFormalsList = new ast_list();
					fromFormalsList = formalsBar();
					if (fromFormalsList) {
						if (fromFormalsList->head) {
							listOfFormals->tail = fromFormalsList;
							return listOfFormals;
						}
						else {//lambda
							listOfFormals->tail = NULL;
							return listOfFormals;
						}
					}
					else {
						return NULL;
					}
				}
				else return NULL;

			}
			else return NULL;

		}
		else return NULL;

	}
	return new ast_list();
	
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
ast_list* parser::argList() { 
	TOKEN *t = new TOKEN();
	t->type = lx_lparen;
	AST* x = new AST();
	if (match(t)) {
		ast_list * fromA = new ast_list();
		fromA = A();
		return fromA;
	}
	return NULL;
}
ast_list* parser::A() {
	TOKEN *t = new TOKEN();
	t->type = lx_rparen;

	if (match(t))
		return new ast_list();
	else {
		ast_list * argsList = args();
		if (argsList) {
			if (match(t))
				return argsList;
			else 
				return NULL;
		}
		else 
			return NULL;
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
				fromExprBar->f.a_binary_op.larg = fromD;
				return fromExprBar;
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
		AST* fromD = new AST();
		fromD = D();
		if (fromD) {
			AST* fromExprBar = new AST();
			fromExprBar = exprbar();
			if (fromExprBar) {
				if (fromExprBar->type) {
					fromExprBar->f.a_binary_op.larg = fromD;
					return make_ast_node(fromRelConj->type == kw_or ? ast_cor : ast_cand, new AST(), fromExprBar);
				}
				else {
					return make_ast_node(fromRelConj->type == kw_or ? ast_cor : ast_cand, new AST(), fromD);
				}
			}
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
				fromDbar->f.a_binary_op.larg = fromF; //making left of plus_minus = the thing came from G();
				return fromDbar;
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
int parser::findRelOp(int x) {
	int relOper;
	if (x == 51)return 23;//lt
	else if (x == 53) return 25; // gt
	else if (x == 54)return 26;//ge
	else if (x == 50)return 21;//notEqual
	else if (x == 49)return 20;//equal
	else if (x == 52)return 24; //less and equal
}
int parser::findJTypeFromToken(int x) {
	if (x == 19)return 0;//int
	else if (x == 6) return 2; // boolean
	else if (x == 27)return 1;//string
}
AST* parser::Dbar() {


	TOKEN* t = new TOKEN();
	t = relOp();
	if (t) {
		AST* fromF = new AST();
		fromF = F();
		if (fromF) {
			AST* fromDbar = new AST();
			fromDbar = Dbar();
			if (fromDbar) {
				if (fromDbar->type) {
					fromDbar->f.a_binary_op.larg = fromF;
					return make_ast_node(findRelOp(t->type), new AST(), fromDbar);
				}
				else {
					AST *opNode = new AST();
					opNode = make_ast_node(findRelOp(t->type), new AST(), fromF);
					return opNode;
				}
			}
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
			if (fromGbar->type != 0 ) {
				fromGbar->f.a_binary_op.larg = temp; //making left of plus_minus = the thing came from G();
				return fromGbar;
			}
			else 
				return temp; // return integer came from I() from H() ie Gbar() returns lambda
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
	TOKEN *t = new TOKEN();
	t = uniaryOp();
	if (t) {
		AST* fromH = new AST();
		fromH = H();
		if (fromH) {
			return make_ast_node(t->type == kw_not ? ast_not : ast_uminus, fromH);
		}
		else {

		}
	}
	else {
		AST * fromI = new AST();
		fromI = I();
		return fromI;
	}
}
AST* parser::I() {
	TOKEN *t = new TOKEN();
	AST* temp = new AST();
	t->type = lx_identifier;
	if (match(t)) {
		ast_list *fromJ = new ast_list();
		Element *tempE = new Element();
		tempE =	st->makeElement(tokens->at(index - 1)->str_ptr, *tokens->at(index - 1));
		fromJ = J();
		if (fromJ) {
			return make_ast_node(ast_call, tempE, fromJ);
		}
		return new AST();// should return J()
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
					temp = make_ast_node(ast_boolean,1);
					return temp;
				}
				else {
					t->type = kw_false;
					if (match(t)) {
						temp = make_ast_node(ast_boolean, 0);
						return temp;
					}
					else {
						t->type = lx_lparen;
						if (match(t)) {
							AST * fromExpr = new AST();
							fromExpr = expr();
							if (fromExpr) {
								t->type = lx_rparen;
								if(match(t))
									return fromExpr;
							}
						}
					}
				}
			}
		}
	}
	return NULL;
}
ast_list* parser::J() {
	ast_list* fromArgList = new ast_list();
	fromArgList = argList();
	return fromArgList;
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
ast_list* parser::args() { 
	ast_list* list1 = new ast_list();
	AST* fromExpr = new AST();
	fromExpr = expr();
	if (fromExpr) {
		ast_list * fromB = new ast_list();
		fromB = B();
		AST *evaluedExpr = new AST();
		evaluedExpr->f.a_integer.value = eval_ast_expr(fd,fromExpr);
		list1->head = fromExpr;
		if (fromB) {
			if (fromB->head) {
				list1->tail = fromB;
			}
			else {
				//lambda
				list1->tail = NULL;
			}
			return list1;
		}
	}
	return NULL;
}
ast_list* parser::B() {
	TOKEN *t = new TOKEN();
	t->type = lx_comma;
	if (match(t)) {
		ast_list * list1 = new ast_list();
		list1 = args();
		if (list1) {
			if (list1->head) {
				return list1;
			}
			else {
				return new ast_list();
			}
		}
		else 
			return NULL;
	}	
	return new ast_list();
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
TOKEN* parser::relOp(){
	TOKEN *t = new TOKEN();
	t->type = lx_eq;
	if (match(t))return t;
	else {
		t->type = lx_neq;
		if (match(t))
			return t;
		else {
			t->type = lx_lt;
			if (match(t))return t;
			else {
				t->type = lx_le;
				if (match(t))return t;
				else {
					t->type = lx_gt;
					if (match(t))return t;
					else {
						t->type = lx_ge;
						if (match(t))return t;
					}
				}
			}
		}
	}
	return false;
}
TOKEN* parser::uniaryOp() {
	TOKEN *t = new TOKEN();
	t->type = kw_not;

	if (match(t))return t;
	else {
		t->type = lx_minus;
		if (match(t))return t;
	}
	return NULL;
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