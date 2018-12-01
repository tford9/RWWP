// read_graph_from_dot.h
#ifndef UTILITIES
#define UTILITIES

/* Begin Include Statements */
#include <string>
#include <vector>
#include "s_types.h"
/* End Include Statements */


namespace utilities
{
	/* Begin Typedefs */  
	typedef std::vector< std::vector<std::string> > edgeMatrix;
	/* End Typedefs */

	// Function Prototypes
	void readEdgeList(std::string , edgeMatrix *);
	void read_graph_from_dot(edgeMatrix *);
}
#endif /* UTILITIES */ 