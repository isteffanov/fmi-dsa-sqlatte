#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <fstream>

class Record
{
	int64_t id;
	std::vector<std::string> row;

public:
	Record(int64_t _id, std::vector<std::string> _row);

	std::string operator[](size_t index) const;

	bool operator==(const Record& other) const;
	bool operator!=(const Record& other) const;

	friend std::ostream& operator<<(std::ostream& os, const	Record& record);
//	friend std::istream& operator>>(std::istream& is,		Record& record);

	size_t	data()	const;
	void	print() const;
};

