#pragma once
#include <iostream>
#include <list>
#include <string>

#include "FileManager.hpp"
#include "Table.hpp"
#include "read_write_helpers.hpp"

class DataBase
{
#include "typedefs.ipp"

	std::list<Table*> database;
	FileManager filemanager;

public:
	DataBase();
	DataBase(const DataBase& other) = delete;
	DataBase& operator=(const DataBase& other) = delete;
	~DataBase();

	void				create(const std::string& name, const table_row& names, const table_row& types);
	void				drop(const std::string& name);
	void				info(const std::string& name);
	void				list() const;

	void				insert(const std::string& name, const table_row& record);
	void				remove(const std::string& name, const std::string& query);
	void				selectAll(const std::string& name, const std::string& query) const;
	void				selectSome(const std::string& name, const table_row& cols, const std::string& query) const;
	size_t				size_of(const std::string& table);

	friend std::ifstream& operator>>(std::ifstream& in, DataBase*& table);
	friend std::ofstream& operator<<(std::ofstream& out, const DataBase*& table);

private:
	list_tableptr_it	find(const std::string& name);
	Table*				find_ptr(const std::string& name) const;
};

