#include "Table.hpp"

Table::Table(std::string _name, std::vector<std::string> _names, std::vector<std::string> _types)
	:table_name(_name), schema(Schema(_names, _types)) {}

void Table::insert(const std::vector<std::string>& row)
{
	uint64_t id = table.size();
	table.push_back(Record(id, row));
}

void Table::remove(const Record& toRemove)
{
	table.remove(toRemove);
}

void Table::info() const
{
	std::cout << "Table " << table_name << " : ";
	
	std::cout << "(";
	for (int i = 0; i < schema.size(); ++i) {
		std::cout << schema.names[i] << ":" << schema.types[i];
		std::cout << (i != schema.size() - 1 ? ", " : "");
	}
	std::cout << ")" << std::endl;

	std::cout << "Total " << table.size() << " rows (";
	std::cout << std::fixed << std::setprecision(1) << data()/1000.0f 
			  << " KB data) in the table" << std::endl;
}

std::string Table::name() const
{
	return table_name;
}

uint64_t Table::data() const
{
	uint64_t total = 0;
	for (const Record& record : table)
		total += record.data();

	return total;
}

