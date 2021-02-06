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

	return "unknown";
}

const std::vector<bool> Schema::columns(const table_row& names) const
{
	std::vector<bool> rtrn;
	
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

//const type_name_pair& type_name_pair::operator=(const type_name_pair& other)
//{
//	if (this != &other) {
//		this->f_type = other.f_type;
//		this->f_name = other.f_name;
//	}
//
//	return *this;
//}

