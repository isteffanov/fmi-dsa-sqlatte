#include "Record.hpp"

Record::Record()
	:id(0), row(std::vector<std::string>()){}

Record::Record(int64_t _id, std::vector<std::string> _row)
	:id(_id), row(_row) {}

const std::string& Record::operator[](size_t index) const
{
	if ( !(index >= 0 && index < row.size()) ) 
		throw std::exception("Record: Invalid index");

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

std::ofstream& operator<<(std::ofstream& out, const Record& record)
{
	uint64_t id = record.id;

	out.write((const char*)&id, sizeof(uint64_t));
	write_vector(out, record.row);

	return out;
}

std::ifstream& operator>>(std::ifstream& in, Record& record)
{
	uint64_t id;
	std::vector<std::string> row;

	in.read((char*)&id, sizeof(uint64_t));
	read_vector(in, row);

	record = Record(id, row);
	return in;
}
