#include "FileManager.hpp"

const std::string FileManager::DB_FILE = "db.bin";

FileManager::FileManager()
{
	out.open(DB_FILE.c_str(), std::ios::binary | std::ios::app);
	in.open(DB_FILE.c_str(), std::ios::binary);
}

FileManager::~FileManager()
{
	out.close();
	in.close();
}

void FileManager::create_table(const Table* table)
{
	if(!write_string(out, table->name())) return;

	std::string name = table->name() + ".bin";
	std::ofstream new_table(name.c_str(), std::ios::binary);
	if (new_table.is_open()) {
		new_table.close();

		std::ofstream new_table(name.c_str(), std::ios::binary);
		new_table << table;
	}
}

void FileManager::remove_table(const std::string& table)
{
	std::ofstream temp("temp.bin", std::ios::binary);

	size_t len;
	char* ptr;
	while (in.read((char*)&len, sizeof(size_t))) {								//move other content to new file
		ptr = new char[len + 1];

		in.read(ptr, len);
		ptr[len] = '\0';

		if (!table.compare(ptr)) {
			delete[] ptr;
			continue;
		}

		temp.write((const char*)&len, sizeof(size_t));
		temp.write(ptr, len);

		delete[] ptr;
	}

	std::string toRemove = table + ".bin";
	if (remove(toRemove.c_str())) {						//delete old file
		std::cerr << "Remove failed!" << std::endl;
		remove("temp.bin");
		return;
	}

	rename("temp.bin", toRemove.c_str());
}

void FileManager::insert_record(const std::string& table_name, const Record& rec)
{
	std::string name = table_name + ".bin";
	std::ofstream table(name.c_str(), std::ios::app | std::ios::binary);
	if (!table.is_open()) {
		std::cerr << "No such table!" << std::endl;
		return;
	}

	table << rec;

	table.close();
}

void FileManager::remove_record(const std::string& table, const Record& rec)
{
	std::string toRemove = table + ".bin";
	std::ifstream from(toRemove.c_str(), std::ios::binary);
	std::ofstream to("temp.bin", std::ios::binary);

	Schema schema;
	from >> schema;	
	to << schema;

	Record cur;
	while (from >> cur) {								//move other content to new file
		if (cur == rec) continue;
		
		to << cur;
	}

	from.close();
	to.close();

	if (remove(toRemove.c_str())) {						//delete old file
		std::cerr << "Remove failed!" << std::endl;
		remove("temp.bin");
		return;
	}

	rename("temp.bin", toRemove.c_str());
}

std::list<Table*> FileManager::get_tables()
{
	std::list<Table*> rtrn;

	std::string fileName, tableName;
	
	while (read_string(in, tableName)) {
		Table* cur = new Table(tableName);
		fileName = tableName + ".bin";

		std::ifstream tableFile(fileName.c_str(), std::ios::binary);
		if (tableFile.is_open()) {
			tableFile >> cur;
			tableFile.close();
			rtrn.push_back(cur);
		}		
	}


	return rtrn;

}
