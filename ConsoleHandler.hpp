#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>

#include "DataBase.hpp"

class ConsoleHandler
{
#include "typedefs.ipp"

	std::fstream file;
	std::string command;
	DataBase* db;

public:
	ConsoleHandler();

	void begin();

private:
	void create_table();
	void drop_table();
	void list_tables() const;
	void table_info() const;

	void insert();
	void remove();
	void select();

	void tolower(std::string& word);

	bool createTableHelper();
	bool insertIntoHelper();
};

