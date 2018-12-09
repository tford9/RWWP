#include <boost/config.hpp>
#include <boost/version.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/static_assert.hpp>
#include "s_types.h"

using namespace boost;
using namespace std;

/* definition of basic boost::graph properties */
enum vertex_properties_t { vertex_properties };
enum edge_properties_t { edge_properties };

namespace boost {
    BOOST_INSTALL_PROPERTY(vertex, properties);
    BOOST_INSTALL_PROPERTY(edge, properties);
}

/* the graph base class template */
template < typename VERTEXPROPERTIES, typename EDGEPROPERTIES >
class Graph
{
public:
    /* an adjacency_list like we need it */
    typedef boost::adjacency_list<boost::vecS, 
            boost::distributedS<boost::graph::distributed::mpi_process_group, 
            boost::vecS>, 
            boost::undirectedS, 
            boost::property<vertex_properties_t, VERTEXPROPERTIES>
            ,boost::property<vertex_color_t, default_color_type>
            > GraphContainer;

    /* a bunch of graph-specific typedefs */
    typedef typename graph_traits<GraphContainer>::vertex_descriptor Vertex;
    typedef typename graph_traits<GraphContainer>::edge_descriptor Edge;
    typedef std::pair<Edge, Edge> EdgePair;

    typedef typename graph_traits<GraphContainer>::vertex_iterator vertex_iter;
    typedef typename graph_traits<GraphContainer>::edge_iterator edge_iter;
    typedef typename graph_traits<GraphContainer>::adjacency_iterator adjacency_iter;
    typedef typename graph_traits<GraphContainer>::out_edge_iterator out_edge_iter;

    typedef typename graph_traits<GraphContainer>::degree_size_type degree_t;

    typedef std::pair<adjacency_iter, adjacency_iter> adjacency_vertex_range_t;
    typedef std::pair<out_edge_iter, out_edge_iter> out_edge_range_t;
    typedef std::pair<vertex_iter, vertex_iter> vertex_range_t;
    typedef std::pair<edge_iter, edge_iter> edge_range_t;


    /* constructors etc. */
    Graph()
    {}

    Graph(const Graph& g) :
        graph(g.graph)
    {}

    virtual ~Graph()
    {}

    /* structure modification methods */
    void Clear();

    Vertex AddVertex(const VERTEXPROPERTIES& prop);

    void RemoveVertex(const Vertex& v);

    EdgePair AddEdge(const Vertex& v1, const Vertex& v2, const EDGEPROPERTIES& prop_12, const EDGEPROPERTIES& prop_21);

    void AddDirectedEdge(const Vertex& v1, const Vertex& v2, const EDGEPROPERTIES& prop);

    /* property access */
    VERTEXPROPERTIES& properties();

    VERTEXPROPERTIES& properties(const Vertex& v);

    const VERTEXPROPERTIES& properties(const Vertex& v) const;

    EDGEPROPERTIES& properties(const Edge& v);

    const EDGEPROPERTIES& properties(const Edge& v) const;


    /* selectors and properties */
    const GraphContainer& getGraph() const;

    vertex_range_t getVertices() const;

    adjacency_vertex_range_t getAdjacentVertices(const Vertex& v) const;

    int getVertexCount() const;

    int getVertexDegree(const Vertex& v) const;


    /* operators */
    Graph& operator=(const Graph &rhs);

    GraphContainer graph;
};