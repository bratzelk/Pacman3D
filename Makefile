# Makefile for Pacman #D
# BY KIM BRATZEL
#

## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS 	=-framework GLUT -framework OpenGL -framework Cocoa -D OSX -std=c99 -g
#CFLAGS =-framework GLUT -framework OpenGL -framework Cocoa -D OSX -std=c99 -g


## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC =		Main.c
OBJ =		Main.o timer.o DrawPacman.o DrawGhost.o camera.o smalloc.o includes.o readfile.o
EXE = 		Pacman

## Top level target is executable.
all:	$(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -lm

$(EXE):	$(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -lm



## Clean: Remove object files and core dump files.
run:
	 make clean
	 make
	 ./Pacman
	 
## Clean: Remove object files and core dump files.
clean:
	 \rm -f *.o main

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE)

## Dependencies

# Linking the objects
smalloc.o: Makefile
DrawPacman.o: Makefile
DrawGhost.o: Makefile
includes.o: smalloc.h Makefile
readfile.o: smalloc.h includes.h Makefile
Main.o: timer.h camera.h readfile.o smalloc.h includes.h Makefile
camera.o: camera.h Makefile
timer.o: Timer.h Makefile



