#include "Schema.hpp"

const std::string type_name_pair::INT = "int";
const std::string type_name_pair::DATE = "date";
const std::string type_name_pair::STRING = "string";
const std::string type_name_pair::UNKNOWN = "unknown";

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

Schema::Schema(const Schema& other)
{
	if (this != &other) {
		this->f_schema.clear();
		for (const type_name_pair& pair : other.f_schema)
			this->f_schema.push_back(pair);
	}
}

const Schema& Schema::operator=(const Schema& other)
{
	if (this != &other) {
		//this->f_schema.clear();
		f_schema.resize(other.size());
		for (int i = 0; i < other.size(); ++i)
			this->f_schema[i] = other.f_schema[i];

	}

	return *this;
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

const type_name_pair& type_name_pair::operator=(const type_name_pair& other)
{
	if (this != &other) {
		this->f_type = other.f_type;
		this->f_name = other.f_name;
	}

	return *this;
}
