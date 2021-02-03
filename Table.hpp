#pragma once
#include <iostream>
#include <iomanip>
#include <list>
#include <string>

#include "read_write_helpers.hpp"
#include "Record.hpp"
#include "Schema.hpp"

class Table
{
#include "typedefs.ipp"

	int f_last_file_pos;
	std::string f_name;
	std::list<Record> f_table;
	Schema f_schema;

public:
	Table();
	Table(std::string _table_name, Schema _schema = Schema());
	Table(std::string _table_name , table_row types, table_row names);

	void				reset();
	void				clear();

	size_t				size() const;
	void				info(const uint64_t size) const;
	void				insert(const table_row& record);

	std::list<Record>	search(const std::string& lhs, const std::string& operation, const std::string& rhs) const;

	const std::string&	name() const;
	const Schema&		schema() const;

	bool				read_chunk(std::ifstream& in, const int maxReturn);

	friend std::ifstream& operator>>(std::ifstream& in, Table*& table);
	friend std::ofstream& operator<<(std::ofstream& out, const Table*& table);

private:
	const std::string&	type(const std::string& col) const;
	std::list<Record>	searchTableInt(const std::string& lhs, const std::string& operation, int value) const;
	std::list<Record>	searchTableStr(const std::string& lhs, const std::string& operation, const std::string& value) const;
};

