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

	std::string table_name;
	std::list<Record> table;
	Schema schema;

public:
	Table();
	Table(std::string _table_name, Schema _schema = Schema());
	Table(std::string _table_name , table_row types, table_row names);

	const Record		insert(const table_row& record);
	std::list<Record>	remove(const std::string& query);
	void				selectAll(const std::string& query);
	void				selectSome(const std::string& query, const table_row& cols);

	std::list<Record>	search(const std::string& lhs, const std::string& operation, const std::string& rhs) const;

	void				info() const;
	size_t				size() const;
	const std::string&	name() const;
	
private:
	uint64_t			data() const;
	const std::string&	type(const std::string& col) const;
	std::list<Record>	searchTableInt(const std::string& lhs, const std::string& operation, int value) const;
	std::list<Record>	searchTableStr(const std::string& lhs, const std::string& operation, const std::string& value) const;

	std::vector<size_t> findColsToPrint(const table_row& cols) const;


	friend std::ifstream& operator>>(std::ifstream& in, Table*& table);
	friend std::ofstream& operator<<(std::ofstream& out, const Table*& table);
};

