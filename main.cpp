#include <iostream>
#include <boost/array.hpp>
#include <boost/random.hpp>
#include <boost/graph/random.hpp> 
#include <boost/graph/grid_graph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graphviz.hpp>

#define DIMENSIONS 2
using namespace boost;

typedef unsigned int uint;

typedef grid_graph<DIMENSIONS> GraphType;
typedef graph_traits<GraphType> Traits;
typedef GraphType::vertex_descriptor Vertex;
typedef GraphType::edge_descriptor Edge;

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
	void move(Vertex v)
	{
		
	}
	void update(Vertex v)
	{
		future_step = current_step;

		infect();
		recover();
		move(v);

	}
	void set_current_values(int pop , int ipop , int spop)
	{
		current_step.population = pop;
		current_step.infective_population = ipop;
		current_step.suceptible_population = spop;

	}

	friend std::ostream& operator<<(std::ostream& os, VertexProperties& vp) {
        return os << "{" << vp.current_step.population << "," << vp.current_step.infective_population << "," << vp.current_step.suceptible_population << "}";
    }
    friend std::istream& operator>>(std::istream& is, VertexProperties& vp) {
    	return is >> vp.row >> vp.col >> vp.current_step.population;
        // return is >> vp.current_step.population >> vp.current_step.infective_population >> vp.current_step.suceptible_population;
    }
};

// Define a simple function to print vertices
void print_vertex(Traits::vertex_descriptor vertex_to_print) {
  std::cout << "(" << vertex_to_print[0] << ", " << vertex_to_print[1] << 
  ")" << std::endl;
}

int main(int argc, char* argv[]) {

	static boost::random::mt19937 rng;                                        


	// Define a 2x2 grid_graph where the second dimension doesn't wrap
	boost::array<std::size_t, 2> lengths = { { 2, 2 } };
	boost::array<bool, 2> wrapped = { { false, false } };
	GraphType graph(lengths, wrapped);

	// Get the index map of the grid graph
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

    // SELECT INFECTED
    Vertex init_infected = random_vertex(graph, rng);
    auto init_infected_vp = get(dataMap, init_infected);
    init_infected_vp.set_current_values(1,1,0);
    put(dataMap, init_infected, init_infected_vp );

    
	// // // Get the data at the node at position (0,1) in the grid
	// // std::pair<float,float> retrieved = get(dataMap, v);
	// // std::cout << "Retrieved values: (" << retrieved.first <<
	// // ", " << retrieved.second << ")"<< std::endl;
	// std::cout << (v);

	boost::dynamic_properties dp;
    dp.property("node_id", dataMap);
    boost::write_graphviz_dp(std::cout, graph, dp);
	return 0;
}