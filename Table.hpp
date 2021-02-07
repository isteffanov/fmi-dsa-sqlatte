#pragma once
#include <iostream>
#include <iomanip>
#include <list>
#include <string>

#include "Record.hpp"
#include "Schema.hpp"
#include "Date.hpp"
#include "helper.hpp"

/**
 * @details A table is represented by its name, schema and
 * a certain amount of records it stores in a given moment.
 */
class Table
{
	int f_last_file_pos;
	std::string f_name;
	list_record f_table;
	Schema f_schema;

public:
	Table();
	Table(std::string _table_name, Schema _schema = Schema());
	Table(std::string _table_name , table_row types, table_row names);

	const list_record&	table() const;
	const std::string&	name() const;
	const Schema&		schema() const;

	void				reset();
	void				clear();

	void				info(const size_t count, const uint64_t size) const;
	void				insert(const table_row& record);

	//return the records, which satisfy the statement
	list_record			search(const Statement& st) const; 
	bool				validate(const Record& rec);

	//reads certain number of records
	bool				readChunk(std::ifstream& in, const int maxReturn);	
	//void				init(std::ifstream& in, const std::string& name);
	friend std::ofstream& operator<<(std::ofstream& out, const Table*& table);

private:
	list_record	searchTableInt(const Statement& st) const;
	list_record	searchTableStr(const Statement& st) const;
	list_record	searchTableDate(const Statement& st) const;
};

