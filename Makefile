FLAGS = -lpthread -g 
CC = g++

objects = lizard.cpp

lizard.o: $(objects)
		$(CC) $(objects) -o lizard $(CFLAGS)


.PHONY : clean
	clean: 
		rm lizard $(objects)
