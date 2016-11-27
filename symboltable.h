#pragma once
#include "scanner.h"
#include <list>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

typedef enum {

	type_integer,
	type_string,
	type_boolean,
	type_float,
	type_none

} j_type;

static char *type_names[] = { "integer", "string", "boolean", "float", "none" };
typedef enum {
	ste_var,     // a variable
	ste_const,    //A constant
	ste_routine,    //A routine
	ste_undefined,   // ste_entry
} element_type;
static char *ste_type_names[] = { "variable", "constant", "routine", "undefined" };

struct Element {
	int id;
	int scopeId;
	char* name;
	TOKEN token;
	element_type entry_type;

	union {
		struct {//l .for a variable record its type
			j_type type;
		} var;

		struct {// for a constant record its value
			int value;
		} constant;

		struct {// for a routine, record formal parameters and result type
			struct element_list_cell* formals;// will be defined later
			j_type result_type;
		} routine;
	} f;
};


struct Head {
	int id;
	int parentId;
	Head *parentScope;
	vector<Head*> *scopes;
	vector<Element*> *elements;
};

class symbolTable {
private:
	int elementsCount;
	int scopesCount;
	int currScopeId;
	int levelsCount;
	Head *ST;
	
public:
	void printElementsInScope(Head s);
	void initScope(Head * currScope);
	Head *currScope;
	Head *parentScope;
	//symbolTable(int x);
	symbolTable();
	bool checkIfEqualst(char * first, char * second);
	Element* searchInCurrScope(char * s, vector <Element*>*v);
	~symbolTable();
	void insertElement(char* idName, TOKEN token);
	TOKEN lookUp(char* idName);
	int parentScopeId(char* idName);
	void initHead();
	void printElement(Element e);
	void printElementsInScope(Head * h);
	void printElement(int i);
	void printElement(char * s);
	void printString(char * s);
	void printElement(Element * e);
	void initScope(Head scope);
	void enterScope();
	void outScope();
	void downAnotherLevel();
	void upAnotherLevel();
	Element * makeElement(char * idName, TOKEN token);
	Element* makeElement();
};

