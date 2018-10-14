CFLAGS=-std=c++11 -Wall -g
# CFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0 -std=c++11 -Wall -g

CC=g++
LDFLAGS= -I/usr/local/Cellar/boost/1.67.0_1
LIBS= -L/Users/trenton/Desktop/boost_1_68_0/stage/lib
LINKFLAGS = -lboost_thread-mt -lboost_system-mt

walk: walk.cpp array_printer.o boost_graph_wrapper.o
	$(CC) $(CFLAGS) $(LIBS) $(LINKFLAGS) -o walk walk.cpp array_printer.o boost_graph_wrapper.o
boost_graph_wrapper.o: boost_graph_wrapper.cpp
	$(CC) $(CFLAGS) $(LIBS) $(LINKFLAGS) -c boost_graph_wrapper.cpp
array_printer.o: array_printer.cpp array_printer.h
	$(CC) $(CFLAGS) -c array_printer.cpp array_printer.h

clean: 
	rm -rf *.o *.gch walk
