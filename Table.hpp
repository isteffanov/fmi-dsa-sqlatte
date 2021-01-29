#pragma once
#include <iostream>
#include <iomanip>
#include <list>
#include <string>

#include "Schema.hpp"
#include "Record.hpp"
#include "BinaryQueryTree.hpp"

class Table
{
#include "typedefs.ipp"

	std::string table_name;
	std::list<Record> table;
	Schema schema;

public:

	Table(std::string _table_name, Schema _schema);
	Table(std::string _table_name, table_row types, table_row names);

	void				insert(const table_row& record);
	void				remove(const std::string& query);
	void				select(const std::string& query);

	std::list<Record>	search(const std::string& lhs, const std::string& operation, const std::string& rhs) const;

	void				info() const;
	size_t				size() const;
	const std::string&	name() const;
	
private:
	uint64_t			data() const;
	const std::string&	type(const std::string& col) const;
	std::list<Record>	searchTableInt(const std::string& lhs, const std::string& operation, int value) const;
	std::list<Record>	searchTableStr(const std::string& lhs, const std::string& operation, const std::string& value) const;
};

