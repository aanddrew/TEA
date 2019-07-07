#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stdio.h>

struct Buffer
{
	FILE* file;
	char* fileName;
	// char* encoding;

	//array of strings, not null terminated, lines end in 
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
bool addRow(struct Buffer* buffer, int r);
bool deleteRow(struct Buffer* buffer, int r);
bool backspace(struct Buffer* buffer, int r, int c);
bool del(struct Buffer* buffer, int r, int c);

void saveBuffer(struct Buffer* buffer);
void destroyBuffer(struct Buffer* buffer);

#endif