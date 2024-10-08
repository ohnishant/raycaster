CC = g++
CFLAGS = -Wall -g -Wextra

LIBS = -lm -lGL -lGLU -lglut

all: main.cpp
	$(CC) $(CFLAGS) -o main main.cpp $(LIBS)
