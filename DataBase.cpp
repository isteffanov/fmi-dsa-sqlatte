#include "DataBase.hpp"

DataBase::DataBase() {
	database = filemanager.get_tables();
}

DataBase::~DataBase()
{
}

void DataBase::create(const std::string& name, const table_row& names, const table_row& types)
{
	Table* newTable = new Table(name, names, types);
	database.push_back(newTable);
	filemanager.create_table(newTable);
}

void DataBase::drop(const std::string& name)
{
	list_tableptr_it table_it = find(name);
	if (table_it == database.end()) {
		std::cerr << "No such table!" << std::endl;
		return;
	}

	delete (*table_it);
	database.erase(table_it);
	filemanager.remove_table(name);
	std::cout << "Table dropped!" << std::endl;
}

void DataBase::info(const std::string& name)
{
	list_tableptr_it table_it = find(name);

	if (table_it != database.end()) (*table_it)->info();
	else std::cerr << "No such table!" << std::endl;
}

void DataBase::list() const
{
	if (database.size() == 0) std::cout << "There are no tables in the database!" << std::endl;
	else if (database.size() == 1) std::cout << "There is 1 table in the database:" << std::endl;
	else std::cout << "There are " << database.size() << " tables in the database:" << std::endl;

	for (const Table* const table : database)
		std::cout << "\t" << table->name() << std::endl;
}

void DataBase::insert(const std::string& name, const table_row& record)
{
	Table* table = find_ptr(name);
	if (!table) {
		std::cerr << "No such table!" << std::endl;
		return;
	}

	const Record rec = table->insert(record);
	filemanager.insert_record(name, rec);
}

void DataBase::remove(const std::string& name, const std::string& query)
{
	Table* table = find_ptr(name);

	if (table) {
		std::list<Record> found = table->remove(query);
		for (const Record& rec : found)
			filemanager.remove_record(name, rec);
	}
	else std::cerr << "No such table!" << std::endl;
}

void DataBase::selectAll(const std::string& name, const std::string& query) const
{
	Table* table = find_ptr(name);

	if (table) table->selectAll(query);
	else std::cerr << "No such table!" << std::endl;
}

void DataBase::selectSome(const std::string& name, const table_row& cols, const std::string& query) const
{
	Table* table = find_ptr(name);

	if (table) table->selectSome(query, cols);
	else std::cerr << "No such table!" << std::endl;
}

size_t DataBase::size_of(const std::string& table)
{
	Table* searched = find_ptr(table);
	if (!searched) return 0;

	return searched->size();
}

typename DataBase::list_tableptr_it DataBase::find(const std::string& name) 
{
	for (list_tableptr_it it = database.begin(); it != database.end(); ++it)
		if ((*it)->name() == name) return it;

	return database.end();
}

Table* DataBase::find_ptr(const std::string& name) const
{
	for (Table* table : database)
		if (table->name() == name) return table;

	return nullptr;
}

std::ifstream& operator>>(std::ifstream& in, DataBase*& db)
{
	size_t size;
	in.read((char*)&size, sizeof(size_t));

	std::string cur;
	for (int i = 0; i < size; ++i) {
		read_string(in, cur);

		std::ifstream file(cur);
		if (file.is_open()) {
			Table* ptr;
			file >> ptr;
			db->database.push_back(ptr);

			file.close();
		}
	}
	return in;
}

std::ofstream& operator<<(std::ofstream& out, const DataBase*& db)
{
	size_t size = db->database.size();
	out.write((const char*)&size, sizeof(size_t));

	for (const Table* table : db->database)
		write_string(out, table->name());

	return out;
}
