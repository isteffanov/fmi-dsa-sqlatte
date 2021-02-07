#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <regex>
#include <vector>

#include "DataBase.hpp"
#include "helper.hpp"

/**
 * @brief The class handels all user input, most of the validation 
 *  and passes them to the database.
 * @details The accepted format for each command is stated down below. 
 * The input is case insensitive.
 * @note <> means parameter.
 * @note [] means optional. 
 * @note | means or.
 */
class ConsoleHandler
{
	DataBase* db;

public:
	ConsoleHandler();
	ConsoleHandler(const ConsoleHandler&) = delete;
	const ConsoleHandler& operator=(const ConsoleHandler&) = delete;
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

	bool createTableHelper();
	bool selectHelper();
	bool removeHelper();
	bool insertIntoHelper();

	bool assert_input(const std::string& line, const std::string& expr);
	void tolower(std::string& word);
};

