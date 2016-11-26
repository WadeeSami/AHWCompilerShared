#include <fstream>
#pragma once
#define SET 1
#define UNSET 0
#define BUFFER_SIZE 256
class FileDescriptor
{
	/* You must fill in these fields */
private:

	int line_number; /* line number in the file */
	int char_number; /* character number in the line */
	int flag; /* to prevents two ungets in a row */
	int buf_size; /* stores the buffer size */
	char *buffer; /* buffer to store a line */
	char *file; /* file name, allocate memory for this */
	int flag2;
	int currentLineLength;

public:
	/* Externally-visible functions: */
	FILE *fp;
	FILE *GetFP();
	char *GetFileName();
	bool IsOpen(); // returns true if a file is open without errors, //otherwise returns false
	char *getNextLine();
	char * getCurrLine();
	// returns a pointer to buffer, null if EOF
	int GetLineNum();
	int GetCharNum();
	int GetLineLength();
	FileDescriptor(char *FileName); // opens the stdin FileName is Null
	~FileDescriptor();
	FileDescriptor();
	//FileDescriptor(); //opens the stdin
	void Close(); // closes the file descriptor
	char GetChar(); // Gets the current character in the file
					//				//bool open_file_descriptor (char *filename);
	void ReportError(char *msg); // reports the error specifying the current line and //character
	void UngetChar(); // puts back the current character, modifies char number
	void addSpaces(char * str1);
	char * shiftOnIndex(char * s, int index);
	bool isOperator(char s);
	bool checkIfEqual(char * first, char * second);
};