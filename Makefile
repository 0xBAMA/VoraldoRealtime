CC = g++

MAKE_EXE=-o exe

LIBS = -lglut -lGLEW -lGL -lGLU
FLAGS = -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -time

all: main.cc
	$(CC) main.cc  $(MAKE_EXE) $(LIBS) $(FLAGS)
