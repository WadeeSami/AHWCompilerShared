#pragma once
#include "scanner.h"
#include <list>
#include <string>
#include <iostream>
#include <vector>
using namespace std;



struct Element {
	int id;
	int scopeId;
	char* name;
	TOKEN token;

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

