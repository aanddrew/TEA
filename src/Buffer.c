#include "../include/Buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Buffer* createBuffer(const char* fileName)
{
	struct Buffer* buffer = (struct Buffer*) malloc(sizeof(struct Buffer));
	buffer->file = fopen(fileName, "r+");
	//if file doesn't open, return null
	if (buffer->file == NULL)
		return NULL;

	buffer->fileName = malloc(sizeof(char) * strlen(fileName));
	//copy the fileName to our buffer object
	strcpy(buffer->fileName, fileName);

	//calculate the max rows in the file
	buffer->maxRows = 1; //double this if we go past it
	{
		//assume there is one line because the last line doesn't have \n char
		int tempRows = 1;
		char c = fgetc(buffer->file);
		while (c != EOF)
		{
			// printf("%c", c);
			if (c == '\n')
			{
				tempRows++;
			}
			c = fgetc(buffer->file);
		}
		while(buffer->maxRows < tempRows)
		{
			buffer->maxRows *= 2;
		}


	}//end calculating rows.
	buffer->rows = (char**) malloc(sizeof(char*) * buffer->maxRows);	
	buffer->maxLengths = (int*) malloc(sizeof(int) * buffer->maxRows);
	buffer->lengths = (int*) malloc(sizeof(int) * buffer->maxRows);

	for(int i = 0; i < buffer->maxRows; i++)
	{
		buffer->rows[i] = NULL;
	}	
	//start over and let's fill our rows array
	rewind(buffer->file);

	//start by assuming the line is under 128 chars
	char* tempLine = (char*) malloc(sizeof(char)* 128);
	int i = 0;
	int r = 0;
	char c = fgetc(buffer->file);
	while (c != EOF)
	{
		if (tempLine[i-1] == '\n')
		{
			tempLine[i-1] = '\0';
			buffer->rows[r] = (char*) malloc(sizeof(char) * i);
			buffer->maxLengths[r] = i;
			buffer->lengths[r] = i;
			int x = 0;
			for(; tempLine[x] != '\0'; x++)
			{
				buffer->rows[r][x] = tempLine[x];
			}
			buffer->rows[r][x] = '\0';

			r++;
			i = 0;
		}


		tempLine[i] = c;
		c = fgetc(buffer->file);
		i++;
	}
	//grab the last line of the file
	tempLine[i] = '\0';
	buffer->rows[r] = (char*) malloc(sizeof(char) * i);
	int x = 0;
	for(; tempLine[x] != '\0'; x++)
	{
		buffer->rows[r][x] = tempLine[x];
	}
	buffer->rows[r][x] = '\0';
	r++;

	buffer->numRows = r;

	free(tempLine);

	// printf("max rows: %d\n", buffer->maxRows);

	return buffer;
}

//returns true if the start is greater than 0 and less than the strlen(string)
bool shiftStringBackward(char* string, int start, int length)
{
	// int len = strlen(string);
	if (start == 0 || start > length)
		return false;

	for(int i = start-1; i < length; i++)
	{
		string[i] = string[i+1];
		//this will bring the null character backwards too
	}
	return true;
}

bool deleteFromString(char* string, int start, int length)
{
	// int len = strlen(string);
	if (start >= length + 1 || length == 0)
		return false;

	for(int i = start-1; i < length; i++)
	{
		string[i] = string[i+1];
		//this will bring the null character backwards too
	}
	return true;
}

//helper function returns the new max length of the string
int shiftStringForward(char** string, int maxLength, int start)
{
	int newMaxLength = maxLength;
	int size = strlen(*string);

	if (start > size+1)
		return -1;

	if (strlen(*string) + 1 >= newMaxLength)
	{
		newMaxLength *= 2;
		char* newString = (char*) malloc(sizeof(char) * newMaxLength);

		//we'll just handle it here
		for(int i = 0; i < start; i++)
		{
			newString[i] = (*string)[i];
		}
		//fill the gaps in with spaces
		for(int i = start; i < start + 1; i++)
		{
			newString[i] = ' ';
		}
		//then we put the rest at the end
		int i = start;
		for(; i <= size; i++)
		{
			newString[i + 1] = (*string)[i];
		}

		newString[newMaxLength-1] = '\0';

		//remove the old one
		free(*string);
		*string = newString;	
	}
	//we don't have to reallocate memory
	else
	{
		for(int i = size-1; i >= start; i--)
		{
			(*string)[i+1] = (*string)[i];
		}
		(*string)[start] = ' ';
	}

	return newMaxLength;
}

bool insertIntoBuffer(struct Buffer* buffer, int r, int c, char inserted)
{	
	if (r >= buffer->maxRows)
	{
		return false;
	}

	if (buffer->rows[r] == NULL)
	{
		buffer->rows[r] = (char*) malloc(sizeof(char) * 80);
		for(int i = 0; i < c; i++)
		{
			buffer->rows[r][i] = ' ';
		}

		buffer->rows[r][c] = inserted;
		buffer->rows[r][c+1] = '\0';
	}
	else
	{
		buffer->maxLengths[r] = 
			shiftStringForward(&(buffer->rows[r]), buffer->maxLengths[r], c);
		buffer->rows[r][c] = inserted;
	}

	buffer->lengths[r]++;
	return true;
}

bool backspace(struct Buffer* buffer, int r, int c)
{
	if (shiftStringBackward(buffer->rows[r], c, buffer->lengths[r]))
	{
		buffer->lengths[r]--;
		return true;
	}
	return false;
}

bool del(struct Buffer* buffer, int r, int c)
{
	if (deleteFromString(buffer->rows[r], c, buffer->lengths[r]))
	{
		buffer->lengths[r]--;
		return true;
	}
	return false;
}

bool deleteRow(struct Buffer* buffer, int r)
{

}

// void shiftRowsUp(struct Buffer* buffer, int r)
// {

// }

//adds a row after the current row, returns true if the buffer was resized
bool addRow(struct Buffer* buffer, int r)
{
	bool resized = false;
	int newMaxRows = buffer->maxRows;
	if (buffer->numRows >= buffer->maxRows)
	{
		//resize the buffer
		newMaxRows *= 2;
		char** copyOfOldRows = buffer->rows;

		buffer->rows = (char**) malloc(sizeof(char*) * newMaxRows);

		//count up
		for(int i = 0; i < r+1; i++)
		{
			buffer->rows[i] = copyOfOldRows[i];
		}
		//in the middle
		buffer->rows[r+1] = (char*) malloc(sizeof(char)*80);
		buffer->rows[r+1][0] = '\0';
		buffer->maxLengths[r+1] = 80;
		buffer->lengths[r+1] = 0;
		//count down
		for(int i = buffer->numRows; i > r+1; i--)
		{
			buffer->rows[i] = copyOfOldRows[i-1];
			buffer->maxLengths[i] = buffer->maxLengths[i-1];
			buffer->lengths[i] = buffer->lengths[i-1];
		}

		buffer->numRows++;
		buffer->maxRows = newMaxRows;
		resized = true;
	}
	else
	{
		char** copyOfOldRows = buffer->rows;

		buffer->rows = (char**) malloc(sizeof(char*) * newMaxRows);

		//count up
		for(int i = 0; i < r+1; i++)
		{
			buffer->rows[i] = copyOfOldRows[i];
		}
		//in the middle
		buffer->rows[r+1] = (char*) malloc(sizeof(char)*80);
		buffer->rows[r+1][0] = '\0';
		buffer->maxLengths[r+1] = 80;
		buffer->lengths[r+1] = 0;
		//count down
		for(int i = buffer->numRows; i > r+1; i--)
		{
			buffer->rows[i] = copyOfOldRows[i-1];
			buffer->maxLengths[i] = buffer->maxLengths[i-1];
			buffer->lengths[i] = buffer->lengths[i-1];
		}

		buffer->numRows++;
	}
}

void destroyBuffer(struct Buffer* buffer)
{
	for(int i = 0; i < buffer->maxRows; i++)
	{
		if ((buffer->rows)[i] != NULL)
		{
			// printf("freeing buffer->rows[%d]: %p\n", i, (buffer->rows)[i]);
			// printf("\tbuffer contents:\n\t%s\n", buffer->rows[i]);
			free(((buffer->rows)[i]));
		}
	}
	free(buffer->rows);
	free(buffer->fileName);
	free(buffer->lengths);
	free(buffer->maxLengths);

	fclose(buffer->file);
	free(buffer);
}
