#include "fd.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
char * Operator1[] = {
	"{","}","(",")",":",".", ";" ,",",":=","+","-",
	"*","/","=","!=","<","<=",
	">",">="
};
FILE *FileDescriptor::GetFP()
{
	return fp;
}

char * FileDescriptor::GetFileName()
{
	return file;
}

bool FileDescriptor::IsOpen()
{
	return (fp != NULL);
}

char * FileDescriptor::getNextLine()
{
	char *check;
	buffer = new char[buf_size];
	if (!feof(fp)) {
		check = fgets(buffer, buf_size, fp);
		if (check == NULL)
			return NULL;
	}
	else {
		return NULL;
	}
	line_number++;
	char_number = 0;
	currentLineLength = 0;

	addSpaces(buffer);
	int i = 0;
	while (buffer[i] != '\0')
	{
		currentLineLength++;
		i++;
	}
	return buffer;
}

char * FileDescriptor::getCurrLine() {
	if(line_number == 0)
		return getNextLine();
	else return buffer;
}

int FileDescriptor::GetLineNum()
{
	return line_number;
}

int FileDescriptor::GetCharNum()
{
	return this->char_number;
}

int FileDescriptor::GetLineLength()
{
	return this->currentLineLength;
}

FileDescriptor::FileDescriptor(char * FileName)
{
	//store file name
	file = FileName;
	//initaialize buffer size
	buf_size = BUFFER_SIZE;
	buffer = new char[buf_size];
	//initialize line number
	line_number = 0;
	char_number = 0;

	fp = fopen(FileName, "r");
	if (fp == NULL)
		perror("Error opening file");
}

FileDescriptor::~FileDescriptor()
{
	fclose(fp);
}

FileDescriptor::FileDescriptor()
{
	buffer = NULL;
}

void FileDescriptor::Close()
{
	fclose(fp);
}

char FileDescriptor::GetChar()
{
	//do {
	if (GetCharNum() < currentLineLength)
	{
		char c = buffer[this->char_number];
		this->char_number++;
		return c;
	}
	//}
	//while(GetCurrLine()!=NULL);

	return NULL;
}

void FileDescriptor::ReportError(char * msg)
{
	cout << msg;
	cout << "[[on line " << GetLineNum() << " of prog.COMP2016 ]]" << endl;
	cout << buffer <<endl;
	for (int i = 0; i < GetCharNum()-1; i++) {
		cout << " ";
	}
	cout << "^" <<endl;
}

void FileDescriptor::UngetChar()
{
	if (char_number>0)
		char_number--;
}


void FileDescriptor::addSpaces(char * str1) {

	char * str = new char[strlen(str1)+2];
	strcpy(str, str1);
	int shiftIndex = 0;
	bool doubleOp = false, singleOp = false, endOfLine = false;
	for (int i = 0; i < strlen(str) ; i++) {
		if (i == strlen(str) - 1)endOfLine = true;
		doubleOp = false;
		singleOp = false;
		if (isOperator(str[i]) && str[i] != '.') {
			shiftIndex = i;
			singleOp = true;
			char t[1] = {' '};
			t[0] = str[i + 1];
			if (isOperator(str[i + 1]) && checkIfEqual(t,"(")) {
				shiftIndex = i + 1;
				doubleOp = true;
				singleOp = false;
			}
		}
		if (doubleOp) {
			if(!endOfLine)
				strcpy(str, shiftOnIndex(str, shiftIndex + 1));
			strcpy(str, shiftOnIndex(str, shiftIndex - 1));
			i += 3;
			doubleOp = false;
		}
		else if (singleOp) {
			if(!endOfLine)
				strcpy(str, shiftOnIndex(str, shiftIndex + 1));
			strcpy(str, shiftOnIndex(str, shiftIndex));
			i += 2;
			singleOp = false;
		}
	}
	strcpy(str1, str);
}
char * FileDescriptor::shiftOnIndex(char *s, int index) {
	char *shifted = new char[strlen(s) + 1];
	strcpy(shifted, s);
	int N = strlen(shifted);
	shifted[N + 1] = '\0';
	for (int i = N; i > index; i--) {
		shifted[i] = shifted[i - 1];
	}
	shifted[index] = ' ';
	return shifted;
}
bool FileDescriptor::isOperator(char s) {
	char op[2];
	op[0] = s;
	op[1] = '\0';
	bool valid = false;
	int i;
	for (i = 0; i < 19; i++) {
		valid = checkIfEqual(op, Operator1[i]);
		if (valid) {
			return true;
			break;
		}
	}
	return false;
}
bool FileDescriptor::checkIfEqual(char* first, char* second) {

	if (!strcmp(first, second))
		return true;
	return false;
}