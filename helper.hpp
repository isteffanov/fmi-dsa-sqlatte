/**
 *	@file helper.hpp
 *	@brief Helper methods used across the project.
 */

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

/**
 * @brief Holds a statement, i.e a >= b
 * @details The structure is used in BinaryTreeNode mainly, whose nodes hold a pointer to it, which are only referenced
 *	when the node is a leaf. 
 */
struct Statement {
	std::string lhs;
	std::string rhs;

	std::string op;

	Statement(std::string _lhs = "", std::string _op = "", std::string _rhs = "")
		:lhs(_lhs), op(_op), rhs(_rhs) {}
};

/**
 * @ brief
 * @ param 	The function accepts two strings - a string to search from and a regex string.
 * @ return The first match from the regex_search
 */
std::string					findMatch(const std::string& str, const std::string& expr);
/**
 * @ brief
 * @ param 	The function accepts two strings - a string to search from and a regex string.
 * @ return A vector with all matches from the regex_search except the empty strings.
 */
std::vector<std::string>	findMatches(const std::string& str, const std::string& expr);

/**
 * @ details A string is writen to a file by first writing its length, and then the string
 *	itself.
 */
std::ofstream&				write_string(std::ofstream& out, const std::string& str);
std::fstream&				write_string(std::fstream& out, const std::string& str);

/**
 * @ details A string is read the same way it is being writen - first its length and then the 
 *	consecutive characters.
 */
std::ifstream&				read_string(std::ifstream& in, std::string& str);
std::fstream&				read_string(std::fstream& in, std::string& str);

/**
 * @details A vector of strings is writen to a file by first writing its length and then
 *	writing the strings in it.
 */
void						write_vector(std::ofstream& out, const std::vector<std::string>& vec);
void						write_vector(std::fstream& out, const std::vector<std::string>& vec);

/**
 * @details A vector of strings is read from a file the same way it is being written - first its
 * length and then the strings in it
 */
void						read_vector(std::ifstream& in, std::vector<std::string>& vec);
void						read_vector(std::fstream& in, std::vector<std::string>& vec);

template<class T>
bool isPresent(const std::forward_list<T>& list, const T& cmp)
{
	for (const T& rec : list)
		if (rec == cmp) return true;

	return false;
}

/**
 * @details Time complexity: O(N^2)
 * @return The crosssection of two forward lists. 
 */
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

/**
 * @details Time complexity: O(N^2)
 * @return The union of two forward lists.
 */
template<class T>
std::forward_list<T> unify(std::forward_list<T>& lhs, const std::forward_list<T>& rhs)
{
	std::forward_list<T> rtrn = lhs;
	//push the elements which are not in the list
	for (typename std::forward_list<T>::const_iterator it = rhs.cbegin(); it != rhs.cend(); ++it)
		if (!isPresent(lhs, *it)) rtrn.push_front(*it);

	return rtrn;
}

/**
 * @details Appends the elements from the second list to the first list, erasing them in the process.
 *	Time complexity: O(N) where N is the length of the second list. 
 */
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