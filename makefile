CFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0 -std=c++11 -Wall -g
CC=g++
LIBS=-L/Users/trenton/Desktop/boost_1_68_0/stage/lib

walk: walk.cpp array_printer.o boost_graph_wrapper.o
	$(CC) $(CFLAGS) $(LIBS) -o walk walk.cpp array_printer.o boost_graph_wrapper.o
boost_graph_wrapper.o: boost_graph_wrapper.cpp
	$(CC) $(CFLAGS) -c boost_graph_wrapper.cpp
array_printer.o: array_printer.cpp array_printer.h
	$(CC) $(CFLAGS) -c array_printer.cpp array_printer.h

clean: 
	rm -rf *.o *.gch walk
