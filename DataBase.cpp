#include "DataBase.hpp"

const std::string DataBase::DB_RELATIVE_PATH = "database/";
const std::string DataBase::DB_FILE = DB_RELATIVE_PATH + "db.bin";

DataBase::DataBase()
{
	out.open(DB_FILE.c_str(), std::ios::binary | std::ios::app);
	in.open(DB_FILE.c_str(), std::ios::binary);
}

DataBase::~DataBase()
{
	out.close();
	in.close();
}

void DataBase::info(const std::string& name)
{
	Table* table = initTable(name);
	
	if (table) table->info(size(name));
	else std::cerr << "No such table!" << std::endl;
}

void DataBase::list()
{
	std::list<Table*> database = get_tables();

	if (database.size() == 0) std::cout << "There are no tables in the database!" << std::endl;
	else if (database.size() == 1) std::cout << "There is 1 table in the database:" << std::endl;
	else std::cout << "There are " << database.size() << " tables in the database:" << std::endl;
	
	for (const Table* const table : database)
		std::cout << "\t" << table->name() << std::endl;
}

bool DataBase::create_table(const std::string& name, const table_row& names, const table_row& types)
{
	const Table* table = new Table(name, Schema(names, types));

	if(!write_string(out, table->name())) return false;

	if (!(out << table->getSchema())) return false;

	save();

	std::string fileName = DB_RELATIVE_PATH + table->name() + ".bin";
	std::ofstream file = getStreamOUT(fileName); //create the table file
	file.close();

	return true;
}

void DataBase::remove_table(const std::string& table)
{
	std::ofstream temp("temp.bin", std::ios::binary);

	std::string toRemove = table + ".bin";
	if (std::remove(toRemove.c_str())) {						//delete old file
		std::cerr << "Remove failed!" << std::endl;
		std::remove("temp.bin");
		return;
	}

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

	std::rename("temp.bin", toRemove.c_str());
}

void DataBase::insert_records(const std::string& table_name, const std::list<Record>& recs)
{
	std::ofstream table = getStreamOUT(table_name);
	if (!table.is_open()) {
		std::cerr << "No such table!" << std::endl;
		return;
	}

	for(const Record& rec: recs)
		table << rec;

	table.close();
}

void DataBase::remove_records(const std::string& table, std::list<Record>& found)
{
	std::ifstream from = getStreamIN(table);
	std::ofstream to = getStreamOUT("temp");

	Record cur;
	bool same = false;
	while (from >> cur) {								//move other content to new file
		for (const Record rec : found) {
			if (rec == cur) {
				same = true;
				break;
			}
		}

		if(!same) to << cur;
		same = false;
	}

	from.close();
	to.close();

	std::string del = DB_RELATIVE_PATH + table + ".bin";
	std::string tmp = DB_RELATIVE_PATH + "temp.bin";

	int res = std::remove(del.c_str());
	if( res != 0 ){		//delete old file
		std::cerr << "Remove failed!" << strerror(errno) << '\n';
		std::remove(tmp.c_str());
		return;
	}

	std::rename(tmp.c_str(), del.c_str());
}

void DataBase::removeQuery(const std::string& tableName, const std::string& query)
{
	Table* table = initTable(tableName);
	if (!table) return;
	std::ifstream tableFile = getStreamIN(tableName);

	BinaryQueryTree bq_tree(table);
	bq_tree.init(query);

	while(table->getContents(tableFile, MAX_ROWS_RETURN)){
		
		tableFile.close();
		std::list<Record> found = bq_tree.search();

		remove_records(tableName, found);

		table->clear();
		tableFile = getStreamIN(tableName);
	}

	tableFile.close();
	table->reset();
}

void DataBase::selectAll(const std::string& tableName, const std::string& query)
{
	Table* table = initTable(tableName);
	if (!table) return;
	std::ifstream tableFile = getStreamIN(tableName);

	BinaryQueryTree bq_tree(table);
	bq_tree.init(query);

	while (table->getContents(tableFile, MAX_ROWS_RETURN)) {

		tableFile.close();
		std::list<Record> found = bq_tree.search();

		std::cout << tableName << std::endl;
		printSelected(found);

		table->clear();
		tableFile = getStreamIN(tableName);
	}

	tableFile.close();
	table->reset();

	//print elements in every iteration
}

void DataBase::selectSome(const std::string& tableName, const table_row& cols, const std::string& query) 
{
	Table* table = initTable(tableName);
	std::vector<bool> selectedColumns = table->getSchema().columns(cols);

	if (!table) return;
	std::ifstream tableFile = getStreamIN(tableName);

	BinaryQueryTree bq_tree(table);
	bq_tree.init(query);

	while (table->getContents(tableFile, MAX_ROWS_RETURN)) {

		tableFile.close();
		std::list<Record> found = bq_tree.search();

		std::cout << tableName << std::endl;

		printSelected(found, selectedColumns);

		table->clear();
		tableFile = getStreamIN(tableName);
	}

	tableFile.close();
	table->reset();
}

std::list<Table*> DataBase::get_tables()
{
	std::list<Table*> rtrn;
	std::string tableName;
	
	std::string name;
	Schema schema;

	while (true) {
		read_string(in, name);
		in >> schema;

		if (!in) break;
		rtrn.push_back(new Table(name, schema));
	}

	return rtrn;
}

Table* DataBase::getTable(const std::string& tableName) const
{
	Table* table = new Table(tableName);
	std::ifstream tableFile = getStreamIN(tableName);

	if (tableFile.is_open()) {
		tableFile >> table;
		tableFile.close();
	}

	return table;
}

Table* DataBase::initTable(const std::string& tableName) 
{
	std::string name;
	Schema schema;
	do {
		read_string(in, name);
		in >> schema;
		if (!in) break;
	} while (name != tableName);

	if (name != "") return new Table(name, schema);
	else return nullptr;
}

void DataBase::save()
{
	in.close();
	out.close();
	in.open(DB_FILE.c_str(), std::ios::binary);
	out.open(DB_FILE.c_str(), std::ios::binary | std::ios::app);
}

uint64_t DataBase::size(const std::string& name) const
{
	std::fstream stream((name + ".bin").c_str());

	stream.seekg(0L, SEEK_END);
	uint64_t ans = stream.tellg();
	stream.close();

	return ans;
}

void DataBase::printSelected(const std::list<Record>& found) const
{
	for (const Record& rec : found)
		rec.print();
}

void DataBase::printSelected(const std::list<Record>& found, const std::vector<bool>& cols)
{
	for (const Record& rec : found)
		rec.print(cols);
}

std::ifstream DataBase::getStreamIN(const std::string& table) const
{
	std::string path = DB_RELATIVE_PATH + table + ".bin";
	std::ifstream file(path.c_str(), std::ios::binary);
	return file;
}

std::ofstream DataBase::getStreamOUT(const std::string& table) const
{
	std::string path = DB_RELATIVE_PATH + table + ".bin";
	std::ofstream file(path.c_str(), std::ios::binary | std::ios::app);
	return file;
}