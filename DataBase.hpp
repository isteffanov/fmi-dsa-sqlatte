#pragma once
#include <iostream>
#include <list>
#include <string>

#include "Table.hpp"

class DataBase
{
	typedef std::list<Table*>::iterator list_tableptr_it;

	std::list<Table*> database;

public:
	DataBase();
	DataBase(const DataBase& other) = delete;
	DataBase& operator=(const DataBase& other) = delete;

	void	create(const std::string& name, std::vector<std::string> names, std::vector<std::string> types);
	void	drop(const std::string& name);
	void	info(const std::string& name);
	void	list();

private:
	list_tableptr_it find(const std::string& name);

};

