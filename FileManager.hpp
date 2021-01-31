#pragma once
#include <fstream>
#include <string>

#include "read_write_helpers.hpp"
#include "Table.hpp"

class FileManager
{
	static const std::string DB_FILE;

	std::ifstream in;
	std::ofstream out;

public:
	FileManager();
	FileManager(const FileManager&) = delete;
	const FileManager& operator=(const FileManager&) = delete;
	~FileManager();

	void create_table(const Table* table);
	void remove_table(const std::string& table);
	void insert_record(const std::string& table, const Record& rec);
	void remove_record(const std::string& table, const Record& rec);

	std::list<Table*> get_tables();
};

