#define _CRT_SECURE_NO_WARNINGS

/* Begin Include Statements */
#include "utilities.h"

#include <string>
#include <vector> 
#include <iostream>
#include <fstream>

/* End Include Statements */


/* Begin Typedefs */
#include "s_types.h"

/* End Typedefs */

using namespace std;
using namespace utilities;

void utilities::read_graph_from_dot(edgeMatrix *edgemat) {

	std::string filename;
	// Prompt for edgelist
	std::cout << "Enter Edgelist Location\n";
	// Store Results
	std::cin >> filename;
	std::cout << "The filename you chose is: " << filename << std::endl;
	try{
		readEdgeList(filename, edgemat);	
	}catch(...) {
		std::cerr << "File Read Failed" << std::endl;
	}
}

void utilities::readEdgeList(std::string fileName, edgeMatrix *edgemat) {

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
			
		    edgemat->push_back(row);
		}
	}
	inFile.close();

	std::cout << "File read complete\n Lines skipped: " << skippedLines << std::endl;
	std::cout << "The number of edges read: " << edgemat->size() << std::endl;
}
