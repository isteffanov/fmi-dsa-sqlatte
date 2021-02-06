#pragma once
#include <forward_list>
#include <iterator>
#include <vector>
#include <fstream>
#include <regex>

class Record;
class Table;

typedef	 std::forward_list<Table*> list_tableptr;
typedef	 std::forward_list<Record> list_record;
typedef  std::forward_list<Table*>::iterator list_tableptr_it;
typedef  std::forward_list<Record>::iterator list_record_it;
typedef std::vector<std::string> table_row;

struct Statement {
	std::string lhs;
	std::string rhs;

	std::string op;

	Statement(std::string _lhs = "", std::string _op = "", std::string _rhs = "")
		:lhs(_lhs), op(_op), rhs(_rhs) {}
};

template<class T>
bool isPresent(const std::forward_list<T>& list, const T& cmp)
{
	for (const T& rec : list)
		if (rec == cmp) return true;

	return false;
}

template<class T>
void intersect(std::forward_list<T>& lhs, std::forward_list<T>& rhs)
{
	for (typename std::forward_list<T>::iterator it = lhs.begin(); it != lhs.end();) {
		if (!isPresent(rhs, *it)) 
			it = lhs.erase_after(it);
		else
			++it;
		
	}
}

template<class T>
void unify(std::forward_list<T>& lhs, std::forward_list<T>& rhs)
{
	for (typename std::forward_list<T>::iterator it = rhs.begin(); it != rhs.end(); ++it)
		if (!isPresent(lhs, *it)) lhs.push_front(*it);
}

template<class T>
void append(std::forward_list<T>& to, std::forward_list<T>& what)
{
	if (!what.empty()) to.splice_after(to.before_begin(), what);
}

template<class T>
size_t getSize(std::forward_list<T>& list) {
	size_t size = 0;
	for (typename std::forward_list<T>::iterator it = list.begin(); it != list.end(); ++it)
		size++;

	return size;
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
void write_list(std::fstream& out, const std::forward_list<T>& list) {
	for (const T& el : list)
		out << el;
}

template<class T>
void write_list(std::ofstream& out, const std::forward_list<T>& list) {
	for (const T& el : list)
		out << el;
}

template<class T>
void read_list(std::fstream& in, std::forward_list<T>& list) {

	if (in.good()) {
		T cur;
		while (in >> cur) {
			list.psuh_front(cur);
		}
	}
}

template<class T>
void read_list(std::ifstream& in, std::forward_list<T>& list) {

	if (in.good()) {
		T cur;
		while (in >> cur) {
			list.push_front(cur);
		}
	}
}