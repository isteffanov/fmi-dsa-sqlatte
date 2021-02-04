#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <fstream>

#include "read_write_helpers.hpp"

class Record
{
	std::vector<std::string> row;

public:
	class Compare {
		size_t pos;
		bool asc;
	public:
		Compare(size_t _pos, bool _asc = true)
			:pos(_pos), asc(_asc) {}

		bool operator()(const Record& lhs, const Record& rhs) {
			if (lhs.row.size() != rhs.row.size())
				throw std::exception("Cannot compare records of different size!");

			if (asc) return lhs.row[pos] < rhs.row[pos];
			else return lhs.row[pos] > rhs.row[pos];
		}
	};

public:
	Record();
	Record(std::vector<std::string> _row);
	const Record& operator=(const Record& other);

	const std::string& operator[](size_t index) const;

	bool operator==(const Record& other) const;
	bool operator!=(const Record& other) const;

	void	print() const;
	void	print(const std::vector<bool>& cols) const;

	friend std::ofstream& operator<<(std::ofstream& out, const Record& record);
	friend std::ifstream& operator>>(std::ifstream& in, Record& record);
};

