#include "DataBase.hpp"

DataBase::DataBase() {}

void DataBase::create(const std::string& name, std::vector<std::string> names, std::vector<std::string> types)
{
	Table* newTable = new Table(name, names, types);
	database.push_back(newTable);
}

void DataBase::drop(const std::string& name)
{
	try {
		list_tableptr_it table_it = find(name);
		delete *table_it;
		database.erase(table_it);
		std::cout << "Table dropped!" << std::endl;
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

void DataBase::info(const std::string& name)
{
	try {
		list_tableptr_it table = find(name);
		(*table)->info();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

void DataBase::list()
{
	if (database.size() == 0) std::cout << "There are no tables in the database!" << std::endl;
	else if (database.size() == 1) std::cout << "There is 1 table in the database:" << std::endl;
	else std::cout << "There are " << database.size() << " tables in the database:" << std::endl;

	for (const Table* const table : database)
		std::cout << "\t" << table->name() << std::endl;
}

DataBase::list_tableptr_it DataBase::find(const std::string& name)
{
	for (list_tableptr_it it = database.begin(); it != database.end(); ++it)
		if ((*it)->name() == name) return it;

	throw std::exception("No such table!");
}


