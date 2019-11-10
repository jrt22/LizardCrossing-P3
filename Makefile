FLAGS = -g -Wall -std=c++11 
CC = g++
CFLAGS = -lpthread

objects = lizard.cpp

lizard.o: $(objects)
		$(CC) $(objects) -o lizard $(CFLAGS)


.PHONY : clean
	clean: 
		rm lizard $(objects)
