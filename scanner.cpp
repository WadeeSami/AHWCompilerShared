
#include "scanner.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <vector>
using namespace std;

#define ALPHABET_SIZE (127-31)
#define CHAR_TO_INDEX(biggest) ((int)biggest - (int)' ')

int keys = 32; /* number of keywords */
char *keyword[] = {
	"and", "begin", "boolean", "by", "constant",
	"do", "else", "end", "false", "fi", "float", "for", "from",
	"function", "if", "integer", "not", "od", "or", "procedure",
	"program", "read", "return", "string", "then", "to", "true",
	"var", "while", "write"
};

char *keywordString = "and begin boolean by constant do else end false fi float for from function if integer not od or procedure program read return string then to true var while write";

LEXEME_TYPE key_type[] = {
	kw_and, kw_begin, kw_boolean, kw_by, kw_constant,
	kw_do, kw_else, kw_end, kw_false, kw_fi,kw_float,
	kw_for, kw_from, kw_function, kw_if, kw_integer, kw_not,
	kw_od, kw_or, kw_procedure, kw_program,kw_read, kw_return,
	kw_string, kw_then, kw_to, kw_true, kw_var, kw_while, kw_write
};

int op = 20; // number of operators
char * Operator[] = {
	"{","}","(",")","[","]",":",".", ";" ,",",":=","+","-",
	"*","/","=","!=","<","<=",
	">",">="
};


char op22 = '=';

char *operatorString = "{ } ( ) : . ; , := + - * / = != < <= > >=";

LEXEME_TYPE op_key[] = {
	lx_lbrace, lx_rbrace, lx_lparen, lx_rparen, lx_lbracket,
	lx_rbracket, Ix_colon, lx_dot, lx_semicolon, lx_comma,
	Ix_colon_eq, lx_plus, lx_minus, lx_star, lx_slash,
	lx_eq, lx_neq, lx_lt, lx_le, lx_gt,
	lx_ge, lx_eof
};

float SCANNER::string_to_float(char * Buffer, int size)
{
	double value = 0.0; bool AfterPoint = false;
	char currChar; int afterPointDigit = 0;
	for (int i = 0; i<size; i++)
	{
		currChar = Buffer[i];
		if (currChar == '.')
		{
			AfterPoint = true;
			afterPointDigit = 1;
		}

		else if (!AfterPoint)
			value = value * 10 + (double)(currChar - '0');
		else
		{
			double power = pow(10, -afterPointDigit);
			double data = currChar - '0';
			double answer = data * power;
			value += answer;
			afterPointDigit++;
		}
	}//for

	return (float)value;
	//cout << "string to float";
}

int SCANNER::string_to_int(char * Buffer, int size)
{
	int value = 0;
	for (int i = 0; i<size; i++)
		value = value * 10 + Buffer[i] - '0';
	return value;
}

bool SCANNER::skip_comments()
{
	char currChar = Fd->GetChar();
	bool b1 = true, b2 = false, e1 = false, e2 = false;
	//chech if it is a valid comment begins with ##
	if (currChar == '#')
	{
		b2 = true;
		while (currChar != '\0')
		{
			currChar = Fd->GetChar();
			if (currChar == '#')
			{
				e1 = true;
				char nextChar = Fd->GetChar();
				if (nextChar == '#') {
					e2 = true;
					return true;
					//1	break; 
				}
				else
					Fd->UngetChar();
			}//if
		}//while
	}//if
	if (b1&&!b2)
		return false;
	return true;
}

bool SCANNER::check_keyword(LEXEME_TYPE & LEX, char currChar)
{
	
	int index = 0;
	int counter = 0;
	char c = currChar;
	while (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
	{
		c = Fd->GetChar();
		counter++;
	}
	char * buffer = new char[counter];
	int counter1 = counter;
	while (counter--)Fd->UngetChar();
	c = currChar;
	while (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
	{
		buffer[index] = c;
		c = Fd->GetChar();
		index++;
	}
	buffer[index] = '\0';
	//cout << strlen(buffer);

	bool check = false;
	for (int i = 0; i < 30; i++)
	{
		check = checkIfEqual(keyword[i], buffer);
		if (check)
		{
			LEX = key_type[i];
			return check;
		}
	}

	for (int j = 0; j < index; j++)
		Fd->UngetChar();

	return false;
	//search(rootOfKeywords,);
	/*int index = 0,
	count = 0;
	bool unGet; // to identify when to unget chars


	while (index != 32 && index != -1)
	{
	unGet = false;

	while ( !unGet)
	{
	if (keyword[index][count]=='\0' && (currChar == ' ' || currChar == '\0' || currChar == '\n'))
	{
	unGet=false;
	break;
	}
	//equal means there's a match continue examining the same keyword
	if (currChar == keyword[index][count])
	{
	count++;
	currChar = Fd->GetChar();
	unGet = false;
	}

	//no match search for keywords in the upper array and unget the previous ones
	else if (currChar > keyword[index][count])
	{

	unGet = true;
	index++;
	break;
	}
	//no match search for keywords in the lower array and unget the previous ones
	else
	{

	unGet = true;
	index = -1;
	break;
	}

	}//while

	// the token matched a keyword
	if (!unGet)
	{
	LEX = key_type[index];
	return true;
	}

	// return the file pointer to its original place before entering this function
	else
	{
	for (int j = 0; j < count; j++)
	Fd->UngetChar();
	count = 0;

	}

	}//while

	return false;
	*/
	return false;
}

TOKEN * SCANNER::get_id(bool & isValid)
{
	int index = 0;
	char Buffer[4086];
	char *idName = new char[30];
	char currChar;
	bool validId = false;
	TOKEN * result = new TOKEN();

	//unGet char to reget the first char in the identifer.
	Fd->UngetChar();

	currChar = Fd->GetChar();
	
	while (currChar == '_' || (currChar >= 'a' && currChar <= 'z') || (currChar >= 'A' && currChar <= 'Z') || (currChar >= '0' && currChar <= '9'))
	{
		Buffer[index] = currChar;
		index++;
		currChar = Fd->GetChar();

		validId = true;
	}//while

	if (!validId)
	{
		isValid = false;
		return nullptr;
	}

	Buffer[index] = '\0';
	strcpy(idName, Buffer);
	isValid = true;
	result->type = lx_identifier;
	result->str_ptr = idName;
	return result;
}

char ** SCANNER::getKeywordString() {
	return &keywordString;
}

char ** SCANNER::getOperatorString() {
	return &operatorString;
}

TOKEN * SCANNER::get_string(bool & isValid)
{
	TOKEN * result = new TOKEN();

	int index = 0;
	char Buffer[4086];
	char currChar;

	while ((currChar = Fd->GetChar()) != '"')
	{
		//invalid string not terminated by "
		if (currChar == '\0')
		{
			isValid = false;
			return nullptr;
		}

		Buffer[index++] = currChar;

	}//while

	Buffer[index] = '\0';
	result->str_ptr = Buffer;
	result->type = lx_string;
	isValid = true;
	return result;
}

TOKEN * SCANNER::get_int(bool & isValid)
{
	int  index = 0;
	int counterOfDots = 0;
	char Buffer[4086];
	int counterOfDigsBeforeDot = 0;
	int counterOfDigsAfterDot = 0;
	char currChar;
	TOKEN * result = new TOKEN();
	result->type = lx_integer; // the first assumption is integer

							   //unGet char to reget the first char in the identifer.
	Fd->UngetChar();

	while ((currChar = Fd->GetChar()) != '\0' && currChar != ' ' && currChar != '\n')
	{
		if (currChar == '+' || currChar == '-' || currChar == '*' || currChar == '/' || currChar == '<' || currChar == '>' || currChar == '=' || currChar == ';' || currChar == ':')
		{
			Fd->UngetChar();
			break;
		}

		if (currChar >= '0' && currChar <= '9')
		{
			counterOfDigsBeforeDot++;
			Buffer[index] = currChar;
			index++;
		}
		else if (currChar == '.' && counterOfDots == 0 && counterOfDigsBeforeDot > 0)
		{
			counterOfDots++;
			Buffer[index++] = currChar;
			result->type = lx_float;
		}
		else
		{
			isValid = false;
			return nullptr;
		}

	}//while

	if (result->type == lx_float && Buffer[index - 1] == '.')
	{
		isValid = false;
		return nullptr;
	}

	isValid = true;
	if (result->type == lx_float) {
		result->float_value = string_to_float(Buffer, index);
		result->type = lx_float;
	}
	else {
		result->value = string_to_int(Buffer, index);
		result->type = lx_integer;
	}
	return result;
}

TOKEN * SCANNER::get_op(char c)
{
	TOKEN *T = new TOKEN();
	char c2 = Fd->GetChar();
	bool valid = false;
	int i;
	if (c2 == ' ') {
		Fd->UngetChar();
		char op1[2];
		op1[0] = c;
		op1[1] = '\0';

		for (i = 0; i < 20; i++) {
			valid = checkIfEqual(op1, Operator[i]);
			if (valid) {
				T->type = op_key[i];
				break;
			}
		}
	}
	else {
		char op2[3];
		op2[0] = c;
		op2[1] = c2;
		op2[2] = '\0';

		for (i = 0; i < 21; i++) {
			valid = checkIfEqual(op2, Operator[i]);
			if (valid) {
				T->type = op_key[i];
				break;
			}
		}
	}
	return T;
}

SCANNER::SCANNER()
{
}

TOKEN * SCANNER::Scan()
{
	char *line = new char[Fd->GetLineLength()];
	strcpy(line, Fd->getCurrLine());
	TOKEN *result = new TOKEN();
	char currChar;
	bool valid = false;
	currChar = Fd->GetChar();

	//if charcter == NULL WHO CARES??!
	if (currChar == NULL)
		return nullptr;

	//if(currChar == '\0')Fd->next check if end of line in the text file

	//if Char == things down here, skip them till you find something new
	while (currChar == ' ' || currChar == '\n' || currChar == '\r')
	{
		currChar = Fd->GetChar();
	}

	// if char is an alphabet -> check for keyword || identifier
	if ((currChar >= 'a' && currChar <= 'z') || (currChar >= 'A' && currChar <= 'Z')|| currChar == '_')
	{
		valid = check_keyword(result->type, currChar);
		if (!valid)
			result = get_id(valid);
		if (!valid && result == nullptr)
		{
			Fd->ReportError("Invalid Token");
			return nullptr;
		}
		if (valid) {
			return result;
		}//if
	}
	
	else if (currChar >= '0' && currChar <= '9')
	{
		result = get_int(valid);
		if (!valid && result == nullptr)
		{
			Fd->ReportError("Invalid Token");
			return nullptr;
		}
		return result;
	}//if
	
	else if (currChar == '"')
	{
		result = get_string(valid);
		if (!valid && result == nullptr)
		{
			Fd->ReportError("Invalid string");
			return nullptr;
		}
		return result;
	}
	
	else if (currChar == '#')
	{
		valid = skip_comments();
		if (!valid)
		{
			Fd->ReportError("Invalid comment");
			return nullptr;
		}
		return new TOKEN();
	}//if
	
	else {
		result = get_op(currChar);
		if (!result)
		{
			Fd->ReportError("Invalid Operator");
			return nullptr;
		}
		else {
			return result;
		}
	}
	return NULL;
}

void SCANNER::print_token(TOKEN * result)
{
	if (result->type != NULL && result->type < 34 && result->type>3)
		printf("The lexeme type [%s] \n", keyword[result->type-4]);
	else if (result->type != NULL && result->type > 33)
		printf("The lexeme type [%s] \n", Operator[result->type-34]);
	if (result->str_ptr != nullptr)
		printf("the id of string is [%s] \n", result->str_ptr);
	if (result->value != NULL)
		printf("the integer value [%d] \n", result->value);
	if (result->float_value != NULL)
		printf("the floating value [%f] \n", result->float_value);
}

SCANNER::SCANNER(FileDescriptor *fd)
{
	Fd = fd;
}

struct TrieNode *SCANNER::getRootOfOperators() {
	return rootOfOperators;
}
struct TrieNode *SCANNER::getRootOfKeywords() {
	return rootOfKeywords;
}

void SCANNER::split(char * ToSplit, char *** splitted, int numberOfTokens, char *delim) {
	char * pch;
	(*splitted) = new char*[numberOfTokens];

	char *WTH = new char[strlen(ToSplit)];
	strcpy(WTH, ToSplit);

	pch = strtok(WTH, delim);
	(*splitted)[0] = pch;
	int i = 1;
	while (pch != NULL)
	{
		pch = strtok(NULL, delim);
		(*splitted)[i] = pch;
		i++;
	}

}

int SCANNER::numberOfTokens(char * line, char delim) {
	int counter = 0;
	for (int i = 0; line[i] != '\0'; i++) {
		if (line[i] == delim)
			counter++;
	}
	return counter + 1;
}

bool SCANNER::checkIfEqual(char* first, char* second) {

	if (!strcmp(first, second))
		return true;
	return false;
}

// Returns new trie node (initialized to NULLs)
struct TrieNode *getNode(void)
{
	struct TrieNode *pNode = NULL;

	pNode = new struct TrieNode;
	if (pNode)
	{
		int i;
		pNode->isLeaf = false;
		for (i = 0; i < ALPHABET_SIZE; i++)
			pNode->children[i] = NULL;
	}
	return pNode;
}

char SCANNER::findMinChar(char *line) {
	int temp = 500;
	char smallest;
	for (int i = 0; i < strlen(line); i++) {
		if (line[i] < temp && line[i] != ' ' && line[i] != '\n' && line[i] != '\0') {
			smallest = line[i];
			temp = (int)line[i];
		}
	}

	return smallest;
}

char SCANNER::findMaxChar(char *line) {
	int temp = 500;
	char Biggest;
	for (int i = 0; i < strlen(line); i++) {
		if (line[i] > temp && line[i] != ' ' && line[i] != '\n' && line[i] != '\0') {
			Biggest = line[i];
			temp = (int)line[i];
		}
	}

	return Biggest;
}

// If not present, inserts key into trie
// If the key is prefix of trie node, just marks leaf node
void SCANNER::insert(struct TrieNode **root, char *key)
{
	int level;
	int length = strlen(key);
	int index;

	struct TrieNode *pCrawl = (*root);

	for (level = 0; level < length; level++)
	{
		index = (int)CHAR_TO_INDEX(key[level]);
		if (!pCrawl->children[index])
			pCrawl->children[index] = getNode();

		pCrawl = pCrawl->children[index];
	}

	// mark last node as leaf
	pCrawl->isLeaf = true;
}

// Returns true if key presents in trie, else false
bool SCANNER::search(struct TrieNode **root, char *key)
{
	int level;
	int length = strlen(key);
	int index;
	struct TrieNode *pCrawl = (*root);

	for (level = 0; level < length; level++)
	{
		index = (int)CHAR_TO_INDEX(key[level]);

		if (!pCrawl->children[index])
			return false;

		pCrawl = pCrawl->children[index];
	}


	return (pCrawl != NULL && pCrawl->isLeaf);

}

char ** SCANNER::cleanToken(char *c) {
	int i = 0;
	char * temp;
	while (c[i] != '\n')
		i++;
	temp = new char[i];
	for (int j = 0; j < i; j++)
		temp[j] = c[j];
	return &temp;
}

void SCANNER::addStringToTrie(char **s, struct TrieNode **root) {
	(*root) = getNode();
	// Construct trie
	/*int i;
	int N = numberOfTokens(s, ' ');
	char **keys = new char*[N];
	split(s, &keys, N, " ");*/

	//smallestInString = findMinChar(*s);

	for (int i = 0; s[i] != NULL; i++) {
		size_t length = strlen(s[i]);
		if (s[i][length - 1] == '\n')
			s[i][length - 1] = '\0';
		insert(root, s[i]);
	}
}

void SCANNER::addSpaces(char * str1) {
	char * str = new char[strlen(str1)];
	strcpy(str,str1);
	int shiftIndex = 0;
	bool doubleOp = false, singleOp = false;
	for (int i = 0; i < strlen(str); i++) {
		doubleOp = false;
		singleOp = false;
		if (isOperator(str[i])&&str[i]!='.') {
			shiftIndex = i;
			singleOp = true;
			if (isOperator(str[i+1])&&str[i+1]!='(') {
				shiftIndex = i+1;
				doubleOp = true;
				singleOp = false;
			}
		}
		if (doubleOp) {
			strcpy(str, shiftOnIndex(str, shiftIndex + 1));
			strcpy(str, shiftOnIndex(str, shiftIndex - 1));
			i += 3;
			doubleOp = false;
		}
		else if(singleOp){

			strcpy(str, shiftOnIndex(str, shiftIndex + 1));
			strcpy(str, shiftOnIndex(str, shiftIndex));
			i += 2;
			singleOp = false;
		}
	}
	strcpy(str1,str);
}
char * SCANNER::shiftOnIndex(char *s, int index) {
	char *shifted = new char[strlen(s) + 1];
	strcpy(shifted,s);
	int N = strlen(shifted);
	shifted[N+1] = '\0';
	for (int i = N; i > index ; i--) {
		shifted[i] = shifted[i-1];
	}
	shifted[index] = ' ';
	return shifted;
}
bool SCANNER::isOperator(char s) {
	char op[2];
	op[0] = s;
	op[1] = '\0';
	bool valid = false;
	int i;
	for (i = 0; i < 20; i++) {
		valid = checkIfEqual(op, Operator[i]);
		if (valid) {
			return true;
			break;
		}
	}
	return false;
}