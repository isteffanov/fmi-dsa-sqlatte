#include "Record.hpp"

Record::Record(int64_t _id, std::vector<std::string> _row)
	:id(_id), row(_row) {}

std::string Record::operator[](size_t index) const
{
	//if ( !(index >= 0 && index < row.size()) ) throw std::exception("Record: Invalid index");

	return row[index];
}

bool Record::operator==(const Record& other) const
{
	return id == other.id;
}

bool Record::operator!=(const Record& other) const
{
	return !(*this == other);
}

size_t Record::data() const
{
	size_t total = 0;
	for (const std::string& elem : row)
		total += elem.size();

	return total;
}

void Record::print(const std::vector<size_t>& cols) const
{
	std::cout << "|" << id << "|";
	for (size_t i : cols)
		std::cout << row[i] << "|";
	std::cout << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Record& record)
{
	os << '|';
	for (const std::string& entry : record.row) 
		os << '\t' << entry << '|';
	os << '\n';

	return os;
}
