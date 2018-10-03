 /*
* Author: Trenton Ford
* Date: Oct 10, 2017
*/

/* Begin Include Statements */
#include <string>
#include <vector> 
#include <array>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "boost/graph/graph_utility.hpp"
#include "boost/graph/adjacency_matrix.hpp"
#include "boost/graph/random.hpp"
#include "boost/graph/graphml.hpp"
#include <boost/lexical_cast.hpp>


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
// typedef boost::adjacency_list<boost::vecS,
// 							boost::vecS,
// 							boost::directedS,
// 							VertexProperties,
// 							EdgeProperties
// 										> Graph;
typedef Graph<VertexProperties, EdgeProperties> MyGraph;
typedef MyGraph::Vertex VertexID;


/* End Typedefs */

std::map<std::string, VertexID> name_to_Vertex_map;


// Function Prototypes
void readFile();
VertexID step(VertexID current_node, int* total_count);

// Random Number Generator
static std::mt19937 gen { std::random_device{}() };
std::uniform_real_distribution<double> ud(0.0,1.0);

// Global containers for file I/O manipulation 
std::vector<std::array<std::string, 7> > graph_input_data;

// Global graph object
MyGraph g;
MyGraph::GraphContainer G;
typedef typename graph_traits<MyGraph::GraphContainer>::edge_descriptor Edge;

namespace boost {
	template<>
    std::string lexical_cast(const VertexProperties& vp) 
    {
        return vp.name;
    }
    template<>
    VertexProperties lexical_cast(const std::basic_string<char> & s) 
    {
    	VertexProperties vp;
    	
        return vp;
    }
	std::istream& operator>>(std::istream& is, const VertexProperties& vp)
	{
		std::string temp;
	    is >> temp;

	    return is;
	}
	std::ostream& operator<<(std::ostream& os, const VertexProperties& vp)
	{
	    os << vp.name << vp.ID;
	    return os;
	}
    // #########################
    template<>
    std::string lexical_cast(const EdgeProperties& ep) 
    {
        return std::to_string(ep.trav_count);
    }

    template<>
    EdgeProperties lexical_cast(const std::basic_string<char> & s) 
    {
    	EdgeProperties ep;
    	
        return ep;
    }
    std::istream& operator>>(std::istream& is, const EdgeProperties& vp)
	{
		std::string temp;
	    is >> temp;

	    return is;
	}
	std::ostream& operator<<(std::ostream& os, const EdgeProperties& ep)
	{
	    os << ep.trav_count;
	    return os;
	}
    // #########################
}

int main() {

	std::cout << "Using Boost "     
          << BOOST_VERSION / 100000     << "."  // major version
          << BOOST_VERSION / 100 % 1000 << "."  // minor version
          << BOOST_VERSION % 100                // patch level
          << std::endl;

	try{
		readFile();	
	}catch(...) {
		std::cerr << "File Read Failed" << std::endl;
	}
	std::ofstream outGraphFile1("org_graph.dot");
	std::ofstream outGraphFile2("org_graph.graphml");
	// For loop takes read in data and adds it to the graph object

	for( auto it = graph_input_data.begin(); it != graph_input_data.end(); ++it) {
		// Add forward directional edge and attribute 
		VertexProperties vp01, vp02;
		EdgeProperties ep01, ep02;
		MyGraph::Vertex v01, v02;

		vp01.ID = std::stoi((*it)[5]);
		vp02.ID = std::stoi((*it)[6]);

		vp01.name = (*it)[1];
		vp02.name = (*it)[2];

		if(name_to_Vertex_map.find(vp01.name) == name_to_Vertex_map.end())
		{
			v01 = g.AddVertex(vp01);
			name_to_Vertex_map.insert(std::pair<std::string, VertexID>(vp01.name,v01));
		} else 
		{
			v01 = (name_to_Vertex_map.find(vp01.name))->second;
		}

		if(name_to_Vertex_map.find(vp02.name) == name_to_Vertex_map.end())
		{
			v02 = g.AddVertex(vp02);
			name_to_Vertex_map.insert(std::pair<std::string, VertexID>(vp02.name,v02));
		} else 
		{
			v02 = (name_to_Vertex_map.find(vp02.name))->second;
		}

		ep01.prob = std::stod((*it)[4]);
		ep02.prob = std::stod((*it)[5]);

		ep01.trav_count = 0;
		ep02.trav_count = 0;

		g.AddEdge(v01, v02, ep01, ep02);
		
	}
	

  	// define the number of steps to take
  	uint steps = 100;
  	int total_count = 0;
  	int* ptr_total_count = &total_count;

	std::cout << g.getVertexCount() << std::endl << "How many steps to take?" << std::endl;
//	std::cin >> steps;

	G = g.getGraph();
	
	// Select a random starting node from the graph
  	auto v01 = random_vertex(G, gen);
	std::cout << "Starting Node: " << v01 << std::endl; 

	for(uint i = 0; i < steps; i++) {
		std::cout << "Walk from " << g.properties(v01).name; 
		v01 = step(v01, ptr_total_count);
		std::cout <<  " to " << g.properties(v01).name << std::endl;
	}
	std::cout << "Total Count = " << total_count << std::endl;
	{
		using namespace boost;

		boost::dynamic_properties dp(boost::ignore_other_properties);;
		dp.property("gene_label", get(vertex_properties_t(), G));
		dp.property("walk_count", get(edge_properties_t(), G));

		boost::write_graphml(outGraphFile2, G, dp, true);
	}
	outGraphFile1.close();
	outGraphFile2.close();
}

VertexID step(VertexID current_node, int* total_count) {	

    double random_number = ud(gen);

    // std::cout << "\nRandom Number: " << random_number << std::endl;
	graph_traits<MyGraph::GraphContainer>::out_edge_iterator eit, eend;
  	std::tie(eit, eend) = boost::out_edges( current_node, G );

  	for( auto it = eit; it != eend; ++it) {
  		std::cout << "Random Number: " << random_number << std::endl;
  		random_number -= g.properties(*it).prob;
  		// std::cout << "Edge prob: " << g[*it].prob << std::endl;

  		if (random_number <= 0) {

			g.properties(*it).trav_count++;
  			return boost::target ( *it, G );

  		}
  	}

  	std::cout << "No edge found!! Preparing to jump. " << std::endl;

  	return random_vertex(G, gen);;

}

void readFile() {

	std::ifstream inFile("pearson_size_match_sorted_indexed.csv");

	std::array<std::string, 7> line_array;
	std::string line;

	while (std::getline(inFile, line))
	{
	    inFile >> line_array[0] >> line_array[1] >> line_array[2] >> line_array[3] >> line_array[4] >> line_array[5] >> line_array[6];
	    graph_input_data.push_back(line_array);
	}
	inFile.close();
	std::cout << "File read successful!" << std::endl;
}
