#include "../include/font.h"

#include <stdlib.h>
#include <stdio.h>

// #include "../include/shader.h"

//reference to the constants in font.h

// static GLuint fontTextureID;
// static unsigned char* fontPixels;
// static unsigned int fontWidth;
// static unsigned int fontHeight;

// static unsigned int glyphWidth;
// static unsigned int glyphHeight;

void initFont()
{
	fontPixels = loadbmp("res/default_font.bmp", &fontWidth, &fontHeight);
	fixPixelsForSDL(fontPixels, fontWidth, fontHeight);

	fontSurface = SDL_CreateRGBSurfaceFrom(
		fontPixels,
		fontWidth,
		fontHeight,
		32,//32 bit color
		fontWidth*4,//width pixels * 4 bytes long
		0xff000000, //rmask
		0x00ff0000, //gmask
		0x0000ff00, //bmask
		0x000000ff  //amask
		);
}

//these have to do with the font texture image
#define CHAR_WIDTH 14
#define CHAR_HEIGHT 18
#define NUM_CHARS_PER_ROW 18

//the characters are all 14x18 (WxH)
int getXYForChar(char letter, int* x, int* y)
{	
	if (letter < ' ' || letter > '~')
		return 0;
	int delta = (int)(letter - ' ');
	// printf("delta: %d\n", delta);
	*x = (delta%NUM_CHARS_PER_ROW ) * CHAR_WIDTH;
	*y = (delta/NUM_CHARS_PER_ROW ) * CHAR_HEIGHT;
	return 1;
}

void drawGlyph(char letter, int r, int c, SDL_Renderer* dst)
{
	if (fontTexture == NULL)
	{
		fontTexture = SDL_CreateTextureFromSurface(dst, fontSurface);
	}	

	int fontX, fontY;
	if (!getXYForChar(letter, &fontX, &fontY))
	{
		// fprintf(stderr, "Error, character %c is not renderable!\n", letter);
		return;
	}

	int outX = c * CHAR_WIDTH;
	int outY = r * CHAR_HEIGHT;

	SDL_Rect fontRect;
	fontRect.x = fontX;
	fontRect.y = fontY;
	fontRect.w = CHAR_WIDTH;
	fontRect.h = CHAR_HEIGHT;

	SDL_Rect outRect;
	outRect.x = outX;
	outRect.y = outY;
	outRect.w = CHAR_WIDTH;
	outRect.h = CHAR_HEIGHT;

	SDL_RenderCopy(dst, fontTexture, &fontRect, &outRect);
}

void cleanUpFont()
{
	SDL_DestroyTexture(fontTexture);
	SDL_FreeSurface(fontSurface);
	free(fontPixels);
}