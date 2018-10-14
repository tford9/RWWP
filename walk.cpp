 /*
* Author: Trenton Ford
* Date: Oct 10, 2017
*/

/* Begin Include Statements */
// STD
#include <string>
#include <vector> 
#include <array>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

//Boost
#include "boost/graph/graph_utility.hpp"
#include "boost/graph/adjacency_matrix.hpp"
#include "boost/graph/random.hpp"
#include "boost/graph/graphml.hpp"
#include <boost/lexical_cast.hpp>

//Local
#include "boost_graph_wrapper.cpp"
#include "array_printer.h"

/* End Include Statements */

/* Begin Structs */

struct VertexProperties 
{
	unsigned int ID;
	std::string name;
};

struct EdgeProperties
{
	double prob = 0.0;
	unsigned int trav_count = 0;	
};

/* End Structs */

/* Begin Typedefs */

typedef unsigned int uint;  
typedef std::vector<std::array<std::string, 2>> edgeMatrix;
typedef Graph<VertexProperties, EdgeProperties> initGraphType;
typedef initGraphType::Vertex VertexID;
typedef typename graph_traits<initGraphType::GraphContainer>::edge_descriptor Edge;

/* End Typedefs */

// Function Prototypes
edgeMatrix readEdgeList(std::string fileName);
VertexID step(VertexID current_node, int* total_count);

// Global graph object
initGraphType g;
initGraphType::GraphContainer G;


int main() {

	std::cout << "Using Boost "     
          << BOOST_VERSION / 100000     << "."  // major version
          << BOOST_VERSION / 100 % 1000 << "."  // minor version
          << BOOST_VERSION % 100                // patch level
          << std::endl;

	try{
		readEdgeList("web-NotreDame.txt");	
	}catch(...) {
		std::cerr << "File Read Failed" << std::endl;
	}

// 		boost::write_graphml(outGraphFile2, G, dp, true);
// 	}
}

edgeMatrix readEdgeList(std::string fileName) {

	std::ifstream inFile(fileName);
	// this assumes a standard edgelist format
	// and ignores commented lines
	edgeMatrix edgemat;
	std::array<std::string, 2> row;

	std::string line;
	int skippedLines = 0;

	while (std::getline(inFile, line))
	{
		// skip commented or empty lines
		if (line.length() == 0 || line[0] == ('#') ) {
			skippedLines += 1;
			continue;
		} else {
		    inFile >> row[0] >> row[1];
		    edgemat.push_back(row);
		}
	}
	inFile.close();
	std::cout << "File read complete\n Lines skipped: " << skippedLines << std::endl;
	return edgemat;
}
