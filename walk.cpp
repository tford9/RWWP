#define _CRT_SECURE_NO_WARNINGS

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

// Boost
#include "boost/graph/graph_utility.hpp"
#include "boost/graph/adjacency_matrix.hpp"
#include "boost/graph/random.hpp"
#include "boost/graph/graphml.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/graph/breadth_first_search.hpp>

//Local
#include "boost_graph_wrapper.cpp"
#include "array_printer.h"

/* End Include Statements */

/* Begin Structs */
// VertexProperties is the property container that will be passed into the 
// templated graph object - should work for serial and parallel versions
struct VertexProperties 
{
	unsigned int ID;
	std::string name;
};
// EdgeProperties is the property container that will be passed into the 
// templated graph object - should work for serial and parallel versions
struct EdgeProperties
{
	double prob = 1.0;
	unsigned int trav_count = 0;	
};

/* End Structs */

/* Begin Typedefs */

typedef unsigned int uint;  
typedef std::vector<std::vector<std::string>> edgeMatrix;

typedef Graph<VertexProperties, EdgeProperties> initGraphType;
typedef initGraphType::Vertex VertexID;

typedef typename graph_traits<initGraphType::GraphContainer>::edge_descriptor Edge;
typedef typename graph_traits<initGraphType::GraphContainer>::vertex_descriptor Vertex;

/* End Typedefs */

// Function Prototypes
edgeMatrix readEdgeList(std::string fileName);

// Global graph object
initGraphType g;
initGraphType::GraphContainer G;

/* Begin Class Defs*/
class MyVisitor : public boost::default_dfs_visitor
{
public:
	void discover_vertex(Vertex v, const initGraphType::GraphContainer& g) const
	{
		cerr << v << endl;
		return;
	}
};
/* End Class Defs*/

int main() {

	std::string filename;

	std::cout << "Using Boost "     
          << BOOST_VERSION / 100000     << "."  // major version
          << BOOST_VERSION / 100 % 1000 << "."  // minor version
          << BOOST_VERSION % 100                // patch level
          << std::endl;

	// Prompt for edgelist
	std::cout << "Enter Edgelist Location\n";
	// Store Results
	std::cin >> filename;
	std::cout << "The filename you chose is: " << filename << std::endl;
	edgeMatrix edgemat;
	try{
		edgemat = readEdgeList(filename);	
	}catch(...) {
		std::cerr << "File Read Failed" << std::endl;
	}

	for (auto row = edgemat.begin(); row != edgemat.end(); ++row ) {
		// tracks vertices added to the graph
		std::map<std::string, VertexID> name_to_Vertex_map;

		VertexProperties vp01, vp02;
		EdgeProperties ep01, ep02;
		initGraphType::Vertex v01, v02;

		
		vp01.ID = std::stoi((*row)[0]);
		vp02.ID = std::stoi((*row)[1]);

		vp01.name = (*row)[0];
		vp02.name = (*row)[1];


		// Check if source vertex is already in the graph
		if (name_to_Vertex_map.find(vp01.name) == name_to_Vertex_map.end())
		{
			v01 = g.AddVertex(vp01);
			name_to_Vertex_map.insert(std::pair<std::string, VertexID>(vp01.name, v01));
		}
		else
		{
			v01 = (name_to_Vertex_map.find(vp01.name))->second;
		}

		// Check if target vertex is already in the graph
		if (name_to_Vertex_map.find(vp02.name) == name_to_Vertex_map.end())
		{
			v02 = g.AddVertex(vp02);
			name_to_Vertex_map.insert(std::pair<std::string, VertexID>(vp02.name, v02));
		}
		else
		{
			v02 = (name_to_Vertex_map.find(vp02.name))->second;
		}

		g.AddEdge(v01, v02, ep01, ep02);
	}

	//MyVisitor vis;
	//boost::depth_first_search(g, boost::visitor(vis));

	std::cout << *(boost::vertices(G).first) << *(boost::vertices(G).second);

	return 0;

// 		boost::write_graphml(outGraphFile2, G, dp, true);
// 	}
}

edgeMatrix readEdgeList(std::string fileName) {

	edgeMatrix edgemat;

	std::string line;
	int skippedLines = 0;
	int idx = 0;

	// this assumes a standard edgelist format
	// and ignores commented lines
	std::ifstream inFile(fileName);
	while (std::getline(inFile, line))
	{
		if (idx == 0) {
			std::cout << "Edgelist file opened.." << std::endl;
		}
		idx++;
		//std::cout << line.length() << std::endl;
		// skip commented or empty lines
		if (line.length() == 0 || line[0] == ('#')) {
			skippedLines += 1;
			continue;
		} else {
			std::vector<std::string> row;

			char *cstr = new char[line.length() + 1];
			std::strcpy(cstr, line.c_str());
			auto pch = std::strtok(cstr, "\t");
			while (pch != NULL) {
				row.push_back(pch);
				pch = std::strtok(NULL, "\t");
			}
			
		    edgemat.push_back(row);
		}
	}
	inFile.close();

	std::cout << "File read complete\n Lines skipped: " << skippedLines << std::endl;
	std::cout << "The number of edges read: " << edgemat.size() << std::endl;

	return edgemat;
}
