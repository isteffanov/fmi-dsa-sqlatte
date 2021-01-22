#pragma once
#include <iostream>
#include <iomanip>
#include <list>
#include <vector>
#include <string>

#include "Record.hpp"

class Table
{
	struct Schema {
		std::vector<std::string> names;
		std::vector<std::string> types;

		Schema(std::vector<std::string> _names, std::vector<std::string> _types)
			:names(_names), types(_types) {}

		size_t size() const { return names.size(); }
	};

	std::string table_name;
	std::list<Record> table;
	Schema schema;

public:
	Table(std::string _table_name, std::vector<std::string> _names, std::vector<std::string> _types);

	void		insert(const std::vector<std::string>& record);
	void		remove(const Record& record);

	void		info() const;
	std::string name() const;

private:
	uint64_t	data() const;
};

