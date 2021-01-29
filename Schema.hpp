#pragma once
#include <string>
#include <vector>


struct type_name_pair {
	std::string type;
	std::string name;

	type_name_pair(std::string _name, std::string _type)
		:name(_name), type(_type) {}
};

struct Schema {
	std::vector<type_name_pair> schema;

	Schema(std::vector<type_name_pair> _schema)
		:schema(_schema) {}

	Schema(std::vector<std::string> names, std::vector<std::string> types) {
		if (types.size() != names.size()) return;

		for (int i = 0; i < types.size(); ++i)
			schema.push_back(type_name_pair(names[i], types[i]));
	}

	const type_name_pair& operator[](size_t pos) const { return schema[pos]; }

	size_t size() const { return schema.size(); }

	int posName(std::string name) const {
		for (int i = 0; i < schema.size(); ++i)
			if (schema[i].name == name) return i;

		return -1;
	}

	std::string find_type(std::string col) const
	{
		for (int i = 0; i < size(); ++i)
			if (schema[i].name == col) return schema[i].type;

		return "";
	}


};