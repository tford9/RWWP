/*
 * array_printer.cpp
 *
 *  Created on: Jun 17, 2016
 *      Author: micronx
 */
#include <cstdlib>
#include <iostream>
#include <string>
#include <array>

void print_array(std::array<std::string ,7> arr, int size, std::string debug = ""){
	if (size > 0){
		std::cout <<"[";
		for (int I = 0; I < size-1; I++){
			std::cout << arr[I] << ", ";
		};
		std::cout << arr[size-1] << "] " << debug << std::endl;
	}
}



