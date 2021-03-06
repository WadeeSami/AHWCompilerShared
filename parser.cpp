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
	symanicChecker = new sc(this->st);
	program = new ast_list_cell();
	myS = new SCANNER(this->fd);
	scanAllTokens();
	cleanUpVector();

	//AST* a = new AST();
	ast_list* a = new ast_list();
	a = declList();
	this->symanicChecker->symanticallyCheck(a);
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
			Element * newId = st->makeElement(tokens->at(index - 1)->str_ptr, *tokens->at(index - 1));
			newId->name = tokens->at(index - 1)->str_ptr;
			newId->entry_type = ste_var;
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
					e->entry_type = ste_const;
					AST * fromExpr = new AST();
					fromExpr = expr();
					if (fromExpr) {
						int evalued = eval_ast_expr(fd, fromExpr);
						e->f.constant.value = evalued;
						st->insertElement(e);
						ast = make_ast_node(ast_const_decl,	e, evalued);
						return ast;
					}
				}
			}
		}
		else {
			t->type = kw_function;
			TOKEN * routineReturnType = new TOKEN();
			if (match(t)) {
				t->type = lx_identifier;
				if (match(t)) {
					TOKEN * tempFunctionToken = tokens->at(index - 1);


					ast_list* fromFormalList = new ast_list();
					fromFormalList = formalList();
					if (fromFormalList) {
						t->type = Ix_colon;
						if (match(t)) {
							routineReturnType = type();
							if (routineReturnType) {
								
								//create an element
								Element* e = new Element();
								e = st->makeElement(tempFunctionToken->str_ptr, *tempFunctionToken);
								e->entry_type = ste_routine;
								e->f.routine.result_type = j_type(this->findJTypeFromToken(routineReturnType->type));
								//TODO: fix this shit
								e->f.routine.formals = this->convertASTListToElemenntList(fromFormalList);
								if (!st->insertElement(e)) {
									//already defined
									return NULL;
								}
								
								///////

								AST* ast = new AST();
								AST* fromBlock = new AST();
								fromBlock = block();
								if (fromBlock) {//the body
									
									ast = make_ast_node(ast_routine_decl,
										e, this->convertASTListToElemenntList(fromFormalList),
										this->findJTypeFromToken(routineReturnType->type),
										fromBlock
									);
									return ast;
								}
								
								
							}
						}
						else {
							return NULL;
						}
					}
				}
			}
			else {
				t->type = kw_procedure;
				if (match(t)) {
					t->type = lx_identifier;
					if (match(t)) {
						//create an element
						Element* e = new Element();
						e = st->makeElement(tokens->at(index - 1)->str_ptr, *tokens->at(index - 1));
						if (!st->insertElement(e)) {
							//already defined
							return NULL;
						}
						ast_list* fromFormalList = new ast_list();
						fromFormalList = formalList();
						if (fromFormalList) {
									AST* ast = new AST();
									AST* fromBlock = new AST();
									fromBlock = block();
									if (fromBlock) {//the body
										ast = make_ast_node(ast_routine_decl,
											e, fromFormalList,
											type_none,//this is a procedure, ot should return nithing
											fromBlock
										);
										return ast;
									}


								
							
							
						}
					}
				}
			}



		}
	}
	
	return NULL;
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
ast_list* parser::formalList() { 
	TOKEN *t = new TOKEN();
	t->type = lx_lparen;
	if (match(t)) {
		ast_list *fromX = new ast_list();
		fromX = X();
		if (fromX){
			return fromX;
		}
		else {
			return NULL;
		}
	}
	else {
		cout << "Syntax Error, missing Left paren at line " << this->fd->GetLineNum() <<endl;
		return NULL;
	}
}
ast_list* parser::X() {
	TOKEN *t = new TOKEN();
	ast_list * fromFormals = new ast_list();
	fromFormals = formals();
	if (fromFormals) {
		t->type = lx_rparen;
		if (match(t)) {
			return fromFormals;
		}
		else { 
			cout << "Syntax Error, missing right paren at line " << this->fd->GetLineNum() << endl;
			return NULL; 
		}
	}
	else {
		t->type = lx_rparen;
		if (match(t))return new ast_list();
		else return NULL;
	}
	return NULL;
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
				eOfFunc->f.var.type = j_type(findJTypeFromToken(t->type));
				listOfFormals->head = make_ast_node(ast_var, eOfFunc);
				ast_list *fromFormalsBar = new ast_list();
				fromFormalsBar = formalsBar();
				if (fromFormalsBar) {
					if (fromFormalsBar->head) {
						listOfFormals->tail = fromFormalsBar;
						return listOfFormals;
					}
					else {
						listOfFormals->tail = NULL;
						return listOfFormals;
					}
				}
				else return NULL;
			}
			else {
				cout << "Syntax Error, missing type at line " << this->fd->GetLineNum() << endl;
			}
		}
		else {
			cout << "Syntax Error, missing colon at line " << this->fd->GetLineNum() << endl;
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
					AST* res = new AST();
					res = make_ast_node(ast_var_decl, eOfFunc, findJTypeFromToken(tt->type));
					listOfFormals->head = res;
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
AST* parser::stmt() { 
	Element * eTest = new Element();

	/*simulate a var in symbol tabe;
	eTest->name = "id3";
	eTest->token = *(new TOKEN());
	eTest->token.value = 3;
	eTest->token.str_ptr = "id3";
	st->insertElement(eTest);*/
	TOKEN *t = new TOKEN();
	t->type = lx_identifier;
	if (match(t)) {

		Element * eTest = new Element();
		
		/*simulate a var in symbol tabe;
		eTest->name = "id3";
		eTest->token = *(new TOKEN());
		eTest->token.value = 3;
		eTest->token.str_ptr = "id3";
		st->insertElement(eTest);*/
		TOKEN* tt = new TOKEN();
		eTest = st->lookUp(tokens->at(index - 1)->str_ptr);
		Element * e = new Element();
		if (!eTest) {
			//this is not null
			cout << "The variable " << tokens->at(index - 1)->str_ptr << " Is not defined, This error is at line "<<this->fd->GetLineNum() << endl;
			return NULL;
		}
		
		AST * fromY = new AST();
		fromY = Y();
		if (fromY) {
			if (fromY->type == ast_call) {//function invocation
				//get function's return type
				cout << "helloo" << endl;
				fromY->f.a_call.callee = eTest;
				return fromY;
			}
			else {
				//if (! this->isSameType(eTest, fromY)) {
				//	cout << "Symantic Error, 2  different types "<<eTest->name << " is not the same type  "<< endl;
				//}
				////if(eTest->f.var.type == )
				//if (fromY->type == ast_integer) {
				//	//update the symbol table entry
				//	eTest->token.value = eval_ast_expr(fd, fromY);

				//}
				//else if (fromY->type == ast_string) {
				//	eTest->token.stringValue = fromY->f.a_string.str_value;

				//}
				//TODO: think of making new AST with the evaluated expression stored inside
				//this is an assignment node, assign the left argument
				int result = eval_ast_expr(this->fd, fromY);
				if (result != ERROR_EVAL_EXPR) {
					eTest->token.value = result;
				}
				else if (fromY->type == ast_string) {
					eTest->token.stringValue = fromY->f.a_string.str_value;
				}
				else {
					//error
				}

				fromY->f.a_assign.lhs = eTest;
				return fromY;
				//return make_ast_node(ast_assign, eTest, fromY);
			}
		}
	}
	else {
		t->type = kw_if;
		if (match(t)) {
			AST* fromExpr = new AST();
			fromExpr = expr();
			if (fromExpr) {
				t->type = kw_then;
				if (match(t)) {
					AST* fromStmt = new AST();
					fromStmt = stmt();
					if (fromStmt) {
						AST* fromZ = new AST();
						fromZ = Z();
						if (fromZ) {
							return make_ast_node(ast_if, fromExpr, fromStmt, fromZ);
						}
						else {
							return NULL;//if Z is NULL
						}
					}
					else {
						return NULL;//if stmt is NULL
					}
				}
				else {
					return NULL; //match failed
				}
			}
			else {
				return NULL;//fromExpr is null
			}
		}

		else {
			t->type = kw_while;
			if (match(t)) {
				AST* fromExpr = new AST();
				fromExpr = expr();
				if (fromExpr) {
					t->type = kw_do;
					if (match(t)) {
						AST* fromStmt = new AST();
						fromStmt = stmt();
						if (fromStmt) {
							t->type = kw_od;
							if (match(t)) {
								return make_ast_node(ast_while, fromExpr, fromStmt);
							}
							else {
								return NULL;//if Z is NULL
							}
						}
						else {
							return NULL;//if stmt is NULL
						}
					}
					else {
						return NULL; //match failed
					}
				}
				else {
					return NULL;//fromExpr is null
				}
			}
			else {
				//match if failed
				t->type = kw_for;
				if (match(t)) {
					t->type = lx_identifier;
					if (match(t)) {
						Element *e = new Element();
						e = st->lookUp(tokens->at(index - 1)->str_ptr);
						t->type = Ix_colon_eq;
						if (match(t)) {
							AST * fromExpr1 = new AST();
							fromExpr1 = expr();
							if (fromExpr1) {
								t->type = kw_to;
								if (match(t)) {
									AST * fromExpr2 = new AST();
									fromExpr2 = expr();
									if (fromExpr2) {
										t->type = kw_do;
										if (match(t)) {
											AST * fromStmt = new AST();
											fromStmt = stmt();
											if (fromStmt) {
												t->type = kw_od;
												if (match(t)) {
													e->token.value = eval_ast_expr(fd, fromExpr1);
													return make_ast_node(ast_for, e, fromExpr1, fromExpr2, fromStmt);
												}
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
							if (match(t)) {
								Element *e = new Element();
								e = st->lookUp(tokens->at(index - 1)->str_ptr);
								t->type = lx_rparen;
								if (match(t)) {
									return make_ast_node(ast_read, e);
								}
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
									Element *e = new Element();
									e = st->lookUp(tokens->at(index - 1)->str_ptr);
									t->type = lx_rparen;
									if (match(t)) {
										return make_ast_node(ast_write, e);
									}
								}
							}
						}
						else {
							t->type = kw_return;
							if (match(t)) {
								t->type = lx_lparen;
								if (match(t)) {
									AST* fromExpr = new AST();
									fromExpr = expr();
									if (fromExpr) {
										t->type = lx_rparen;
										if (match(t)) {
											//return make_ast_node(ast_return, eval_ast_expr(fd, fromExpr));
											return make_ast_node(ast_return, fromExpr);
										}
										else {
											cout << "syntax error , missing right paren in return statement " << endl;
										}
									}
								}
								else {
									cout << "syntax error , missing left paren in return statement " << endl;
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
	return NULL;
}
AST* parser::Z() {

	TOKEN  * t = new TOKEN();
	t->type = kw_fi;
	if (match(t)) {
		return new AST();
	}
	else {
		t->type = kw_else;
		if (match(t)) {
			AST* fromStmt = new AST();
			fromStmt = stmt();
			if (fromStmt) {
				t->type = kw_fi;
				if (match(t)) {
					return fromStmt;
				}
			}
			else {
				return NULL;
			}
		}
	}
	return false;
}
AST* parser::Y() {
	TOKEN * t = new TOKEN();
	t->type = Ix_colon_eq;
	if (match(t)) {
		AST* fromExpr = new AST();
		fromExpr = expr();
		if (fromExpr) {
			//assignment node
			return make_ast_node(ast_assign, new AST(), fromExpr);
		}
		else return NULL;
	}
	ast_list * fromArgList = new ast_list();
	fromArgList = argList();
	if (fromArgList) {
		AST* toStmt = new AST();
		return make_ast_node(ast_call, new Element(), fromArgList);
	}
	cout << "Syntax error , either := is missing, or error in function invokation at line " << this->fd->GetLineNum() <<endl;
	return NULL;
}
AST* parser::block() {
	TOKEN *t = new TOKEN();
	AST* blockNode = new AST();
	element_list * elementList = new element_list();
	t->type = kw_begin;
	if (match(t)) {
		//enters new scope
		st->enterScope();
		ast_list * fromVarDL = new ast_list();
		fromVarDL = varDeclList();
		if (fromVarDL) {
			if (fromVarDL->head) {
				//convert ast_list to element_list
				elementList = this->convertASTListToElemenntList(fromVarDL);

			}
		}
			ast_list * fromStmtList = new ast_list();
			fromStmtList = stmtList();
			if (fromStmtList) {
				t->type = kw_end;
				if (match(t)) {
					blockNode =  make_ast_node(ast_block, elementList,fromStmtList);
					st->outScope();
					return blockNode;
				}
			}
		
	}
	return NULL;
}
ast_list* parser::varDeclList() { 
	TOKEN *t = new TOKEN();
	ast_list * parentList = new ast_list();
	AST* fromVarDec = new AST();
	fromVarDec = varDecl();
	if (fromVarDec) {
		parentList->head = fromVarDec;
		t->type = lx_semicolon;
		if (match(t)) {
			ast_list* fromDesclList = new ast_list();
			fromDesclList = varDeclList();
			if (fromDesclList) {
				parentList->tail = fromDesclList;
			}
			else {
				parentList->tail = NULL;
			}
			return parentList;
		}
		else {
			return NULL;
		}
	}
	else {
		return NULL;
	}
	return new ast_list();	
}
AST* parser::varDecl() { 
	TOKEN *t = new TOKEN();
	t->type = kw_var;
	if (match(t)) {
		t->type = lx_identifier;
		if (match(t)) { 
			t->type = Ix_colon;
			Element *e = new Element();
			e = st->makeElement(tokens->at(index-1)->str_ptr, *tokens->at(index - 1));
			e->entry_type = ste_var;

			if(match(t)){
				TOKEN * t = new TOKEN();
				t = type();
				//store in symbole table
				if (t) {
					e->entry_type = ste_var;
					e->f.var.type = j_type(findJTypeFromToken(t->type));
					if (!st->insertElement(e)) {
						//already defined
						return NULL;
					}
					return make_ast_node(ast_var,e);
				}
				else return NULL;
			}
			else {
				cout << "missing colon at line " << this->fd->GetLineNum()<<endl;
			}
		}
		else {
			cout << "missing identifier at line " << endl;
		}
	}
	//else {
		//cout << "missing keyword 'var' at line " << this->fd->GetLineNum() <<endl;
	//}
	return NULL;
}
ast_list* parser::stmtList() { 
	TOKEN *t = new TOKEN();
	AST* fromStmt = new AST();
	ast_list * parentStmtList = new ast_list();
	fromStmt = stmt();
	if (fromStmt) {
		parentStmtList->head = fromStmt;
		t->type = lx_semicolon;
		if (match(t)) {
			ast_list * fromStmtList = new ast_list();
			fromStmtList = stmtList();
			if (fromStmtList) {
				if (fromStmtList->head) {
					parentStmtList->tail = fromStmtList;
				}
				else {
					parentStmtList->tail = NULL;
					
				}

				return parentStmtList;
			}
		}
		else {
			cout << "missing semicolon at line " << this->fd->GetLineNum() << endl;
			return NULL;
		}
	}
	 return new ast_list();
	
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
				return make_ast_node(fromFbar->type, fromFbar->f.a_binary_op.larg, fromFbar->f.a_binary_op.rarg);
				//return fromFbar;
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
		tempE = this->st->lookUp(tokens->at(index - 1)->str_ptr);
		if (!tempE) {
			cout << "Error, invoking undefined variable or function at line" << this->fd->GetLineNum() << endl;
			return NULL;
		}
		//tempE =	st->makeElement(tokens->at(index - 1)->str_ptr, *tokens->at(index - 1));
		fromJ = J();
		if (fromJ) {
			if (tempE->entry_type != ste_routine) {
				cout << "calling undefined function " << endl;
			}
			return make_ast_node(ast_call, tempE, fromJ);
		}
		else {
			//this is not a function, it's a typical variable
			if (tempE->entry_type == ste_var || tempE->entry_type == ste_const) {
				return make_ast_node(ast_var, tempE);
			}
			else {
				cout << "calling undefined variable " << endl;
			}
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
				temp = make_ast_node(ast_string, tokens->at(index - 1)->str_ptr);
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

bool parser::isSameType(Element * element, AST * ast)
{
	//this method compares if 2 types are the same
	/*
	ast might be a function, a variable or an expression or a simple a literal
	*/
	if (element->entry_type == ste_var) {
		//check ast
		if (this->convertASTTypeToElementType(ast) == element->f.var.type) {
			return true;
		}
		else {
			return false;
		}

	}
	return false;
}

int parser::convertASTTypeToElementType(AST * a)
{
	if (a->type == ast_integer) {
		return type_integer;
	}
	else if (a->type == ast_string) {
		return type_string;
	}
	else if (a->type == ast_boolean) {
		return type_boolean;
	}
	else if (a->type == ast_float) {
		return type_float;
	}
	return type_none;
}

element_list * parser::convertASTListToElemenntList(ast_list * list)
{
	element_list* elementList = new element_list();
	element_list* head = new element_list();
	head = elementList;
	while (list && list->head) {
		elementList->head = list->head->f.a_var.var;
		elementList->tail = new element_list();
		elementList = elementList->tail;
		list = list->tail;
	}
	return head;
}
