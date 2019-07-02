#include "../include/Input.h"

char getAlphabeticalChar(int keyCode);

char getCharFromSDLCode(int keycode, bool shift)
{
	switch(keycode)
	{
		case SDLK_SPACE:
			return ' ';
		break;
	}

	//shift modifier keys
	if (!shift)//shift not pressed
	{
		if (keycode >= SDLK_0 && keycode <= SDLK_9)
		{
			return '0' + (keycode - SDLK_0);
		}

		char a = getAlphabeticalChar(keycode);
		if (a != '\0')
		{
			return a;
		}
		//checking the rest of our ascii set
		switch(keycode)
		{
			case SDLK_COMMA:
				return ',';
			break;
			case SDLK_PERIOD:
				return '.';
			break;
			case SDLK_SLASH:
				return '/';
			break;
			case SDLK_SEMICOLON:
				return ';';
			break;
			case SDLK_QUOTE:
				return '\'';
			break;
			case SDLK_LEFTBRACKET:
				return '[';
			break;
			case SDLK_RIGHTBRACKET:
				return ']';
			break;
			case SDLK_MINUS:
				return '-';
			break;
			case SDLK_EQUALS:
				return '=';
			break;
			case SDLK_BACKQUOTE:
				return '`';
			break;
		}
	}
	else//shift pressed
	{
		char a = getAlphabeticalChar(keycode);
		if (a != '\0')
		{
			return (a - ('a'-'A'));
		}

		//checking the rest of our ascii set
		switch(keycode)
		{
			case SDLK_COMMA:
				return '<';
			break;
			case SDLK_PERIOD:
				return '>';
			break;
			case SDLK_SLASH:
				return '?';
			break;
			case SDLK_SEMICOLON:
				return ':';
			break;
			case SDLK_QUOTE:
				return '\"';
			break;
			case SDLK_LEFTBRACKET:
				return '{';
			break;
			case SDLK_RIGHTBRACKET:
				return '}';
			break;
			case SDLK_MINUS:
				return '_';
			break;
			case SDLK_EQUALS:
				return '+';
			break;
			case SDLK_BACKQUOTE:
				return '~';
			break;
			//number keys
			case SDLK_1:
				return '!';
			break;
			case SDLK_2:
				return '@';
			break;
			case SDLK_3:
				return '#';
			break;
			case SDLK_4:
				return '$';
			break;
			case SDLK_5:
				return '%';
			break;
			case SDLK_6:
				return '^';
			break;
			case SDLK_7:
				return '&';
			break;
			case SDLK_8:
				return '*';
			break;
			case SDLK_9:
				return '(';
			break;
			case SDLK_0:
				return ')';
			break;
		}
	}

	return '\0';
}

char getAlphabeticalChar(int keycode)
{
	switch(keycode)
	{
		case SDLK_a:
			return 'a';
		break;
		case SDLK_b:
			return 'b';
		break;
		case SDLK_c:
			return 'c';
		break;
		case SDLK_d:
			return 'd';
		break;
		case SDLK_e:
			return 'e';
		break;
		case SDLK_f:
			return 'f';
		break;
		case SDLK_g:
			return 'g';
		break;
		case SDLK_h:
			return 'h';
		break;
		case SDLK_i:
			return 'i';
		break;
		case SDLK_j:
			return 'j';
		break;
		case SDLK_k:
			return 'k';
		break;
		case SDLK_l:
			return 'l';
		break;
		case SDLK_m:
			return 'm';
		break;
		case SDLK_n:
			return 'n';
		break;
		case SDLK_o:
			return 'o';
		break;
		case SDLK_p:
			return 'p';
		break;
		case SDLK_q:
			return 'q';
		break;
		case SDLK_r:
			return 'r';
		break;
		case SDLK_s:
			return 's';
		break;
		case SDLK_t:
			return 't';
		break;
		case SDLK_u:
			return 'u';
		break;
		case SDLK_v:
			return 'v';
		break;
		case SDLK_w:
			return 'w';
		break;
		case SDLK_x:
			return 'x';
		break;
		case SDLK_y:
			return 'y';
		break;
		case SDLK_z:
			return 'z';
		break;
	}

	return '\0';
}