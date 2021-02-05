#include "Table.hpp"

Table::Table()
	:f_last_file_pos(0), f_name(""), f_schema(Schema()){}

Table::Table(std::string _name, Schema _schema)
	: f_last_file_pos(0), f_name(_name), f_schema(_schema) {}

Table::Table(std::string _name, table_row names, table_row types)
	: f_last_file_pos(0), f_name(_name), f_schema(Schema(names, types)) {}

void Table::reset()
{
	f_last_file_pos = 0;
}

void Table::clear()
{
	f_table.clear();
}

const Schema& Table::schema() const
{
	return f_schema;
}

void Table::info(const uint64_t size) const
{
	std::cout << "Table " << f_name << " : ";

	std::cout << "(";
	for (int i = 0; i < f_schema.size(); ++i) {
		std::cout << f_schema[i].name() << ":" << f_schema[i].type();
		std::cout << (i != f_schema.size() - 1 ? ", " : "");
	}
	std::cout << ")" << std::endl;

	std::cout << "Total " << f_table.size() << " rows (";
	std::cout << std::fixed << std::setprecision(1) << size / 1000.0f
		<< " KB data) in the table" << std::endl;
}

void Table::insert(const table_row& row)
{
	Record rec = Record(row);
	f_table.push_back(rec);
}

const std::list<Record>& Table::table() const
{
	return f_table;
}

std::list<Record> Table::search(const Statement& st) const
{
	std::list<Record> rtrn;

	std::string type = f_schema.type(st.lhs);
	if (type == "int")
		rtrn = searchTableInt(st);

	else if (type == "string")
		rtrn = searchTableStr(st);

	else if (type == "date")
		rtrn = searchTableDate(st);

	else
		throw std::exception("No such column!");

	return rtrn;
}

const std::string& Table::name() const
{
	return f_name;
}

bool Table::read_chunk(std::ifstream& in, const int maxReturn)
{
	Record cur;

	in.seekg(f_last_file_pos);

	int cnt = 0;
	while (cnt < maxReturn) {
		in >> cur;
		if (!in.good()) break;
		f_table.push_back(cur);
		cnt++;
	}
	if (cnt == 0) return false;
	f_last_file_pos = in.tellg();

	return true;
}

void Table::print_schema() const
{
	std::cout << "|";
	for (int i = 0; i < f_schema.size(); ++i)
		std::cout << f_schema[i].name() << '\t' << '|';
	std::cout << std::endl;

	std::cout << "-----------------------------" << std::endl;
}

void Table::print_schema(const std::vector<bool>& cols) const
{
	std::cout << "|";
	for (int i = 0; i < f_schema.size(); ++i)
		if(cols[i]) std::cout << f_schema[i].name() << '\t' << '|';
	std::cout << std::endl;

	std::cout << "-----------------------------" << std::endl;
}

std::ifstream& operator>>(std::ifstream& in, Table*& table)
{
	read_list(in, table->f_table);

	return in;
}

std::ofstream& operator<<(std::ofstream& out, const Table*& table)
{
	write_list(out, table->f_table);

	return out;
}

std::list<Record> Table::searchTableInt(const Statement& st) const
{
	std::list<Record> rtrn;
	int pos = f_schema.pos(st.lhs);

	int lhs;
	int rhs = std::stoi(st.rhs);
	for (const Record& rec : f_table) {
		if (pos != -1) lhs = std::stoi(rec[pos]);	
		
		if		((st.op == "==")	&& lhs == rhs) rtrn.push_back(rec);
		else if ((st.op == "!=")	&& lhs != rhs) rtrn.push_back(rec);
		else if ((st.op == "<")		&& lhs <  rhs) rtrn.push_back(rec);
		else if ((st.op == ">")		&& lhs >  rhs) rtrn.push_back(rec);
		else if ((st.op == "<=")	&& lhs <= rhs) rtrn.push_back(rec);
		else if ((st.op == ">=")	&& lhs >= rhs) rtrn.push_back(rec);
	}
	
	return rtrn;
}

std::list<Record> Table::searchTableStr(const Statement& st) const
{
	std::list<Record> rtrn;
	std::string lhs;

	int pos = f_schema.pos(st.lhs);
	for (const Record& rec : f_table) {
		if (pos != -1) lhs = rec[pos];

		if		((st.op == "==")	&& lhs == st.rhs) rtrn.push_back(rec);
		else if ((st.op == "!=")	&& lhs != st.rhs) rtrn.push_back(rec);
		else if ((st.op == "<")		&& lhs <  st.rhs) rtrn.push_back(rec);
		else if ((st.op == ">")		&& lhs >  st.rhs) rtrn.push_back(rec);
		else if ((st.op == "<=")	&& lhs <= st.rhs) rtrn.push_back(rec);
		else if ((st.op == ">=")	&& lhs >= st.rhs) rtrn.push_back(rec);
	}

	return rtrn;
}

std::list<Record> Table::searchTableDate(const Statement& st) const
{
	std::list<Record> rtrn;

	Date lhs;
	Date rhs = Date(st.rhs);
	int pos = f_schema.pos(st.lhs);
	for (const Record& rec : f_table) {
		if (pos != -1) lhs = Date(rec[pos]);

		if		((st.op == "==")	&& lhs == rhs) rtrn.push_back(rec);
		else if ((st.op == "!=")	&& lhs != rhs) rtrn.push_back(rec);
		else if ((st.op == "<")		&& lhs <  rhs) rtrn.push_back(rec);
		else if ((st.op == ">")		&& lhs >  rhs) rtrn.push_back(rec);
		else if ((st.op == "<=")	&& lhs <= rhs) rtrn.push_back(rec);
		else if ((st.op == ">=")	&& lhs >= rhs) rtrn.push_back(rec);
	}

	return rtrn;
}

