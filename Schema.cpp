#include "Schema.hpp"

const std::string type_name_pair::INT = "int";
const std::string type_name_pair::DATE = "date";
const std::string type_name_pair::STRING = "string";
const std::string type_name_pair::UNKNOWN = "unknown";

Schema::Schema()
	:f_schema(std::vector<type_name_pair>()){}

Schema::Schema(std::vector<type_name_pair> _schema)
	:f_schema(_schema) {}

Schema::Schema(table_row names, table_row types)
{
	if (types.size() != names.size()) return;

	for (int i = 0; i < types.size(); ++i)
		f_schema.push_back(type_name_pair(names[i], types[i]));
}

const type_name_pair& Schema::operator[](size_t index) const
{
	if (!(index >= 0 && index < size()))
		throw std::exception("Schema: Invalid index!");

	return f_schema[index];
}

const size_t Schema::size() const
{
	return f_schema.size();
}

const size_t Schema::pos(const std::string& name) const
{
	for (int i = 0; i < f_schema.size(); ++i)
		if (f_schema[i].name() == name) return i;

	return std::string::npos;
}

const bool Schema::date(const std::string& name) const
{
	for (const type_name_pair& pair : f_schema)
		if (pair.name() == name) 
			return (pair.type() == "date");

	return false;
}

const std::vector<type_name_pair>& Schema::schema() const
{
	return f_schema;
}

const std::string& Schema::type(const std::string& name) const
{
	for (const type_name_pair& pair : f_schema)
		if (pair.name() == name) return pair.type();

	return type_name_pair::UNKNOWN;
}

const std::vector<bool> Schema::columns(const table_row& names) const
{
	std::vector<bool> rtrn;
	
	//return vector of same length as the schema with has
	//true where the name is contained in the passed vector
	bool found;
	for (const type_name_pair& pair : f_schema) {
		found = false;
		for (const std::string col : names) {
			if (pair.name() == col) {
				found = true;
				break;
			}
		}

		rtrn.push_back(found);
	}
		
	return rtrn;
}

void Schema::print() const
{
	std::cout << "|";
	for (int i = 0; i < f_schema.size(); ++i)
		std::cout << std::setw(6) << f_schema[i].name() << '|';
	std::cout << std::endl;

	std::cout << std::setw(22) << std::setfill('-') << '\n';
}

void Schema::print(const std::vector<bool>& cols) const
{
	std::cout << "|";
	for (int i = 0; i < f_schema.size(); ++i)
		if (cols[i]) std::cout << std::setw(6) << f_schema[i].name() << '|';
	std::cout << std::endl;

	std::cout << std::setw(22) << std::setfill('-') << '\n';
}

std::ifstream& operator>>(std::ifstream& in, Schema& schema)
{
	std::vector<type_name_pair> pairs;

	if (in.good()) {
		size_t size;
		std::string name, type;

		in.read((char*)&size, sizeof(size_t));
		
		for (int i = 0; i < size; ++i) {
			read_string(in, name);
			read_string(in, type);
			
			pairs.push_back({ name, type });
		}
	}

	schema = Schema(pairs);

	return in;
}

std::ofstream& operator<<(std::ofstream& out, const Schema& schema)
{
	//write the size of the schema, then the two strings in each pair
	if (out.good()) {
		size_t size = schema.size();
		out.write((const char*)&size, sizeof(size_t));

		for (int i = 0; i < schema.size(); ++i) {
			write_string(out, schema[i].name());
			write_string(out, schema[i].type());
		}
	}

	return out;
}


