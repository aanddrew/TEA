#include <stdio.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "include/Buffer.h"
#include "include/Input.h"
#include "graphics/include/bmploader.h"
#include "graphics/include/font.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Window* init();
void renderBufferToTerminal(struct Buffer* buffer);
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
	unsigned char* logoPixels = loadbmp("res/icon_256.bmp", &logoWidth, &logoHeight);
	fixPixelsForSDL(logoPixels, logoWidth, logoHeight);

	printf("icon is %ux%u\n", logoWidth, logoHeight);

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

	SDL_Surface* windowSurface = SDL_GetWindowSurface(window);
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

	printf("buffer info: \n");
	printf("File: %s\n", buffer->fileName);
	printf("contents:\n");
	for(int i = 0; i < buffer->numRows; i++)
	{
		printf("%s\n", buffer->rows[i]);
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
				//keydown
				case SDL_KEYDOWN:
				{
					int keyCode = e.key.keysym.sym;
					if(keyCode == SDLK_LSHIFT 
						||keyCode == SDLK_RSHIFT)
					{
						shift = true;
					}
					char c = getCharFromSDLCode(keyCode, shift);
					if (c != '\0')
					{
						insertIntoBuffer(buffer, cursorR, cursorC++, c);
					}

					switch(keyCode)
					{
						case SDLK_BACKSPACE:
							if(backspace(buffer, cursorR, cursorC))
								cursorC--;
						break;
						case SDLK_DELETE:
							del(buffer, cursorR, cursorC+1);
						break;
						case SDLK_RETURN:
							addRow(buffer, cursorR);
							cursorR++;
							cursorC = 0;
						break;
						case SDLK_LEFT:
							if (cursorC ==0)
							{
								if (cursorR != 0)
								{
									cursorR--;
									cursorC = buffer->lengths[cursorR] - 1;
								}
							}
							else
							{
								cursorC--;
							}
						break;
						case SDLK_RIGHT:
							if (cursorC == buffer->lengths[cursorR] - 1)
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
							if (cursorR != buffer->maxRows - 1)
							{
								cursorR++;
								cursorC = (cursorC > buffer->lengths[cursorR] - 1 ?
									buffer->lengths[cursorR] - 1 :
									cursorC);
							}
						break;
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
		SDL_RenderClear(windowRenderer);
		// glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
		// glClear(GL_COLOR_BUFFER_BIT);

		//rendering takes place here
		// renderBufferToTerminal(buffer);
		renderBufferToRenderer(buffer, windowRenderer, cursorR, cursorC, 0, 0);

		// SDL_GL_SwapWindow(window);
		SDL_RenderPresent(windowRenderer);
	}

	
	free(logoPixels);
	cleanUpFont();
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

	// SDL_GL_SetSwapInterval(1);

	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);

	// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window* window = SDL_CreateWindow
	(
		"tea", 
		0, 
		0, 
		SCREEN_WIDTH, 
		SCREEN_HEIGHT, 
		SDL_WINDOW_OPENGL
	);

	// SDL_GLContext context = SDL_GL_CreateContext(window);

	//this is critical, seg fault happens without it
	// glewExperimental = GL_TRUE; 
	// glewInit();

	return window;
}

void renderBufferToRenderer(struct Buffer* buffer, SDL_Renderer* dst, 
	int cursorR, int cursorC, int scrollRows, int scrollCols)
{
	static const int BLINK_LENGTH = 30;
	static int numTicks = 0;
	if (numTicks > BLINK_LENGTH)
		numTicks = 0;
	else
		numTicks++;

	// printf("%d, %d\n", buffer->numRows, buffer->maxRows);

	for(unsigned int r = 0; r < buffer->numRows; r++)
	{
		for(unsigned int c = 0; c < buffer->lengths[r]; c++)
		{
			drawGlyph(buffer->rows[r][c], r, c, dst);
		}
	}

	if (numTicks <= BLINK_LENGTH/2)
	{
		drawGlyph('_', cursorR, cursorC, dst);
	}
}

void renderBufferToTerminal(struct Buffer* buffer)
{
	printf("\033[2J"); // Clear screen
	// printf("\f============\n");
	printf("Num Rows: %d, Max Rows: %d\n", buffer->numRows, buffer->maxRows);
	for(int i = 0; i < buffer->numRows; i++)
	{
		printf("MAX_SIZE: %d , length: %d |", buffer->maxLengths[i], buffer->lengths[i]);
		for(int k = 0; k < buffer->lengths[i]; k++)
		{
			printf("%c", buffer->rows[i][k]);
		}
		printf("\n");
	}

}