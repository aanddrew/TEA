#include "../include/bmploader.h"

#include <stdio.h>
#include <stdlib.h>

//word is two bytes dword is four
#define WORD unsigned short
#define DWORD unsigned int

struct BITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved; must be 0
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits (pixel array)
};

//loads bmp file and returns a pixel array
unsigned char* loadbmp(const char* fileName, unsigned int* width, unsigned int* height)
{
	FILE* file = fopen(fileName, "rb");

	struct BITMAPFILEHEADER fileHeader;
	fread(&fileHeader, sizeof(struct BITMAPFILEHEADER), 1, file);

	if (fileHeader.bfType != (WORD)0x4d42)
	{
		fprintf(stderr, "File %s is not a bmp file\n", fileName);
		return NULL;
	}

	DWORD DIBHeaderSize;
	fseek(file, 14, SEEK_SET);
	fread(&DIBHeaderSize, sizeof(DWORD), 1, file);

	printf("pixels start at: %u\n", fileHeader.bfOffBits);

	//get the width which is 4 bytes from the file header
	fseek(file, 14 + 4, SEEK_SET);
	fread(width, sizeof(unsigned int), 1, file);

	//the height is 8 bytes past that
	fseek(file, 14+8, SEEK_SET);
	fread(height, sizeof(unsigned int), 1, file);

	unsigned int offset;
	fseek(file, 10, SEEK_SET);
	fread(&offset, 4, 1, file);

	printf("offset is %u\n", offset);

	fseek(file, offset, SEEK_SET);

	//allocate width times height pixels
	unsigned int BYTES_PER_PIXEL = 4; //32 bit color - rgba assumed
	unsigned int SIZE_IMAGE_IN_BYTES = (*width) * (*height) * BYTES_PER_PIXEL;

	printf("size image in bytes: %u\n", SIZE_IMAGE_IN_BYTES);

	unsigned char* pixels = (unsigned char*)(malloc(SIZE_IMAGE_IN_BYTES));

	fread(pixels, SIZE_IMAGE_IN_BYTES, 1, file);
	fclose(file);

	return pixels;
}

void fixPixelsForSDL(unsigned char* pixels, unsigned int width, unsigned int height)
{
	unsigned int arrSize = width*height*4;

	// for(int y = 0; y < height*4/2; y++)
	// {
	// 	for (int x = 0; x < width*4; x++)
	// 	{
	// 		unsigned char temp = pixels[y*width + x];
	// 		pixels[y*width + x] = pixels[(height*4 - y)*width + x];
	// 		pixels[(height*4 - y)*width + x] = temp;
	// 	}
	// }

	for(int i = 0; i < arrSize/2; i++)
	{
		unsigned char temp = pixels[i];
		pixels[i] = pixels[arrSize - i];
		pixels[arrSize - i] = temp;
	}

	//it is no longer upside down, now let's flip it horizontally}

}