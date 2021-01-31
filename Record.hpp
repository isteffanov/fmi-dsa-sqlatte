#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <fstream>

#include "read_write_helpers.hpp"

class Record
{
	int64_t id;
	std::vector<std::string> row;

public:
	Record();
	Record(int64_t _id, std::vector<std::string> _row);

	size_t	data()	const;
	void	print(const std::vector<size_t>& cols) const;
	
	const std::string& operator[](size_t index) const;

	bool operator==(const Record& other) const;
	bool operator!=(const Record& other) const;

	friend std::ostream& operator<<(std::ostream& os, const	Record& record);
//	friend std::istream& operator>>(std::istream& is, Record& record);

	friend std::fstream& operator<<(std::fstream& out, const Record& record);
	friend std::fstream& operator>>(std::fstream& in, Record& record);
};

