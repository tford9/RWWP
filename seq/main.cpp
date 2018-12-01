#include <iostream>
#include <fstream>
#include "s_types.h"

#include <boost/array.hpp>
#include <boost/random.hpp>
#include <boost/graph/random.hpp> 
#include <boost/graph/grid_graph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graphml.hpp>

// Boost Graph Algorithms
#include <boost/graph/depth_first_search.hpp>

//Local
#include "boost_graph_wrapper.cpp"
#include "array_printer.h"

using namespace boost;

struct VertexProperties 
{
	struct step_data
	{
		int population = 1;
		int infective_population = 0;
		int suceptible_population = 1;
	};

	step_data current_step;
	step_data future_step;

	int ID;
	std::string name;

	uint row = 0;
	uint col = 0;

	static boost::random::mt19937 rng;                                        
    static boost::random::uniform_real_distribution<double> gen; 

    static constexpr float p = 0.5; // Infection Probability
	static constexpr float lamb = 0.5; // Movement Probability
	static constexpr float mu = 0.25; // Recovery Probability

	VertexProperties() 
	{ 
		static boost::random::uniform_real_distribution<double> gen(0.0, 1.0); 
	}

	VertexProperties(uint r, uint c, uint pop) 
	{ 
		static boost::random::uniform_real_distribution<double> gen(0.0, 1.0); 
		row = r;
		col = c;
		current_step.population = pop;
	}

	void infect()
	{
		for (uint i = 0; i < current_step.infective_population; i++)
		{	
			// Potential infection cycle
			for (uint j = 0; j < current_step.suceptible_population; j++)
			{
				if (gen(rng) < p)
				{
					future_step.suceptible_population -= 1;
					future_step.infective_population += 1;
				}
			}
		}
		future_step.population = future_step.suceptible_population + future_step.infective_population;
	}
	void recover()
	{
		for (uint i = 0; i < current_step.infective_population; i++)
		{	
			// Random Recovery
			if (gen(rng) < mu)
			{
				future_step.suceptible_population += 1;
				future_step.infective_population -= 1;
			}
		}
		future_step.population = future_step.suceptible_population + future_step.infective_population;
	}

	void update()
	{

		infect();
		recover();

	}
	void set_current_values(int pop , int ipop , int spop)
	{
		current_step.population = pop;
		current_step.infective_population = ipop;
		current_step.suceptible_population = spop;

	}
	void adjust_future_values(int pop = 0, int ipop = 0, int spop = 0) {
		future_step.population += pop;
		future_step.infective_population += ipop;
		future_step.suceptible_population += spop;
	}

	friend std::ostream& operator<<(std::ostream& os, VertexProperties& vp) {
        return os << "{" << vp.current_step.population << "," << vp.current_step.infective_population << "," << vp.current_step.suceptible_population << "}";
    }
    friend std::istream& operator>>(std::istream& is, VertexProperties& vp) {
    	return is >> vp.row >> vp.col >> vp.current_step.population;
        // return is >> vp.current_step.population >> vp.current_step.infective_population >> vp.current_step.suceptible_population;
    }
    // Move future data, to current data and prepare to update
    void advance_timestep()
    {
    	current_step = future_step;
    }
};

struct EdgeProperties
{
	uint weight = 1;
};

typedef std::vector<std::vector<std::string>> edgeMatrix;
typedef Graph<VertexProperties, EdgeProperties> GraphType;
typedef GraphType::Vertex VertexID;

typedef typename graph_traits<GraphType::GraphContainer>::edge_descriptor Edge;
typedef typename graph_traits<GraphType::GraphContainer>::vertex_descriptor Vertex;

std::map<std::string, VertexID> name_to_Vertex_map;
class MyVisitor : public boost::default_dfs_visitor
{

public:
	void discover_vertex(Vertex v, const GraphType::GraphContainer& g) const
	{
		typename property_map<GraphType::GraphContainer, vertex_properties_t>::const_type param = get(vertex_properties, g);
		auto vp = param[v];
		// std::cout << "I see dead people!" << std::endl;
		std::cerr << vp << vp.ID << std::endl;
		// return;
	}
};

// void loadGraphML(std::string filename, GraphType::GraphContainer g){

//     std::ifstream inFile;
//     inFile.open(filename, std::ifstream::in);

//     boost::dynamic_properties dp;
// 	boost::read_graphml(inFile, g);
// }

edgeMatrix readEdgeList(std::string fileName);

// Global graph object
GraphType g;
GraphType::GraphContainer G;

int main() {
	std::cout << "Using Boost "     
          << BOOST_VERSION / 100000     << "."  // major version
          << BOOST_VERSION / 100 % 1000 << "."  // minor version
          << BOOST_VERSION % 100                // patch level
          << std::endl;

	std::string filename = "../graph_data/raw/g1.txt";

	edgeMatrix edgemat;
	try{
		edgemat = readEdgeList(filename);	
	}catch(...) {
		std::cerr << "File Read Failed" << std::endl;
	}

	for (auto row = edgemat.begin(); row != edgemat.end(); ++row ) {

		VertexProperties vp01, vp02;
		EdgeProperties ep01, ep02;
		GraphType::Vertex v01, v02;

		vp01.ID = std::stoi((*row)[0]);
		vp02.ID = std::stoi((*row)[1]);

		vp01.name = (*row)[0];
		vp02.name = (*row)[1];

		// Check if source vertex is already in the graph
		{
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

			g.AddDirectedEdge(v01, v02, ep01);
		}
	}

	MyVisitor vis;
	boost::depth_first_search(g.graph, boost::visitor(vis));

	// std::cout << *(boost::vertices(G).first) << *(boost::vertices(G).second);

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