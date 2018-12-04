#include <boost/config.hpp>
#include <boost/version.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/static_assert.hpp>

using namespace boost;
using namespace std;

/* definition of basic boost::graph properties */
enum vertex_properties_t { vertex_properties };
enum edge_properties_t { edge_properties };

namespace boost {
    BOOST_INSTALL_PROPERTY(vertex, properties);
    BOOST_INSTALL_PROPERTY(edge, properties);
}



/* Same as above but doesn't require any copying. */
template < typename VERTEXPROPERTIES, typename EDGEPROPERTIES >
class GraphView
{
public:

    /* an adjacency_list like we need it */
    typedef adjacency_list<
        vecS, // disallow parallel edges
        vecS, // vertex container
        bidirectionalS, // directed graph
        property<vertex_properties_t, VERTEXPROPERTIES>,
        property<edge_properties_t, EDGEPROPERTIES>
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
    GraphView()
    {}

    GraphView(const GraphContainer * const g) :
        graph_ptr(g)
    {}

    virtual ~GraphView()
    {}

    /* property access */
    const VERTEXPROPERTIES& properties(const Vertex& v) const
    {
        typename property_map<GraphContainer, vertex_properties_t>::const_type param = get(vertex_properties, *graph_ptr);
        return param[v];
    }

    const EDGEPROPERTIES& properties(const Edge& v) const
    {
        typename property_map<GraphContainer, edge_properties_t>::const_type param = get(edge_properties, *graph_ptr);
        return param[v];
    }

    /* selectors and properties */
    const GraphContainer * const getGraphPtr() const
    {
        return graph_ptr;
    }

    vertex_range_t getVertices() const
    {
        return vertices(*graph_ptr);
    }

    adjacency_vertex_range_t getAdjacentVertices(const Vertex& v) const
    {
        return adjacent_vertices(v, *graph_ptr);
    }

    out_edge_range_t getAdjacentVertices(const Vertex& v) const
    {
        return adjacent_vertices(v, *graph_ptr);
    }

    int getVertexCount() const
    {
        return num_vertices(*graph_ptr);
    }

    int getVertexDegree(const Vertex& v) const
    {
        return out_degree(v, *graph_ptr);
    }

protected:
    const GraphContainer * const graph_ptr;  // Constant pointer to a constant graph.
};