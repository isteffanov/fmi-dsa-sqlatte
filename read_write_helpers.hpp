#pragma once 
#include <list>
#include <vector>
#include <fstream>

//write string to file
void write_string(std::fstream& out, const std::string& str);

//read string from file
void read_string(std::fstream& in, std::string& str);

//write vector of strings to file
void write_vector(std::fstream& out, const std::vector<std::string>& vec);

//read vector of strings from file
void read_vector(std::fstream& in, std::vector<std::string>& vec);

template<class T>
void write_list(std::fstream& out, const std::list<T>& list) {
	size_t size = list.size();
	out.write((const char*)&size, sizeof(size_t));

	for (const T& el : list)
		out << el;
}

template<class T>
void read_list(std::fstream& in, std::list<T>& list) {

	if (in.good()) {
		size_t size;
		in.read((char*)&size, sizeof(size_t));

		T cur;
		for (int i = 0; i < size; ++i) {
			in >> cur;
			list.push_back(cur);
		}
	}
}
