#include <iostream>
#include <boost/array.hpp>
#include <boost/graph/grid_graph.hpp>
#include <boost/random.hpp>

#define DIMENSIONS 2
using namespace boost;

typedef unsigned int uint;

typedef grid_graph<DIMENSIONS> GraphType;
typedef graph_traits<GraphType> Traits;
typedef GraphType::vertex_descriptor Vertex;
typedef GraphType::edge_descriptor Edge;

struct VertexProperties 
{
	static boost::random::mt19937 rng;                                        
    static boost::random::uniform_real_distribution<double> gen; 

    static constexpr float p = 0.5; // Infection Probability
	static constexpr float lamb = 0.5; // Movement Probability
	static constexpr float mu = 0.25; // Recovery Probability
	VertexProperties() 
	{ 
		static boost::random::uniform_real_distribution<double> gen(0.0, 1.0); 
	}

	struct step_data
	{
		int population = 1;
		int infective_population = 0;
		int suceptible_population = 1;
	};

	step_data current_step;
	step_data future_step;

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
	void move()
	{
		//
	}
	void update()
	{
		future_step = current_step;

		infect();
		recover();
		move();

	}

};

// Define a simple function to print vertices
void print_vertex(Traits::vertex_descriptor vertex_to_print) {
  std::cout << "(" << vertex_to_print[0] << ", " << vertex_to_print[1] << 
  ")" << std::endl;
}

int main(int argc, char* argv[]) {

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
  // put(dataMap, v, std::make_pair(0.0,0.99));

  // // Get the data at the node at position (0,1) in the grid
  // std::pair<float,float> retrieved = get(dataMap, v);
  // std::cout << "Retrieved values: (" << retrieved.first <<
  // ", " << retrieved.second << ")"<< std::endl;

  VertexProperties vert;

  std::cout << (vert.current_step.population);

  return 0;
}