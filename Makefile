#the files to compile as part of project
OBJS = main.c src/*.c graphics/src/*.c

#the compiler we will use
CC = gcc

# the flags for the compiler 
#-w surpresses all warnings
COMPILER_FLAGS = -g -std=c99

#the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lGL -lGLEW

#the name of our executable
OBJ_NAME = a.out

all:
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
