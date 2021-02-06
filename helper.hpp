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

std::string					findMatch(const std::string& str, const std::string& expr);
std::vector<std::string>	findMatches(const std::string& str, const std::string& expr);

//write string to file
std::ofstream&				write_string(std::ofstream& out, const std::string& str);
std::fstream&				write_string(std::fstream& out, const std::string& str);

//read string from file
std::ifstream&				read_string(std::ifstream& in, std::string& str);
std::fstream&				read_string(std::fstream& in, std::string& str);

//write vector of strings to file
void						write_vector(std::ofstream& out, const std::vector<std::string>& vec);
void						write_vector(std::fstream& out, const std::vector<std::string>& vec);

//read vector of strings from file
void						read_vector(std::ifstream& in, std::vector<std::string>& vec);
void						read_vector(std::fstream& in, std::vector<std::string>& vec);

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

template<class T>
bool isPresent(const std::forward_list<T>& list, const T& cmp)
{
	for (const T& rec : list)
		if (rec == cmp) return true;

	return false;
}

template<class T>
std::forward_list<T> intersect(std::forward_list<T>& lhs, const std::forward_list<T>& rhs)
{
	std::forward_list<T> rtrn;
	//push the elements which are in both lists
	for (typename std::forward_list<T>::const_iterator it = lhs.cbegin(); it != lhs.cend(); ++it)
		if (isPresent(rhs, *it))
			rtrn.push_front(*it);

	return rtrn;
}

template<class T>
std::forward_list<T> unify(std::forward_list<T>& lhs, const std::forward_list<T>& rhs)
{
	std::forward_list<T> rtrn = lhs;
	//push the elements which are not in the list
	for (typename std::forward_list<T>::const_iterator it = rhs.cbegin(); it != rhs.cend(); ++it)
		if (!isPresent(lhs, *it)) rtrn.push_front(*it);

	return rtrn;
}

template<class T>
void append(std::forward_list<T>& to, std::forward_list<T>& what)
{
	//transfer all elements from the second list to the first list
	if (!what.empty()) to.splice_after(to.before_begin(), what);
}

template<class T>
size_t getSize(const std::forward_list<T>& list) {
	size_t size = 0;
	//forward_list does not have size() :((
	for (typename std::forward_list<T>::const_iterator it = list.cbegin(); it != list.cend(); ++it)
		size++;

	return size;
}