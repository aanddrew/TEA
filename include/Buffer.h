#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stdio.h>

struct Buffer
{
	FILE* file;
	char* fileName;
	// char* encoding;

	//array of strings - null terminated, no newline characters
	char ** rows;
	int numRows;
	int maxRows;
	int* lengths;
	int* maxLengths;
	//we will just hold the file in a big 2d array of strings
};

struct Buffer* createBuffer(const char* fileName);

//manipulation of buffer
bool insertIntoBuffer(struct Buffer* buffer, int r, int c, char inserted);
bool backspace(struct Buffer* buffer, int r, int c);
bool del(struct Buffer* buffer, int r, int c);
bool addRow(struct Buffer* buffer, int r);
bool deleteRow(struct Buffer* buffer, int r);

void destroyBuffer(struct Buffer* buffer);

#endif