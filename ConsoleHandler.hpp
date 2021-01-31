#pragma once

#include <iostream>
#include <algorithm>
#include <string>

#include "DataBase.hpp"


class ConsoleHandler
{
#include "typedefs.ipp"

	std::string command;
	DataBase* db;

public:
	ConsoleHandler();
	~ConsoleHandler();

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
	bool selectHelper();
	bool removeHelper();
	bool insertIntoHelper(std::list<table_row>& rows);


	void removeWhitespace(std::string& str);
	bool isLetterOrUnderscore(char c) const;
};

