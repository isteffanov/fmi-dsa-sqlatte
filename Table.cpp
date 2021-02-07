#include "Table.hpp"

Table::Table()
	:f_last_file_pos(0), f_name(""), f_schema(Schema()){}

Table::Table(std::string _name, Schema _schema)
	: f_last_file_pos(0), f_name(_name), f_schema(_schema) {}

Table::Table(std::string _name, table_row names, table_row types)
	: f_last_file_pos(0), f_name(_name), f_schema(Schema(names, types)) {}

const list_record& Table::table() const
{
	return f_table;
}

const std::string& Table::name() const
{
	return f_name;
}

const Schema& Table::schema() const
{
	return f_schema;
}

/**
 * @brief Sets the index of the table's file back to 0.
 */
void Table::reset()
{
	f_last_file_pos = 0;
}

void Table::clear()
{
	f_table.clear();
}

void Table::info(const size_t count, const uint64_t size) const
{
	std::cout << "Table " << f_name << " : ";

	std::cout << "(";
	for (int i = 0; i < f_schema.size(); ++i) {
		std::cout << f_schema[i].name() << ":" << f_schema[i].type();
		std::cout << (i != f_schema.size() - 1 ? ", " : "");
	}
	std::cout << ")" << std::endl;

	std::cout << "Total " << count << " rows (";
	std::cout << std::fixed << std::setprecision(1) << size / 1000.0f
		<< " KB data) in the table" << std::endl;
}

void Table::insert(const table_row& row)
{
	Record rec = Record(row);
	f_table.push_front(rec);
}

/**
 * @return std::forward_list of records, 
 *	which satisfy the given statement
 */

list_record Table::search(const Statement& st) const
{
	list_record rtrn;

	std::string type = f_schema.type(st.lhs);
	//depending on the type we use different compatisons
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

/**
 * @return true if the record suits the table's schema
 */

bool Table::validate(const Record& rec)
{
	for (int i = 0; i < rec.size(); ++i) {
		try {
			//if there is an exception, do not allow further continuation
			if (f_schema[i].type() == type_name_pair::INT) std::stoi(rec[i]);
			else if (f_schema[i].type() == type_name_pair::DATE) Date d(rec[i]);
		}
		catch (...) {
			return false;
		}		
	}

	return true;
}

/**
 * @details As we cannot read every one of them
 * because the memory may deplete, we read atmost maxReturn
 * number of Records. The position in the file is saved, and
 * the next reading will begin from there. After successfuly 
 * reading the whole file .reset() should be used.
 * @param maxReturn is the maximum number of records that 
 * shall be returned.
 */
bool Table::readChunk(std::ifstream& in, const int maxReturn)
{
	Record cur;

	in.seekg(f_last_file_pos);

	int cnt = 0;
	while (cnt < maxReturn) {
		in >> cur;
		if (!in.good()) break;
		f_table.push_front(cur);
		cnt++;
	}
	if (cnt == 0) return false;
	f_last_file_pos = in.tellg();

	return true;
}

std::ofstream& operator<<(std::ofstream& out, const Table*& table)
{
	for (const Record& el : table->f_table)
		out << el;

	return out;
}

list_record Table::searchTableInt(const Statement& st) const
{
	list_record rtrn;
	int pos = f_schema.pos(st.lhs);

	int lhs;
	int rhs = std::stoi(st.rhs);
	for (const Record& rec : f_table) {
		if (pos != -1) lhs = std::stoi(rec[pos]);	
		
		if		((st.op == "==")	&& lhs == rhs) rtrn.push_front(rec);
		else if ((st.op == "!=")	&& lhs != rhs) rtrn.push_front(rec);
		else if ((st.op == "<")		&& lhs <  rhs) rtrn.push_front(rec);
		else if ((st.op == ">")		&& lhs >  rhs) rtrn.push_front(rec);
		else if ((st.op == "<=")	&& lhs <= rhs) rtrn.push_front(rec);
		else if ((st.op == ">=")	&& lhs >= rhs) rtrn.push_front(rec);
	}
	
	return rtrn;
}

list_record Table::searchTableStr(const Statement& st) const
{
	list_record rtrn;
	std::string lhs;

	int pos = f_schema.pos(st.lhs);
	for (const Record& rec : f_table) {
		if (pos != -1) lhs = rec[pos];

		if		((st.op == "==")	&& lhs == st.rhs) rtrn.push_front(rec);
		else if ((st.op == "!=")	&& lhs != st.rhs) rtrn.push_front(rec);
		else if ((st.op == "<")		&& lhs <  st.rhs) rtrn.push_front(rec);
		else if ((st.op == ">")		&& lhs >  st.rhs) rtrn.push_front(rec);
		else if ((st.op == "<=")	&& lhs <= st.rhs) rtrn.push_front(rec);
		else if ((st.op == ">=")	&& lhs >= st.rhs) rtrn.push_front(rec);
	}

	return rtrn;
}

list_record Table::searchTableDate(const Statement& st) const
{
	list_record rtrn;

	Date lhs;
	Date rhs = Date(st.rhs);
	int pos = f_schema.pos(st.lhs);
	for (const Record& rec : f_table) {
		if (pos != -1) lhs = Date(rec[pos]);

		if		((st.op == "==")	&& lhs == rhs) rtrn.push_front(rec);
		else if ((st.op == "!=")	&& lhs != rhs) rtrn.push_front(rec);
		else if ((st.op == "<")		&& lhs <  rhs) rtrn.push_front(rec);
		else if ((st.op == ">")		&& lhs >  rhs) rtrn.push_front(rec);
		else if ((st.op == "<=")	&& lhs <= rhs) rtrn.push_front(rec);
		else if ((st.op == ">=")	&& lhs >= rhs) rtrn.push_front(rec);
	}

	return rtrn;
}

