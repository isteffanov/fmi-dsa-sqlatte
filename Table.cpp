#include "Table.hpp"

Table::Table(std::string _name, Schema _schema)
	:table_name(_name), schema(_schema) {}

Table::Table(std::string _name, table_row types, table_row names)
	:table_name(_name), schema(Schema(types, names)) {}

void Table::insert(const table_row& row)
{
	uint64_t id = table.size();
	table.push_back(Record(id, row));
}

void Table::remove(const std::string& query)
{
	//BinaryQueryTree bq_tree = BinaryQueryTree(this);
	//bq_tree.init(query);

	//std::list<Record> found = bq_tree.search();
	//
	////TODO		
}

void Table::select(const std::string& query)
{
	BinaryQueryTree tree(this);
	tree.init(query);

	std::list<Record> found = tree.search();

	for (const Record& rec : found)
		rec.print();
	std::cout << std::endl;
}

std::list<Record> Table::search(const std::string& lhs, const std::string& operation, const std::string& rhs) const
{
	std::list<Record> rtrn;

	std::string type = this->type(lhs);
	if (type == "int")
		rtrn = searchTableInt(lhs, operation, std::stoi(rhs));

	else if (type == "string")
		rtrn = searchTableStr(lhs, operation, rhs);

	else
		std::cout << "search broke" << std::endl;

	return rtrn;
}

void Table::info() const
{
	std::cout << "Table " << table_name << " : ";
	
	std::cout << "(";
	for (int i = 0; i < schema.size(); ++i) {
		std::cout << schema[i].name << ":" << schema[i].type;
		std::cout << (i != schema.size() - 1 ? ", " : "");
	}
	std::cout << ")" << std::endl;

	std::cout << "Total " << table.size() << " rows (";
	std::cout << std::fixed << std::setprecision(1) << data()/1000.0f 
			  << " KB data) in the table" << std::endl;
}


size_t Table::size() const
{
	return schema.size();
}

const std::string& Table::name() const
{
	return table_name;
}

const std::string& Table::type(const std::string& col) const
{
	for (int i = 0; i < size(); ++i)
		if (schema[i].name == col) return schema[i].type;

	return "";
}

uint64_t Table::data() const
{
	uint64_t total = 0;
	for (const Record& record : table)
		total += record.data();

	return total;
}

std::list<Record> Table::searchTableInt(const std::string& lhs, const std::string& op, int value) const
{
	std::list<Record> rtrn;
	for (const Record& rec : table) {
		std::string entry;

		int pos = schema.posName(lhs);
		if (pos != -1) entry = rec[pos];
		
		if ((op == "==")		&& std::stoi(entry) == value) rtrn.push_back(rec);
		else if ((op == "!=")	&& std::stoi(entry) != value) rtrn.push_back(rec);
		else if ((op == "<")	&& std::stoi(entry) < value) rtrn.push_back(rec);
		else if ((op == ">")	&& std::stoi(entry) > value) rtrn.push_back(rec);
		else if ((op == "<=")	&& std::stoi(entry) <= value) rtrn.push_back(rec);
		else if ((op == ">=")	&& std::stoi(entry) >= value) rtrn.push_back(rec);
	}
	
	return rtrn;
}

std::list<Record> Table::searchTableStr(const std::string& lhs, const std::string& op, const std::string& value) const
{
	std::list<Record> rtrn;
	for (const Record& rec : table) {
		std::string entry;

		int pos = schema.posName(lhs);
		if (pos != -1) entry = rec[pos];

		if ((op == "==")		&& entry == value) rtrn.push_back(rec);
		else if ((op == "!=")	&& entry != value) rtrn.push_back(rec);
		else if ((op == "<")	&& entry < value) rtrn.push_back(rec);
		else if ((op == ">")	&& entry > value) rtrn.push_back(rec);
		else if ((op == "<=")	&& entry <= value) rtrn.push_back(rec);
		else if ((op == ">=")	&& entry >= value) rtrn.push_back(rec);
	}

	return rtrn;
}


