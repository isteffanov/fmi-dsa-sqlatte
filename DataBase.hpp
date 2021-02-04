#pragma once
#include <fstream>
#include <list>
#include <string>

#include "BinaryQueryTree.hpp"
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

	void list();
	void info(const std::string& name);

	bool create(const std::string& name, const table_row& names, const table_row& types);
	bool drop(const std::string& table);
	bool insert(const std::string& table, const std::list<Record>& rec);
	void remove(const std::string& table, const std::string& query);
	void select(const std::string& table, const std::string query, const std::vector<std::string> cols,
				bool distinct, const std::string& orderBy, bool asc);


	std::list<Table*> get_tables();

private:
	void remove_records(const std::string& table, std::list<Record>& found);
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
	std::string getTablePath(const std::string& table) const;
};

