objects = game.o board.o

game : $(objects)
	g++ -o four $(objects)

.PHONY : clean
clean :
	rm $(objects)

debug : $(objects)
	g++ -ggdb -o four_debug $(objects)
