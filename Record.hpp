#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>

#include "Date.hpp"
#include "helper.hpp"


/**
 * @brief The class represents a single entry in the database.
 * @details That entry is represented as a vector of strings.
 */
class Record
{
	std::vector<std::string> row;

public:

	/**
	 * @details The class compares two Record objects based on order, specific element and
	 *	whether or not that element is a Date. It is used in the search queries when the user wants the
	 *	output to be sorted.
	 */
	class Compare {
		size_t pos;
		bool isDate;
		bool asc;

	public:
		Compare(size_t _pos, bool date, bool _asc = true)
			:pos(_pos), isDate(date), asc(_asc) {}

		bool operator()(const Record& lhs, const Record& rhs) {
			if (isDate) {
				Date left(lhs[pos]);
				Date right(rhs[pos]);

				if (asc) return left < right;
				else return left > right;
			}
			else {
				if (lhs.row.size() != rhs.row.size())
					throw std::exception("Cannot compare records of different size!");

				if (asc) return lhs.row[pos] < rhs.row[pos];
				else return lhs.row[pos] > rhs.row[pos];
			}
			
		}
	};

public:
	Record();
	Record(std::vector<std::string> _row);
	const Record& operator=(const Record& other);

	const std::string& operator[](size_t index) const;

	bool operator==(const Record& other) const;
	bool operator!=(const Record& other) const;

	size_t	size() const;
	void	print() const;
	void	print(const std::vector<bool>& cols) const;	//prints specific columns

	friend std::ofstream& operator<<(std::ofstream& out, const Record& record);
	friend std::ifstream& operator>>(std::ifstream& in, Record& record);
};

