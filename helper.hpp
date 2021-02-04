#pragma once
#include <list>
#include <iterator>
#include <vector>
#include <fstream>
#include <regex>

template<class T>
bool isPresent(const std::list<T>& list, const T& cmp)
{
	for (const T& rec : list)
		if (rec == cmp) return true;

	return false;
}

template<class T>
void intersect(std::list<T>& lhs, std::list<T>& rhs)
{
	for (typename std::list<T>::iterator it = lhs.begin(); it != lhs.end(); ++it)
		if (!isPresent(rhs, *it)) lhs.erase(it);
}

template<class T>
void unify(std::list<T>& lhs, std::list<T>& rhs)
{
	for (typename std::list<T>::iterator it = rhs.begin(); it != rhs.end(); ++it)
		if (!isPresent(lhs, *it)) lhs.push_back(*it);
}

std::string findMatch(const std::string& str, const std::string& expr);

std::vector<std::string> findMatches(const std::string& str, const std::string& expr);

//write string to file
std::ofstream& write_string(std::ofstream& out, const std::string& str);
std::fstream& write_string(std::fstream& out, const std::string& str);

//read string from file
std::ifstream& read_string(std::ifstream& in, std::string& str);
std::fstream& read_string(std::fstream& in, std::string& str);

//write vector of strings to file
void write_vector(std::ofstream& out, const std::vector<std::string>& vec);
void write_vector(std::fstream& out, const std::vector<std::string>& vec);

//read vector of strings from file
void read_vector(std::ifstream& in, std::vector<std::string>& vec);
void read_vector(std::fstream& in, std::vector<std::string>& vec);

template<class T>
void write_list(std::fstream& out, const std::list<T>& list) {
	for (const T& el : list)
		out << el;
}

template<class T>
void write_list(std::ofstream& out, const std::list<T>& list) {
	for (const T& el : list)
		out << el;
}

template<class T>
void read_list(std::fstream& in, std::list<T>& list) {

	if (in.good()) {
		T cur;
		while (in >> cur) {
			list.push_back(cur);
		}
	}
}

template<class T>
void read_list(std::ifstream& in, std::list<T>& list) {

	if (in.good()) {
		T cur;
		while (in >> cur) {
			list.push_back(cur);
		}
	}
}