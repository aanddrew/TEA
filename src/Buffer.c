#include "../include/Buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//references
// struct Buffer
// {
// 	FILE* file;
// 	char* fileName;
// 	// char* encoding;

// 	//array of strings, not null terminated, lines end in 
// 	char ** rows;
// 	int numRows;
// 	int maxRows;
// 	int* lengths;
// 	int* maxLengths;
// 	//we will just hold the file in a big 2d array of strings
// };

struct Buffer* createBuffer(const char* fileName)
{
	struct Buffer* buffer = (struct Buffer*) (malloc(sizeof(struct Buffer) * 1));

	buffer->file = fopen(fileName, "r+");
	if (buffer->file == NULL)
	{
		buffer->file = fopen(fileName, "w");
	}
	buffer->fileName = (char*)(malloc(sizeof(char) * (strlen(fileName) + 1)));
	strcpy(buffer->fileName, fileName);

	//assume that each file has 1 row.
	buffer->maxRows = 1;
	{
		int tempRows = 1;
		char c = fgetc(buffer->file);
		while(c != EOF)
		{
			if (c == '\n')
			{
				tempRows++;
			}
			c = fgetc(buffer->file);
		}

		buffer->numRows = tempRows;
		while(buffer->maxRows < tempRows)
		{
			buffer->maxRows *= 2;
		}
	}

	buffer->rows = (char**) malloc(sizeof(char*) * buffer->maxRows);	
	buffer->maxLengths = (int*) malloc(sizeof(int) * buffer->maxRows);
	buffer->lengths = (int*) malloc(sizeof(int) * buffer->maxRows);

	for(int i = 0; i < buffer->maxRows; i++)
	{
		buffer->rows[i] = NULL;
	}	
	//start over and let's fill our rows array
	rewind(buffer->file);

	//start by assuming the line is under 80 chars
	int sizeLine = 80;
	char* tempLine = (char*) malloc(sizeof(char)* sizeLine);
	int i = 0;
	int r = 0;
	char c = fgetc(buffer->file);
	while (c != EOF)
	{
		if (i >= sizeLine)
		{
			sizeLine *= 2;
			char* newTempLine = (char*) malloc(sizeof(char) * sizeLine);
			for(int i = 0; i < sizeLine; i++)
			{
				newTempLine[i] = tempLine[i];
			}
			free(tempLine);
			tempLine = newTempLine;
		}

		if (c == '\n')
		{
			int sizeBufferLine = 80;
			while(sizeBufferLine < i)
			{
				sizeBufferLine*= 2;
			}
			buffer->rows[r] = (char*) (malloc(sizeof(char)* sizeBufferLine));
			for(int k = 0; k < i; k++)
			{
				buffer->rows[r][k] = tempLine[k];
			}
			buffer->maxLengths[r] = sizeBufferLine;
			buffer->lengths[r] = i;
			i = 0;
			r++;
		}
		else
		{
			tempLine[i] = c;
			i++;
		}
		c = fgetc(buffer->file);
	}

	int sizeBufferLine = 80;
	while(sizeBufferLine < i)
	{
		sizeBufferLine*= 2;
	}
	buffer->rows[r] = (char*) (malloc(sizeof(char)* sizeBufferLine));
	for(int k = 0; k < i; k++)
	{
		buffer->rows[r][k] = tempLine[k];
	}
	buffer->maxLengths[r] = sizeBufferLine;
	buffer->lengths[r] = i;
	i = 0;
	r++;

	return buffer;
}

//manipulation of buffer
bool insertIntoBuffer(struct Buffer* buffer, int r, int c, char inserted)
{
	bool resized = false;
	//we are only inserting one character into buffer->rows[r];
	if (buffer->lengths[r] >= buffer->maxLengths[r])
	{
		int newMaxLength = buffer->maxLengths[r] * 2;
		char* newRow = (char*) (malloc(sizeof(char) * newMaxLength));
		//copy chars to a new string
		for(int i = 0; i < buffer->lengths[r]; i++)
		{
			newRow[i] = buffer->rows[r][i];
		}
		//free it and re-assign to the new string
		free(buffer->rows[r]);
		buffer->rows[r] = newRow;
		buffer->maxLengths[r] = newMaxLength;

		resized = true;
	}
	//now that it has been resized we are ready to rock and roll.
	//we have to create a space at r, c to insert the character.
	for(int i = buffer->lengths[r]; i > c; i--)
	{
		buffer->rows[r][i] = buffer->rows[r][i-1];
	}
	//now we just insert it in the right place. easy as pie
	buffer->rows[r][c] = inserted;
	buffer->lengths[r]++;

	return resized;
}

bool addRow(struct Buffer* buffer, int r)
{
	bool resized = false;

	//minus 1 because if we add a row at row 7 with 8 max rows, we have to resize
	if (r >= buffer->maxRows - 1 || buffer->numRows == buffer->maxRows 	)
	{
		int newMaxRows = buffer->maxRows * 2;
		char** newRows = (char**) (malloc(sizeof(char*) * newMaxRows));
		int* newLengths = (int*) (malloc(sizeof(int) * newMaxRows));
		int* newMaxLengths = (int*) (malloc(sizeof(int) * newMaxRows));
		//init this to null
		for(int i = 0; i < newMaxRows; i++)
		{
			newRows[i] = NULL;
			newLengths[i] = 0;
			newMaxLengths[i] = 0;
		}
		//copy our rows array to the new one
		for(int i = 0; i < buffer->numRows; i++)
		{
			newRows[i] = buffer->rows[i];
			newLengths[i] = buffer->lengths[i];
			newMaxLengths[i] = buffer->maxLengths[i];
		}
		//trash old one
		free(buffer->rows);
		free(buffer->lengths);
		free(buffer->maxLengths);
		//give it our new, resized array
		buffer->rows = newRows;
		buffer->lengths = newLengths;
		buffer->maxLengths = newMaxLengths;

		buffer->maxRows = newMaxRows;

		resized = true;
	}

	//now we do something similar to the insertIntoBuffer
	for(int i = buffer->numRows; i > r; i--)
	{
		buffer->rows[i] = buffer->rows[i-1];
		buffer->lengths[i] = buffer->lengths[i-1];
		buffer->maxLengths[i] = buffer->maxLengths[i-1];
	}

	//now we can add something new at buffer->rows[r], a new 80 char line
	buffer->rows[r] = (char*) (malloc(sizeof(char) * 80));
	buffer->lengths[r] = 0;
	buffer->maxLengths[r] = 80;
	buffer->numRows++;

	return resized;
}
bool deleteRow(struct Buffer* buffer, int r)
{

}

bool backspace(struct Buffer* buffer, int r, int c)
{
	if (c == 0)
	{
		return false;
	}
	else
	{
		for(int i = c - 1; i < buffer->lengths[r] - 1; i++)
		{
			buffer->rows[r][i] = buffer->rows[r][i+1];
		}
		buffer->lengths[r]--;
		return true;
	}
}
bool del(struct Buffer* buffer, int r, int c)
{
	//at the end of the line
	if (c >= buffer->lengths[r])
	{
		return false;
	}

	for(int i = c - 1; i < buffer->lengths[r] - 1; i++)
	{
		//don't overstep your bounds
		if (i == -1)
			continue;
		buffer->rows[r][i] = buffer->rows[r][i+1];
	}

	buffer->lengths[r]--;
	return true;
}

void saveBuffer(struct Buffer* buffer)
{
	fclose(buffer->file);
	buffer->file = fopen(buffer->fileName, "w");
	for(int r = 0; r < buffer->numRows; r++)
	{
		for (int c = 0; c < buffer->lengths[r]; c++)
		{
			fprintf(buffer->file, "%c", buffer->rows[r][c]);
		}
		fprintf(buffer->file, "\n");
	}
}

void destroyBuffer(struct Buffer* buffer)
{
	fclose(buffer->file);
	for(int i = 0; i < buffer->maxRows; i++)
	{
		if ((buffer->rows)[i] != NULL)
		{
			// printf("freeing buffer->rows[%d]: %p\n", i, (buffer->rows)[i]);
			// printf("\tbuffer contents:\n\t%s\n", buffer->rows[i]);
			free((buffer->rows)[i]);
		}
	}
	free(buffer->rows);
	free(buffer->fileName);
	free(buffer->lengths);
	free(buffer->maxLengths);

	free(buffer);
}
