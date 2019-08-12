#include <stdio.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "include/Buffer.h"
#include "include/Input.h"
#include "graphics/include/bmploader.h"
#include "graphics/include/font.h"

//1120 is 80 characters wide
#define SCREEN_WIDTH 1400
#define SCREEN_HEIGHT 560

SDL_Window* init();
void handleKeyInput(struct Buffer* buffer, SDL_Event* e, int cursorR, int cursorC, 	bool shift);
void renderBufferToTerminal(struct Buffer* buffer, int cursorR, int cursorC);
void renderBufferToRenderer(struct Buffer* buffer, SDL_Renderer* dst, 
	int cursorR, int cursorC, int scrollRows, int scrollCols);

int main(int argc, char** argv)
{
	printf("Welcome to TEA - the Text Editor for Armadillos\n~~\n");
	
	if (argc > 2)
	{
		fprintf(stderr, "Error, too many arguments\n");
		return 1;
	}
	//END STARTUP

	//WINDOW CREATION
	SDL_Window* window = init();

	if (window == NULL)
	{
		fprintf(stderr, "Error, could not init SDL\n");
		return 1;
	}
	//give it an icon
	unsigned int logoWidth, logoHeight;
	unsigned char* logoPixels = loadbmp("res/default/icon_256.bmp", &logoWidth, &logoHeight);
	fixPixelsForSDL(logoPixels, logoWidth, logoHeight);


	//change this to customize font colors
	SDL_Surface* logoSurface = SDL_CreateRGBSurfaceFrom(
		logoPixels,
		logoWidth,
		logoHeight,
		32,//32 bit color
		logoWidth*4,//width pixels * 4 bytes long
		0xff000000, //rmask
		0x00ff0000, //gmask
		0x0000ff00, //bmask
		0x000000ff  //amask
	);
	SDL_SetWindowIcon(window, logoSurface);
	//end window creation

	SDL_Renderer* windowRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

	//load fonts
	initFont();

	//=============DONE WITH INITIALIZATION==============
	
	struct Buffer* buffer;
	if (argc == 1)
		buffer = createBuffer("test.txt");
	else
		buffer = createBuffer(argv[1]);

	if (buffer == NULL)
	{
		fprintf(stderr, "Could not open file: %s, exiting.\n", argv[1]);
		return 1;
	}


	// char* windowTitle = strcat("TEA - ", buffer->fileName);
	char windowTitle[80] = "tea - ";
	strcat(windowTitle, buffer->fileName);
	SDL_SetWindowTitle(window, windowTitle);
	
	//starting events
	int done = 0;
	SDL_Event e;
	bool shift = false;

	int cursorR = 0;
	int cursorC = 0;

	while(!done)
	{	
		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
				case SDL_QUIT:
					done = 1;
				break;
			}

			//regular typing event appens here
			switch(e.type)
			{
				case SDL_KEYDOWN:
				{
					int keyCode = e.key.keysym.sym;
					char c = getCharFromSDLCode(keyCode, shift);
					if (c != '\0')
					{
						insertIntoBuffer(buffer, cursorR, cursorC++, c);
					}
					switch(keyCode)
					{
						case SDLK_BACKSPACE:
						{
							int x = -1;
							if (cursorR != 0)
							x = buffer->lengths[cursorR - 1];
							bool result = backspace(buffer, cursorR, cursorC);
							if (cursorR == 0)
								break;
								if(result)
								cursorC--;
							else
							{
								cursorR--;
								cursorC = x;
							}
						}
							// else
							// {
							// 	// deleteRow(buffer, cursorR);
							// 	// if (cursorR != 0)
							// 	// 	cursorR --;
							// }
						break;
						case SDLK_DELETE:
							del(buffer, cursorR, cursorC+1);
						break;
						case SDLK_RETURN:
							addRow(buffer, cursorR + 1);
							cursorR++;
							cursorC = 0;
						break;
						case SDLK_LEFT:
							if (cursorC ==0)
							{
								if (cursorR != 0)
								{
									cursorR--;
									cursorC = buffer->lengths[cursorR];
								}
							}
							else
							{
								cursorC--;
							}
						break;
						case SDLK_RIGHT:
							if (cursorC == buffer->lengths[cursorR])
							{
								cursorR++;
								cursorC = 0;
							}
							else
							{
								cursorC++;
							}
						break;
						case SDLK_UP:
							if (cursorR != 0)
							{
								cursorR--;
								cursorC = (cursorC >= buffer->lengths[cursorR]?
									buffer->lengths[cursorR] :
									cursorC);
							}
						break;
						case SDLK_DOWN:
							if (cursorR != buffer->numRows - 1)
							{
								cursorR++;
								cursorC = 
								(cursorC > buffer->lengths[cursorR] ? buffer->lengths[cursorR] : cursorC);
							}
						break;
						case SDLK_END:
							cursorC = buffer->lengths[cursorR];
						break;
						case SDLK_HOME:
							cursorC = 0;
						break;
					
					}
					if(e.key.keysym.sym == SDLK_LSHIFT 
						||e.key.keysym.sym== SDLK_RSHIFT)
					{
						shift = true;
					}
					
				}
				break;
				//keyup
				case SDL_KEYUP:
					if(e.key.keysym.sym == SDLK_LSHIFT ||
							e.key.keysym.sym == SDLK_RSHIFT)
					{
						shift = false;
					}
				break;
			}

		}
		//render loop
		SDL_SetRenderDrawColor(windowRenderer, 25, 25, 50, 255);
		SDL_RenderClear(windowRenderer);

		//rendering takes place here
		// renderBufferToTerminal(buffer, cursorR, cursorC);
		renderBufferToRenderer(buffer, windowRenderer, cursorR, cursorC, 0, 0);

		// SDL_GL_SwapWindow(window);
		SDL_RenderPresent(windowRenderer);
	}

	
	free(logoPixels);
	cleanUpFont();

	saveBuffer(buffer);
	destroyBuffer(buffer);

	SDL_FreeSurface(logoSurface);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}

SDL_Window* init()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return NULL;
	}

	SDL_Window* window = SDL_CreateWindow
	(
		"tea", 
		0, 
		0, 
		SCREEN_WIDTH, 
		SCREEN_HEIGHT, 
		0
	);

	return window;
}


void renderBufferToRenderer(struct Buffer* buffer, SDL_Renderer* dst, 
	int cursorR, int cursorC, int scrollRows, int scrollCols)
{
	static const int BLINK_LENGTH = 60;
	static int numTicks = 0;
	if (numTicks > BLINK_LENGTH)
		numTicks = 0;
	else
		numTicks++;


	if (numTicks <= BLINK_LENGTH/2)
	{
		SDL_Rect cursor;
		cursor.x = cursorC * CHAR_WIDTH;
		cursor.y = cursorR * CHAR_HEIGHT;
		cursor.w = CHAR_WIDTH;
		cursor.h = CHAR_HEIGHT;
		SDL_SetRenderDrawBlendMode(dst, SDL_BLENDMODE_BLEND);
		int alpha = (int)(((float)numTicks)/((float)BLINK_LENGTH) * BLINK_LENGTH * 180);
		SDL_SetRenderDrawColor(dst, 255, 255, 255, alpha);

		SDL_RenderFillRect(dst, &cursor);
	}

	for(unsigned int r = 0; r < buffer->numRows; r++)
	{
		for(unsigned int c = 0; c < buffer->lengths[r]; c++)
		{
			drawGlyph(buffer->rows[r][c], r*CHAR_HEIGHT, c*CHAR_WIDTH, dst);
		}
	}

	// if (numTicks <= BLINK_LENGTH/2)
	// {
	// 	drawGlyph('_', cursorR * CHAR_HEIGHT, cursorC * CHAR_WIDTH, dst);
	// }

	//draw a nice box at the bottom
	SDL_SetRenderDrawColor(dst, 50, 50, 100, 255);
	SDL_Rect bar;
	bar.x = 0;
	bar.y = SCREEN_HEIGHT - CHAR_HEIGHT;
	bar.w = SCREEN_WIDTH;
	bar.h = CHAR_HEIGHT;
	SDL_RenderFillRect(dst, &bar);

	// drawGlyph('r', bar.y, 0*CHAR_WIDTH, dst);
	// drawGlyph(':', bar.y, 1*CHAR_WIDTH, dst);
	char string[80];
	sprintf(string, "r: %d c: %d", cursorR, cursorC);
	drawString(string, bar.y, 0, dst);
}

void renderBufferToTerminal(struct Buffer* buffer, int cursorR, int cursorC)
{
	printf("\033[2J"); // Clear screen
	// printf("\f============\n");
	printf("Num Rows: %d, Max Rows: %d, r: %d, c: %d\n", 
		buffer->numRows, buffer->maxRows, cursorR, cursorC);
	for(int i = 0; i < buffer->numRows; i++)
	{
		printf("MAX_SIZE: %d , length: %d, add %p |", 
			buffer->maxLengths[i], buffer->lengths[i], buffer->rows[i]);
		for(int k = 0; k < buffer->lengths[i]; k++)
		{
			putchar(buffer->rows[i][k]);
		}
		putchar('\n');
	}

}
