#include "parser.h"

parser::parser() {
	tokens = new vector <TOKEN*>;
	index = 0;
}

void parser::parse(FileDescriptor *fd, symbolTable *st) {
	this->fd = fd;
	this->st = st;
	myS = new SCANNER(this->fd);
	scanAllTokens();
	cleanUpVector();
	if (!declList())
		cout << "ERRRR";
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

bool parser::declList() { 
	TOKEN *t = new TOKEN();
	if (decl()) {
		t->type = lx_semicolon;
		if (match(t)) {
			return declList();
			
		}
		else return false;
	}	
	return true;
}
bool parser::decl() { 
	TOKEN *t = new TOKEN();
	t->type = kw_var;
	if (match(t)) {
		t->type = lx_identifier;
		if (match(t)) {
			t->type = Ix_colon;
			if (match(t)) {
				if (type()) {
					//cout << "noErrors";
					//add to symbolTable new entry 
					return true;
				}
				else {
					cout << "errors in type in line "<<fd->GetLineNum();
					return false;
				}
			}
			else {
				cout << "error: no colon in line " << fd->GetLineNum();
				return false;
			}
		}
		else {
			cout << "no identefier found in line "<<fd->GetLineNum();
			return false;
		}
	}
	else{
		t->type = kw_constant;
		if (match(t)) {
			t->type = lx_identifier;
			if (match(t)) {
				t->type = lx_eq;
				if (match(t)) {
					if (expr()) return true;
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
								if (block()) return true;
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
							if (block()) return true;	
						}
					}
				}
			}
		}
	}
	
	return false;
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
bool parser::expr() { 
	if (D()) {
		return exprbar();
	}
	return false;
}
bool parser::exprbar() {
	if (relConj()) {
		if (D()) {
			return exprbar();			
		}
		else return false;
	}

	return true;

}
bool parser::D() {
	if (F()) 
		if (Dbar())
			return true;
	return false;
}
bool parser::F() {
	if (G())
		return Fbar();
	else 
		return false;
}
bool parser::Dbar() {
	if (relOp()) {
		if (F()) {
			return Dbar();
		}
		else return false;
	}
	return true;

}
bool parser::G() {
	if (H())
		return Gbar();

	else return false;
}
bool parser::Fbar() {
	if (plus_minus()) {
		if (G()) {
			return Fbar();
		}
		else return false;
	}
	return true;
	
}
bool parser::H() {
	if (uniaryOp()) {
		return H();
	}
	else return I();
	
}
bool parser::I() {
	TOKEN *t = new TOKEN();
	t->type = lx_identifier;
	if (match(t)) {
		return J();
	}
	else {
		t->type = lx_integer;
		if (match(t)) {
			return true;
		}
		else {
			t->type = lx_string;
			if (match(t)) {
				return true;
			}
			else {
				t->type = kw_true;
				if (match(t)) {
					return true;
				}
				else {
					t->type = kw_false;
					if (match(t)) {
						return true;
					}
					else {
						t->type = lx_lparen;
						if (match(t)) {
							if (expr()) {
								t->type = lx_rparen;
								if(match(t))
									return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}
bool parser::J() {
	
	return argList();
	
}
bool parser::Gbar() {
	if (star_divide()) {
		if (H()) {
			return Gbar();
		}
		return false;
	}
	return true;
	
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
bool parser::relConj(){ 
	TOKEN *t = new TOKEN();
	t->type = kw_and;
	if (match(t))return true;
	else {
		t->type = kw_or;
		if (match(t))return true;
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
bool parser::star_divide(){ 
	TOKEN *t = new TOKEN();
	t->type = lx_star;

	if (match(t))return true;
	else {
		t->type = lx_slash;
		if (match(t))return true;
	}
	return false;
}
bool parser::plus_minus(){
	TOKEN *t = new TOKEN();
	t->type = lx_plus;

	if (match(t))return true;
	else {
		t->type = lx_minus;
		if (match(t))return true;
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