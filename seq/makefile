CFLAGS=-std=c++11 -Wall -g
# CFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0 -std=c++11 -Wall -g

CC=g++
LDFLAGS= -I/Users/trentonford/Desktop/KERNAL_PROJECT_SPACE/usr/include

LIBS= -L/Users/trentonford/Desktop/KERNAL_PROJECT_SPACE/usr/lib
LINKFLAGS = -lboost_graph_parallel -lboost_mpi -lboost_system -lboost_serialization -lboost_thread -lboost_graph

main: main.cpp utilities.o
	$(CC) $(CFLAGS) $(LIBS) $(LINKFLAGS) -o main main.cpp utilities.o
boost_graph_wrapper.o: boost_graph_wrapper.cpp
	$(CC) $(CFLAGS) $(LIBS) $(LINKFLAGS) -c boost_graph_wrapper.cpp
utilities.o: read_graph_from_dot.cpp utilities.h
	$(CC) $(CFLAGS) -o utilities.o -c read_graph_from_dot.cpp


clean: 
	rm -rf *.o *.gch main
