#include <iostream>
#include <fstream>
#include <random>
#include "s_types.h"

#include <boost/array.hpp>
#include <boost/random.hpp>
#include <boost/graph/random.hpp> 
#include <boost/random/uniform_real_distribution.hpp>
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
		int population = 0;
		int infective_population = 0;
		int suceptible_population = 0;
	};

	step_data current_step;
	step_data future_step;

	int ID;
	std::string name;

	uint row = 0;
	uint col = 0;

    static constexpr float p = 0.5; // Infection Probability
	static constexpr float lamb = 0.10; // Movement Probability
	static constexpr float mu = 0.05; // Recovery Probability

	VertexProperties() 
	{ 

	}

	VertexProperties(uint r, uint c, uint pop) 
	{ 
		row = r;
		col = c;
		current_step.population = pop;
		VertexProperties();
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
    	return is >> vp.row >> vp.col >> vp.current_step.population;
        // return is >> vp.current_step.population >> vp.current_step.infective_population >> vp.current_step.suceptible_population;
    }
    // Move future data, to current data and prepare to update
    void advance_timestep()
    {
    	set_current_values(future_step.population, future_step.infective_population, future_step.suceptible_population);
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

// Random Number Generator
static std::mt19937 gen { std::random_device{}() };
std::uniform_real_distribution<double> ud(0.0,1.0);

std::map<std::string, VertexID> name_to_Vertex_map;
// Global graph object
GraphType G;

class init_visit : public boost::default_dfs_visitor
{

public:
	void discover_vertex(Vertex v, const GraphType::GraphContainer& g) const
	{

		auto vp = G.properties(v);
		vp.set_current_values(1,0,1);
		G.properties(v) = vp;
	}
};

class Visit_No_Work : public boost::default_dfs_visitor
{

public:
	void discover_vertex(Vertex v, const GraphType::GraphContainer& g) const
	{

		auto vp = G.properties(v);
		std::cerr << "VERTEX #: " << vp.ID << std::endl;
		std::cerr << "Vertex Population:" << vp.current_step.population << std::endl;
		std::cerr << "Vertex Infective Population:" << vp.current_step.infective_population << std::endl;
		std::cerr << "Vertex Susceptible Population:" << vp.current_step.suceptible_population << std::endl;
		std::cerr << std::endl;
	}
};

class Travel : public boost::default_dfs_visitor
{

public:
	void discover_vertex(Vertex v, const GraphType::GraphContainer& g) const
	{

		auto vp = G.properties(v);

		// std::cerr << "VERTEX #: " << vp.ID << std::endl;
		vp.recover();
		vp.infect();
		move(v, g, vp);
		G.properties(v) = vp;
	}
private:
	void move(Vertex v, const GraphType::GraphContainer& g, VertexProperties& source_vp) const
	{
		// std::cout << "move()" << std::endl;	
		// auto source_vp = G.properties(v);

		// infective_population movement
		for(int i1 = 0; i1 < source_vp.current_step.infective_population; i1++)
		{
			// std::cout << "move() for(infective_population movement)" << std::endl;
			double random_number_1 = ud(gen);
			// std::cout << "RNG1: " << random_number_1 << std::endl;
			if(source_vp.lamb > random_number_1)
			{
				GraphType::out_edge_iter eit, eend;
				std::tie(eit, eend) = boost::out_edges(v, g);

				int out_deg = G.getVertexDegree(v);
				// If no adjacent vertices, then no move possible
				if(!out_deg) return;

				double uniform_prob = 1.0/out_deg;
				double stack[out_deg];

				stack[0] = 0.0;

				for( int i = 1; i < out_deg; i++)
				{
					stack[i] = stack[i-1] + uniform_prob;
				}

				// for( int i = 0; i < out_deg; i++){
				// 	std::cout << stack[i] << ",";
				// }
				int i = 0;

				double random_number_2 = ud(gen);
				// std::cout << "RNG2: " << random_number_2 << std::endl;
			  	for( auto it = eit; it != eend; ++it) 
			  	{
			  		// std::cout << "move() for(1) for(2)" << std::endl;
			  		if(random_number_2 > stack[i] && random_number_2 < stack[i+1])
			  		{
			  			auto targ =  boost::target(*it, G.graph);
						auto targ_vp = G.properties(targ);

						source_vp.adjust_future_values(-1,-1,0);

						targ_vp.adjust_future_values(1,1,0);

						G.properties(targ) = targ_vp;

						// std::cerr << "Infected Movement Happened!" << std::endl;
						
			  		}
		  			i++;
			  	}
			}
		}
		// susceptible population movement
		for(int i1 = 0; i1 < source_vp.current_step.suceptible_population; i1++)
		{
			// std::cout << "move() for(susceptible population movement)" << std::endl;

			double random_number_1 = ud(gen);
			// std::cout << "RNG1: " << random_number_1 << std::endl;
			if(source_vp.lamb > random_number_1)
			{

				GraphType::out_edge_iter eit, eend;
				std::tie(eit, eend) = boost::out_edges(v, g);

				int out_deg = G.getVertexDegree(v);

				// If no adjacent vertices, then no move possible
				if(!out_deg) return;
				// std::cout << "Movement Options: " << out_deg << std::endl;
				double uniform_prob = 1.0/out_deg;
				double stack[out_deg];

				stack[0] = 0.0;

				for( int i = 1; i < out_deg; i++)
				{
					stack[i] = stack[i-1] + uniform_prob;
				}

				// for( int i = 0; i < out_deg; i++){
				// 	std::cout << stack[i] << ",";
				// }
				int i = 0;

				double random_number_2 = ud(gen);
				// std::cout << "RNG2: " << random_number_2 << std::endl;

			  	for( auto it = eit; it != eend; ++it) 
			  	{
			  		// std::cout << random_number_2 << std::endl;

			  		// std::cout << "move() for(2) for(2)" << std::endl;
			  		if(random_number_2 > stack[i] && random_number_2 < stack[i+1])
			  		{
			  			auto targ =  boost::target(*it, G.graph);
						auto targ_vp = G.properties(targ);

						source_vp.adjust_future_values(-1,0,-1);
						
						targ_vp.adjust_future_values(1,0,1);

						G.properties(targ) = targ_vp;

						// std::cerr << "Susceptible Movement Happened!" << std::endl;
						
			  		}
		  			i++;
			  	}
			}
		}
	}
};

class Update : public boost::default_dfs_visitor
{

public:
	void discover_vertex(Vertex v, const GraphType::GraphContainer& g) const
	{
		auto vp = G.properties(v);
		vp.update();
		G.properties(v) = vp;
	}
};

int total_population = 0;
int total_susceptible = 0;
int total_infective = 0;

class Consistancy_Check : public boost::default_dfs_visitor
{
public:
	void discover_vertex(Vertex v, const GraphType::GraphContainer& g) const
	{
		auto vp = G.properties(v);
		// If there is nowhere for individuals to move, we can't let them determine if
		// there is consistancy in infection
		if(G.getVertexDegree(v) == 0) return;
		total_population += vp.current_step.population;
		total_susceptible += vp.current_step.suceptible_population;
		total_infective += vp.current_step.infective_population;
	}
};

// void loadGraphML(std::string filename, GraphType::GraphContainer g){

//     std::ifstream inFile;
//     inFile.open(filename, std::ifstream::in);

//     boost::dynamic_properties dp;
// 	boost::read_graphml(inFile, g);
// }

edgeMatrix readEdgeList(std::string fileName);

int main() {
	static boost::random::mt19937 rng;

	std::cerr << "Using Boost "     
          << BOOST_VERSION / 100000     << "."  // major version
          << BOOST_VERSION / 100 % 1000 << "."  // minor version
          << BOOST_VERSION % 100                // patch level
          << std::endl;
    std::string filename ="current_g";
    // std::cout << "Enter Filename" << std::endl;
    // std::cin >> filename;
	std::string filepath = "../graph_data/processed/" + filename;

	edgeMatrix edgemat;
	try{
		edgemat = readEdgeList(filepath);	
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
				v01 = G.AddVertex(vp01);
				name_to_Vertex_map.insert(std::pair<std::string, VertexID>(vp01.name, v01));
			}
			else
			{
				v01 = (name_to_Vertex_map.find(vp01.name))->second;
			}

			// Check if target vertex is already in the graph
			if (name_to_Vertex_map.find(vp02.name) == name_to_Vertex_map.end())
			{
				v02 = G.AddVertex(vp02);
				name_to_Vertex_map.insert(std::pair<std::string, VertexID>(vp02.name, v02));
			}
			else
			{
				v02 = (name_to_Vertex_map.find(vp02.name))->second;
			}

			G.AddDirectedEdge(v01, v02, ep01);
		}
	}
	// Set all vertex properties
	init_visit iVis;
	boost::depth_first_search(G.graph, boost::visitor(iVis));

	// Randomly select a vertex to be infected
    Vertex init_infected = random_vertex(G.graph, rng);
    auto init_infected_vp = G.properties(init_infected);
    init_infected_vp.set_current_values(2,1,1);
    G.properties(init_infected) = init_infected_vp;

	Travel tViz;
	Update uVis;
	Consistancy_Check ccVis;

	while(true)
	{
		boost::depth_first_search(G.graph, boost::visitor(uVis));
		boost::depth_first_search(G.graph, boost::visitor(tViz));
		boost::depth_first_search(G.graph, boost::visitor(uVis));
		boost::depth_first_search(G.graph, boost::visitor(ccVis));

		if (total_population == total_susceptible || total_population == total_infective)
		{
			std::string message;
			if (total_susceptible < total_infective)
			{
				message = "All Infected!";
			} else 
			{
				message = "All Uninfected!";
			}
			std::cout << "Consistancy_Check Success! " << message << std::endl;
			break;
		}

		total_population = total_susceptible = total_infective = 0;

	}

	Visit_No_Work vis;
	// boost::depth_first_search(G.graph, boost::visitor(vis));

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

	std::cerr << "File read complete\n Lines skipped: " << skippedLines << std::endl;
	std::cerr << "The number of edges read: " << edgemat.size() << std::endl;

	return edgemat;
}