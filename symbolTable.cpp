#include "symbolTable.h"
#include "scanner.h"
#include <iostream>
using namespace std;

symbolTable::symbolTable() {
	currScopeId = 0;
	levelsCount = 0;
	elementsCount = 0;
	scopesCount = 1;
	currScope = new Head();
	ST = new Head();
	currScope = ST;
	initHead();
}
bool symbolTable::checkIfEqualst(char* first, char* second) {

	if (!strcmp(first, second))
		return true;
	return false;
}
Element* symbolTable::searchInCurrScope(char* s, vector<Element*> *v) {
	int length = v->size();
	for (int i = 0; i < length; i++) {
		if (checkIfEqualst(v->at(i)->name, s))
			return (v->at(i));
	}
	return (new Element());
}
void symbolTable::initHead() {
	currScope->id = 0;
	currScope->parentId = NULL;
	currScope->elements = new vector<Element*>;
	currScope->scopes = new vector<Head*>;
}
void symbolTable::printString(char *s) {
	for (int i = 0; i < strlen(s); i++)cout << s[i];
}
void symbolTable::printElement(Element *e) {
	cout << endl << "id: \t" << e->id << endl << "scopeID:" << e->scopeId << endl << "Name: \t";
	printString(e->name);
	cout << "\n*****************\n";
}
void symbolTable::printElementsInScope(Head *h)
{

	int length = h->elements->size();
	cout << "# of elements in scope " << length;
	for (int i = 0; i < length; i++) {
		cout << "E" << i << ":";
		printElement(h->elements->at(i));
	}
}
void symbolTable::initScope(Head *currScope) {
	currScope->elements = new vector<Element*>;
	currScope->scopes = new vector<Head*>;
}
void symbolTable::insertElement(char* idName, TOKEN token) {
	Element *temp = makeElement(idName, token);
	cout << currScope->id << endl;
	currScope->elements->push_back(temp);
}
TOKEN symbolTable::lookUp(char* idName) {
	Head * temp = new Head();
	Head * tempParent = new Head();
	temp = this->currScope;
	tempParent = temp->parentScope;
	int length = temp->elements->size();
	vector <Element*> *currElements = new vector<Element*>;
	currElements = temp->elements;
	Element *e;
	for (int i = 0; i < levelsCount + 1; i++) {
		e = searchInCurrScope(idName, temp->elements);
		if (e->name) {
			printElement(e);
			break;
		}
		else {
			temp = tempParent;
			tempParent = tempParent->parentScope;
		}

	}
	return *(new TOKEN());
}
void symbolTable::enterScope() {
	//cout << &currScope;
	//printElementsInScope(currScope);
	downAnotherLevel();
	scopesCount++;
	Head *temp = new Head();
	currScope->scopes->push_back(temp);
	temp->parentScope = new Head();
	temp->parentScope = this->currScope;	// my parent scope is the current scope
	temp->id = scopesCount - 1;
	this->currScope = temp;				//current scope is the newly created one
	initScope(temp);
}
void symbolTable::outScope() {
	upAnotherLevel();
	this->currScope->elements = this->currScope->parentScope->elements;
	this->currScope->scopes = this->currScope->parentScope->scopes;
	this->currScope = this->currScope->parentScope;
}
void symbolTable::downAnotherLevel() {
	levelsCount++;
}
void symbolTable::upAnotherLevel() {
	levelsCount--;
}
Element* symbolTable::makeElement(char *idName, TOKEN token) {
	Element *e = new Element();
	e->id = ++this->elementsCount;
	e->name = idName;
	if (currScope->id == 0) {
		e->scopeId = 0;
	}
	else {
		e->scopeId = this->currScope->id;
	}
	e->token = token;

	return e;
}