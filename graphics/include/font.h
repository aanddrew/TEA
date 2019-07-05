#ifndef BMPFONT_H
#define BMPFONT_H

#include "GL/glew.h"
#include "bmploader.h"

#include <SDL2/SDL.h>

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

void drawGlyph(char letter, int r, int c, SDL_Renderer* dst);

void dispTexture();

#endif