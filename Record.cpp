#include "Record.hpp"

Record::Record()
	:row(std::vector<std::string>()){}

Record::Record(std::vector<std::string> _row)
	:row(_row) {}

const Record& Record::operator=(const Record& other)
{
	if (this != &other) {
		this->row.clear();
		this->row = other.row;
	}

	return *this;
}

const std::string& Record::operator[](size_t index) const
{
	if (!(index >= 0 && index < row.size()))
		throw std::exception("Record: Invalid index");

	return row[index];
}

bool Record::operator==(const Record& other) const
{
	if (this->row.size() != other.row.size()) return false;
	for (int i = 0; i < row.size(); ++i)
		if (this->row[i] != other.row[i]) return false;

	return true;	
}

bool Record::operator!=(const Record& other) const
{
	return !(*this == other);
}

void Record::print() const
{
	std::cout << '|';
	for (const std::string& entry : row)
		std::cout << entry << '\t' << '|';
	std::cout << '\n';
}

void Record::print(const std::vector<bool>& cols) const
{
	std::cout << "|";
	for (int i = 0; i < row.size(); ++i)
		if(cols[i]) std::cout << row[i] << '\t' << "|";
	std::cout << std::endl;
}

std::ofstream& operator<<(std::ofstream& out, const Record& record)
{
	write_vector(out, record.row);

	return out;
}

std::ifstream& operator>>(std::ifstream& in, Record& record)
{
	std::vector<std::string> row;
	read_vector(in, row);

	record = Record(row);
	return in;
}
