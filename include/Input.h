#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

#include <SDL2/SDL.h>
#include "Buffer.h"

char getCharFromSDLCode(int keycode, bool shift);


#endif