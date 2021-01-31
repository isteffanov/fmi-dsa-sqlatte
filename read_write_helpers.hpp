#pragma once 
#include <list>
#include <vector>
#include <fstream>

//write string to file
std::ofstream& write_string(std::ofstream& out, const std::string& str);

//read string from file
std::ifstream& read_string(std::ifstream& in, std::string& str);

//write vector of strings to file
void write_vector(std::ofstream& out, const std::vector<std::string>& vec);

//read vector of strings from file
void read_vector(std::ifstream& in, std::vector<std::string>& vec);

template<class T>
void write_list(std::ofstream& out, const std::list<T>& list) {
	for (const T& el : list)
		out << el;
}

template<class T>
void read_list(std::ifstream& in, std::list<T>& list) {

	if (in.good()) {
		T cur;
		while(in >> cur) {			
			list.push_back(cur);
		}
	}
}
