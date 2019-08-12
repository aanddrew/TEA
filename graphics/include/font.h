#ifndef BMPFONT_H
#define BMPFONT_H

#include "bmploader.h"

#include <SDL2/SDL.h>

//this has to do with the font bmp texture
#define CHAR_WIDTH 14
#define CHAR_HEIGHT 18

static unsigned char* fontPixels;
static unsigned int fontWidth;
static unsigned int fontHeight;
static SDL_Surface* fontSurface;
static SDL_Texture* fontTexture;

static unsigned int glyphWidth;
static unsigned int glyphHeight;

SDL_Surface* glpyhSurface;

void initFont();
void cleanUpFont();

void drawGlyph(char letter, int y, int x, SDL_Renderer* dst);

void drawString(const char* string, int y, int x, SDL_Renderer* dst);

void dispTexture();

#endif
