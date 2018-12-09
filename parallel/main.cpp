#include <iostream>
#include <random>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <boost/mpi.hpp>
#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/distributed/depth_first_search.hpp>

#include <boost/array.hpp>
#include <boost/random.hpp>
#include <boost/graph/random.hpp> 
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/random/linear_congruential.hpp>

#include "s_types.h"
#include "boost_graph_wrapper.cpp"

struct VertexProperties 
{


	VertexProperties() {}

	VertexProperties(int id, std::string name = "node") 
	: id_(id), name(name) 
	{ 

	}

	VertexProperties(int pop , int ipop , int spop)
	{ 
		set_current_values(pop, ipop, spop);
	}

	void infect()
	{
		std::random_device rd;  //Will be used to obtain a seed for the random number engine
    	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_real_distribution<> dis(0.0, 1.0);

		// std::cerr << "infect()" << std::endl;

		for (uint i = 0; i < current_step.infective_population; i++)
		{	
			// std::cerr << "infect() for()" << std::endl;
			// Potential infection cycle
			for (uint j = 0; j < current_step.suceptible_population; j++)
			{
				// std::cerr << "infect() for() for()" << std::endl;
				if (dis(gen) < p)
				{
					// std::cerr << "New Infection!" << std::endl;
					current_step.suceptible_population -= 1;
					current_step.infective_population += 1;
				}
			}
		}
		current_step.population = current_step.suceptible_population + current_step.infective_population;
	}
	void recover()
	{
		std::random_device rd;  //Will be used to obtain a seed for the random number engine
    	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_real_distribution<> dis(0.0, 1.0);

		// std::cout << "recover()" << std::endl;	

		for (uint i = 0; i < current_step.infective_population; i++)
		{	
			// std::cout << "recover() for()" << std::endl;
			// Random Recovery
			if (dis(gen) < mu)
			{
				// std::cerr << "New Recovery!" << std::endl;
				current_step.suceptible_population += 1;
				current_step.infective_population -= 1;
			}
		}
		current_step.population = current_step.suceptible_population + current_step.infective_population;
	}

	void update()
	{
		current_step.population += future_step.population;
		current_step.infective_population += future_step.infective_population;
		current_step.suceptible_population += future_step.suceptible_population;
		set_future_values(0,0,0);
	}
	void set_current_values(int pop , int ipop , int spop)
	{
		current_step.population = pop;
		current_step.infective_population = ipop;
		current_step.suceptible_population = spop;
	}
	void set_future_values(int pop , int ipop , int spop)
	{
		future_step.population = pop;
		future_step.infective_population = ipop;
		future_step.suceptible_population = spop;
	}
	void adjust_future_values(int pop = 0, int ipop = 0, int spop = 0) 
	{
		future_step.population += pop;
		future_step.infective_population += ipop;
		future_step.suceptible_population += spop;
	}

	friend std::ostream& operator<<(std::ostream& os, VertexProperties& vp) {
        return os << "{" << vp.current_step.population << "," << vp.current_step.infective_population << "," << vp.current_step.suceptible_population << "}";
    }
    friend std::istream& operator>>(std::istream& is, VertexProperties& vp) {
    	return is >> vp.id_ >> vp.name >> vp.current_step.population;
        // return is >> vp.current_step.population >> vp.current_step.infective_population >> vp.current_step.suceptible_population;
    }
    // Move future data, to current data and prepare to update
    void advance_timestep()
    {
    	set_current_values(future_step.population, future_step.infective_population, future_step.suceptible_population);
    }
	// Serialization support is required!
	template<typename Archiver>
	void serialize(Archiver& ar, const unsigned int /*version*/) {
		ar & id_ & name;
	}

	struct step_data
	{
		int population = 0;
		int infective_population = 0;
		int suceptible_population = 0;
	};

	step_data current_step;
	step_data future_step;

	int id_;
	std::string name;

    static constexpr float p = 0.5; // Infection Probability
	static constexpr float lamb = 0.10; // Movement Probability
	static constexpr float mu = 0.05; // Recovery Probability
};
struct EdgeProperties
{
	uint weight = 1;
	// Serialization support is required!
	template<typename Archiver>
	void serialize(Archiver& ar, const unsigned int /*version*/) {
	ar & weight;
	}
};

typedef std::vector<std::pair <int,int>> edgeMatrix;
typedef Graph<VertexProperties, EdgeProperties> GraphType;
typedef GraphType::Vertex VertexID;

typedef typename graph_traits<GraphType::GraphContainer>::edge_descriptor Edge;
typedef typename graph_traits<GraphType::GraphContainer>::vertex_descriptor Vertex;

// Random Number Generator
static std::mt19937 gen { std::random_device{}() };
std::uniform_real_distribution<double> ud(0.0,1.0);

std::map<std::string, VertexID> name_to_Vertex_map;

edgeMatrix readEdgeList(std::string fileName);


// GraphType G;

class Visit_No_Work : public boost::default_dfs_visitor
{

public:
	void discover_vertex(Vertex v, const GraphType::GraphContainer& g) const
	{
		// typename property_map<GraphType::GraphContainer, vertex_properties_t>::type param = get(vertex_properties, G.graph);

        // auto vp  = get(param, v);

		// auto vp = G.properties(v);
		std::cerr << "VERTEX #: " << std::endl;
		// std::cerr << "Vertex Population:" << vp.current_step.population << std::endl;
		// std::cerr << "Vertex Infective Population:" << vp.current_step.infective_population << std::endl;
		// std::cerr << "Vertex Susceptible Population:" << vp.current_step.suceptible_population << std::endl;
		std::cerr << std::endl;
	}
};

namespace boost {
  namespace graph {
    // Use the cell id as a key for indexing vertices in the graph
    template<>
    struct internal_vertex_name<VertexProperties> {
      typedef multi_index::member<VertexProperties, int, &VertexProperties::id_> type;
    };

    // Allow the graph to build vertices only given cell id
    template<>
    struct internal_vertex_constructor<VertexProperties> {
      typedef vertex_from_name<VertexProperties> type;
    };
  } //end namespace graph
} // end namespace boost

using namespace boost;
using boost::graph::distributed::mpi_process_group;

int main(int argc, char * argv[])
{

    boost::mpi::environment env{argc, argv};
    boost::mpi::communicator comm;

    std::cout << comm.rank() << std::endl; 

	// GraphType::GraphContainer g(edgemat.begin(), edgemat.end(), 64);
	// mpi_process_group pg = g.process_group();

	std::vector<Vertex> descs; 

	std::string filename ="current_g";
	std::string filepath = "../graph_data/processed/" + filename;

	edgeMatrix edgemat;

	try{
		edgemat = readEdgeList(filepath);	
	}catch(...) {
		std::cerr << "File Read Failed" << std::endl;
	}

	GraphType::GraphContainer g(edgemat.begin(), edgemat.end(), 64);
	
	if (comm.rank() == 0) {
		std::cout << "Using Boost "     
          << BOOST_VERSION / 100000     << "."  // major version
          << BOOST_VERSION / 100 % 1000 << "."  // minor version
          << BOOST_VERSION % 100                // patch level
          << std::endl;

	// 	for (auto row = edgemat.begin(); row != edgemat.end(); ++row ) {

	// // 		// VertexProperties vp1 = VertexProperties( std::stoi((*row)[0]) );
	// // 		// VertexProperties vp2 = VertexProperties( std::stoi((*row)[1]) );

	// // 		// Vertex v1 = add_vertex(vertex(std::stoi((*row)[0]), g), vertex(target, g), G.graph);
	// // 		// Vertex v2 = add_vertex(vp2, G.graph);

	// // 		// EdgeProperties ep;
	// 		// std::cout << "( " << std::stoi((*row)[0]) << "," <<
	// 		// 	std::stoi((*row)[1])  << ")" << std::endl;
	// 		add_edge(vertex(std::stoi((*row)[0]), g), vertex(std::stoi((*row)[1]), g), g);
	// 	}

		std::cout << "ALIVE" << std::endl;
	}
	synchronize(g.process_group());

	BGL_FORALL_VERTICES(v, g, GraphType::GraphContainer)
     {
         std::cout << "V @ " << comm.rank() << " ";// << g[v] << std::endl;
     } 
	// g.redistribute()

	// Visit_No_Work vis;
	// depth_first_search(g, boost::visitor(vis));

    return 0;
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
			std::cerr << "Edgelist file opened.." << std::endl;
		}
		idx++;
		// std::cerr << line.length() << std::endl;
		// skip commented or empty lines
		if (line.length() == 0 || line[0] == ('#')) {
			skippedLines += 1;
			continue;
		} else {
			std::pair <int,int> row;
			std::vector<int> line_data;

			char *cstr = new char[line.length() + 1];
			std::strcpy(cstr, line.c_str());
			auto pch = std::strtok(cstr, "\t");
			bool is_null = true;
			while (pch != NULL) {
				is_null = false;
				line_data.push_back(std::stoi(pch));
				pch = std::strtok(NULL, "\t");
			}
			
			if(!is_null) {
				row = make_pair(line_data[0], line_data[1]);
		    	edgemat.push_back(row);
		    }
		}
	}
	inFile.close();

	std::cerr << "File read complete\n Lines skipped: " << skippedLines << std::endl;
	std::cerr << "The number of edges read: " << edgemat.size() << std::endl;

	return edgemat;
}