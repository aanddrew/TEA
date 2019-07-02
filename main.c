#include <stdio.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "include/Buffer.h"
#include "include/Input.h"
#include "graphics/include/bmploader.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

SDL_Window* init();
void renderBufferToTerminal(struct Buffer* buffer);

int main(int argc, char** argv)
{
	printf("Welcome to TEA - the Text Editor by Andrew\n~~\n");
	
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
	SDL_GLContext context = SDL_GL_CreateContext(window);
	//give it an icon
	unsigned int logoWidth, logoHeight;
	unsigned char* logoPixels = loadbmp("res/logo.bmp", &logoWidth, &logoHeight);

	printf("logo is %ux%u\n", logoWidth, logoHeight);

	// srand(0);
	// for(int i = 0; i < 64*64 * 4; i++)
	// {
	// 	logoPixels[i] = rand()%255;
	// }

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

	for(int y = 0; y < 64; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			printf("%x", (unsigned int)(logoPixels[y*4*64 + x*4]));
			// printf("%c", (logoPixels[y*4*64 + x*4] == 0 ? ' ' : 'A'));
		}
		printf("\n");
	}
	// for(int i = 0; i < 64*64*4; i++)
	// {
	// 	printf("%x", logoPixels[i]);
	// }
	printf("\n");
	//end window creation
	
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
	char windowTitle[80] = "Tea - ";
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
						printf("%c\n", c);
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
		glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//rendering takes place here
		// renderBufferToTerminal(buffer);

		SDL_GL_SwapWindow(window);
	}

	
	free(logoPixels);
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
		"TEA", 
		0, 
		0, 
		SCREEN_WIDTH, 
		SCREEN_HEIGHT, 
		SDL_WINDOW_OPENGL
	);
	SDL_GL_SetSwapInterval(1);

	return window;
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