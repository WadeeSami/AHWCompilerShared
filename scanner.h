#include "fd.h"
#include <string>
/* scanner .h*/
#ifndef __SCANNER__H
#define __SCANNER__H

//Token types enumeration
typedef enum
{
	/* Literals */
	lx_identifier, lx_integer, lx_string, lx_float,
	/* Keywords */
	kw_and,
	kw_begin, kw_boolean, kw_by, kw_constant, kw_do,
	kw_else, kw_end, kw_false, kw_fi, kw_float,
	kw_for, kw_from, kw_function, kw_if, kw_integer,
	kw_not, kw_od, kw_or, kw_procedure, kw_program,
	kw_read, kw_return, kw_string, kw_then, kw_to,
	kw_true, kw_var, kw_while, kw_write,
	/* Operators */
	lx_lbrace, lx_rbrace, lx_lparen, lx_rparen, lx_lbracket,
	lx_rbracket, Ix_colon, lx_dot, lx_semicolon, lx_comma,
	Ix_colon_eq, lx_plus, lx_minus, lx_star, lx_slash,
	lx_eq, lx_neq, lx_lt, lx_le, lx_gt,
	lx_ge, lx_eof
}
LEXEME_TYPE;

// Definition of TOKEN, you can use a clase if you  want 
struct	TOKEN
{
	LEXEME_TYPE type;
	int value;  // can be used instead of the str_ptr for IDs and strings
	float float_value;
	char *str_ptr; // points to strings or Identifiers, can use value
				   //instead but with type casting
};

struct TrieNode
{
	struct TrieNode *children[127 - 31];

	// isLeaf is true if the node represents
	// end of a word
	bool isLeaf;
};

class SCANNER {
private:
	// define your functions ..
	//I would define the following functions..
	// void skip_comments();
	// bool check_keyword(...)
	// TOKEN *get_id(..)
	// TOKEN *get_string(..)
	// TOKEN *get_int(..) // gets integers and floats

public:
	FileDescriptor *Fd;
	struct TrieNode *rootOfKeywords;
	struct TrieNode *rootOfOperators;
	char *keywordString = "and begin boolean by constant do else end false fi float for from function if integer not od or procedure program read return string then to true var while write";
	char *operatorString = "{ } ( ) : . ; , := + - * / = != < <= > >=";
	struct TrieNode *getRootOfOperators();
	struct TrieNode *getRootOfKeywords();
	char **getKeywordString();
	char **getOperatorString();
	char smallestInString = 127;
	char **currLineSplitted;

	SCANNER();
	//TOKEN * Scan(char * data);
	SCANNER(FileDescriptor *fd);
	//void initScanner();
	int numberOfTokens(char * line, char delim);
	bool checkIfEqual(char* first, char* second);
	void addStringToTrie(char** s, TrieNode ** root);
	void addSpaces(char * str);
	char * shiftOnIndex(char * s, int index);
	bool isOperator(char s);
	char findMinChar(char * s);
	char findMaxChar(char * line);
	void insert(TrieNode ** root, char * key);
	bool search(TrieNode ** root, char * key);
	char ** cleanToken(char *c);
	void split(char * ToSplit, char *** splitted, int numberOfTokens, char *delim);
	TOKEN* Scan();
	void print_token(TOKEN * result);
	// You may define it as TOKEN *Scan(FileDescriptor *fd);

	float string_to_float(char * Buffer, int size);

	int string_to_int(char * Buffer, int size);

	bool skip_comments();

	bool check_keyword(LEXEME_TYPE & LEX, char currChar);

	TOKEN * get_id(bool & isValid);

	TOKEN * get_string(bool & isValid);

	TOKEN * get_int(bool & isValid);

	TOKEN * get_op(char c);

};

// It is helpful to define the following arrays in your .cpp file
// not in this header file

/* The keyword list is and must be kept in alphabetical order. To
add a keyword, insert it at the appropriate place in keyword[],
insert its type in the appropriate place in key type[], and
increment the var keys. Order is needed for binary search
*/


#endif
