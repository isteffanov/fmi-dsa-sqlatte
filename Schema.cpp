#include "Schema.hpp"

Schema::Schema()
	:f_schema(std::vector<type_name_pair>()){}

Schema::Schema(std::vector<type_name_pair> _schema)
	:f_schema(_schema) {}

Schema::Schema(std::vector<std::string> names, std::vector<std::string> types)
{
	if (types.size() != names.size()) return;

	for (int i = 0; i < types.size(); ++i)
		f_schema.push_back(type_name_pair(names[i], types[i]));
}

const size_t Schema::size() const
{
	return f_schema.size();
}

const size_t Schema::pos(std::string name) const
{
	for (int i = 0; i < f_schema.size(); ++i)
		if (f_schema[i].name() == name) return i;

	return std::string::npos;
}



const std::string& Schema::find_type(std::string col) const
{
	for (int i = 0; i < size(); ++i)
		if (f_schema[i].name() == col) return f_schema[i].type();

	return "";
}

const std::vector<type_name_pair>& Schema::schema() const
{
	return f_schema;
}

const type_name_pair& Schema::operator[](size_t index) const
{
	if ( !(index >= 0 && index < size()) ) 
		throw std::exception("Schema: Invalid index!");

	return f_schema[index];
}

std::fstream& operator>>(std::fstream& in, Schema& schema)
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

std::fstream& operator<<(std::fstream& out, const Schema& schema)
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
