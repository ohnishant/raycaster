CC = g++
CFLAGS = -Wall -g -Wextra

LIBS = -lm -lGL -lGLU -lglut
RLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: main.cpp
	$(CC) $(CFLAGS) -o main main.cpp $(LIBS)

rl: rl.c
	$(CC) $(CFLAGS) -o rl rl.c $(RLIBS)
