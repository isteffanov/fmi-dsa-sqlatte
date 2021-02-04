#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <regex>

#include "DataBase.hpp"

#include "typedefs.hpp"

class ConsoleHandler
{

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
	std::string findMatch(const std::string& str, const std::regex& reg);
	std::vector<std::string> findMatches(std::string& str, std::regex& reg);

	bool createTableHelper();
	bool selectHelper();
	bool removeHelper();
	bool insertIntoHelper(std::list<Record>& rows);

	std::string runRegex(const std::string& line, const std::string& expr);
	const table_row getSelectedColumns(std::string& select);

	void removeWhitespace(std::string& str);
	bool isLetterOrUnderscore(char c) const;
};

