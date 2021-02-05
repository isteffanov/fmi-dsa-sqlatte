#pragma once
#include <iostream>
#include <iomanip>
#include <list>
#include <string>

#include "helper.hpp"
#include "Record.hpp"
#include "Schema.hpp"
#include "Date.hpp"
#include "typedefs.hpp"

class Table
{
	int f_last_file_pos;
	std::string f_name;
	std::list<Record> f_table;
	Schema f_schema;

public:
	Table();
	Table(std::string _table_name, Schema _schema = Schema());
	Table(std::string _table_name , table_row types, table_row names);

	//getters
	const std::list<Record>&	table() const;
	const std::string&			name() const;
	const Schema&				schema() const;

	//setters
	void						reset();
	void						clear();

	void						info(const uint64_t size) const;
	void						insert(const table_row& record);

	std::list<Record>			search(const Statement& st) const;

	void						print_schema() const;
	void						print_schema(const std::vector<bool>& cols) const;

	bool						read_chunk(std::ifstream& in, const int maxReturn);

	friend std::ifstream& operator>>(std::ifstream& in, Table*& table);
	friend std::ofstream& operator<<(std::ofstream& out, const Table*& table);

private:
	std::list<Record>	searchTableInt(const Statement& st) const;
	std::list<Record>	searchTableStr(const Statement& st) const;
	std::list<Record>	searchTableDate(const Statement& st) const;
};

