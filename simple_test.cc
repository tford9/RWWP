#include <iostream>
#include <string>
#include <vector>

#include <boost/mpi.hpp>

// Enable PBGL interfaces to BGL algorithms
#include <boost/graph/use_mpi.hpp>
// Communicate via MPI
#include <boost/graph/distributed/mpi_process_group.hpp>
// Distributed adjacency list
#include <boost/graph/distributed/adjacency_list.hpp>
// Graphviz Output
#include <boost/graph/distributed/graphviz.hpp>


using namespace std;

int main()
{
	cout << "Hello World!" << endl;
	return 0;
}