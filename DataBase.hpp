#pragma once
#include <fstream>
#include <string>

#include "BinaryQueryTree.hpp"
#include "read_write_helpers.hpp"
#include "Table.hpp"
#include "helper.hpp"

#include "typedefs.hpp"
class DataBase
{
	static const std::string DB_RELATIVE_PATH;
	static const std::string DB_FILE;

	std::ifstream in;
	std::ofstream out;

	static const int MAX_ROWS_RETURN = 2;
public:
	DataBase();
	DataBase(const DataBase&) = delete;
	const DataBase& operator=(const DataBase&) = delete;
	~DataBase();

	void info(const std::string& name);
	void list();

	bool create_table(const std::string& name, const table_row& names, const table_row& types);
	void remove_table(const std::string& table);
	void insert_records(const std::string& table, const std::list<Record>& rec);
	void remove_records(const std::string& table, std::list<Record>& found);
	void select(const std::string& table, const std::string query, const std::vector<std::string> cols,
				bool distinct, const std::string& orderBy, bool asc);
	void removeQuery(const std::string& table, const std::string& query);


	std::list<Table*> get_tables();

private:
	void append(std::list<Record>& to, const std::list<Record>& what);

	Table* getTable(const std::string& tableName) const;
	Table* initTable(const std::string& tableName); //get empty table with its schema

	void selectAll(const std::string& name, const std::string& query, 
		bool distinct, const std::string& orderBy, bool asc);
	void selectSome(const std::string& name, const table_row& cols, 
		const std::string& query, bool distinct, const std::string& orderBy, bool asc);

	void save();
	uint64_t size(const std::string& name) const;
	void printSelected(const std::list<Record>& found) const;
	void printSelected(const std::list<Record>& found, const std::vector<bool>& cols);
	std::ifstream getStreamIN(const std::string& table) const;
	std::ofstream getStreamOUT(const std::string& table) const;
};

