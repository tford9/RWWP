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

typedef Graph<VertexProperties, EdgeProperties> GraphType;
typedef GraphType::Vertex VertexID;

typedef typename graph_traits<GraphType::GraphContainer>::edge_descriptor Edge;
typedef typename graph_traits<GraphType::GraphContainer>::vertex_descriptor Vertex;


class MyVisitor : public boost::default_dfs_visitor
{

public:
	void discover_vertex(Vertex v, const GraphType::GraphContainer& g) const
	{
		// auto vp = get(dataMap, v);
		std::cout << "I see dead people!" << std::endl;
		// std::cerr << v << std::endl;
		// return;
	}
};

void loadGraphML(std::string filename, GraphType::GraphContainer g){

    std::ifstream inFile;
    inFile.open(filename, std::ifstream::in);

    boost::dynamic_properties dp;
	boost::read_graphml(inFile, g);
}

int main(int argc, char* argv[]) {

	static boost::random::mt19937 rng;           

	GraphType g;

	std::string filename = "../graph_data/processed/web-NotreDame.graphml";

	loadGraphML(filename, g.graph);
	
	typedef boost::property_map<GraphType, boost::vertex_index_t>::const_type indexMapType;
	indexMapType indexMap(get(boost::vertex_index, graph));

	// Create a float for every node in the graph
	boost::vector_property_map<VertexProperties , indexMapType> dataMap(num_vertices(graph), indexMap);


	// Associate the value 2.0 with the node at position (0,0) in the grid
	// Vertex v = { { 0, 0 } };
	// VertexProperties vert;
	// put(dataMap, v, vert);

	// INIT VERTEXPROPERTIES
    for (uint i = 0; i < 2; ++i)
    	for (uint j = 0; j < 2; ++j)
put(dataMap, Traits::vertex_descriptor {{i, j}}, VertexProperties{i,j,1});


    MyVisitor vis;
	boost::depth_first_search(g.graph, boost::visitor(vis));

	return 0;
}