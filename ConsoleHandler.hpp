#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include "DataBase.hpp"

class ConsoleHandler
{
	std::fstream file;
	std::string command;
	DataBase* db;

public:
	ConsoleHandler();

	void begin();

private:
	void create_table();
	void drop_table();
	void list_tables();
	void table_info();

	void create_table_wrap();

	void tolower(std::string& word);
};

