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

void DataBase::list()
{
	list_tableptr database = tables();

	size_t size = 0;
	for (list_tableptr_it it = database.begin(); it != database.end(); ++it)
		size++;

	if (size) std::cout << "There are no tables in the database!" << std::endl;
	else if (size) std::cout << "There is 1 table in the database:" << std::endl;
	else std::cout << "There are " << size << " tables in the database:" << std::endl;
	
	for (const Table* const table : database)
		std::cout << "\t" << table->name() << std::endl;

	for (Table*& table : database)
		delete table;
}

void DataBase::info(const std::string& name)
{
	Table* table = initTable(name);
	
	if (table) {
		table->info(count(name), size(name));
		delete table;
	}
	else std::cerr << "No such table!" << std::endl;
}

bool DataBase::create(const std::string& name, const table_row& names, const table_row& types)
{
	if(!write_string(out, name)) return false;
	if (!(out << Schema(names, types)) ) return false;

	save();
	reset();
	std::ofstream file = getStreamOUT(name); //create the table file
	
	if (!file.is_open()) return false;
	file.close();

	return true;
}

bool DataBase::drop(const std::string& table)
{
	std::string toRemove = getTablePath(table);
	if (std::remove(toRemove.c_str()) != 0) {						
		std::cerr << "Remove failed!" << std::endl;
		return false;
	}

	std::ofstream temp = getStreamOUT("temp");

	std::string name;
	Schema schema;
	while (read_string(in, name) && in >> schema) {
		if (name == table) continue;

		write_string(temp, name);
		temp << schema;
	}

	std::rename("temp", toRemove.c_str());
	return true;
}

bool DataBase::insert(const std::string& name, const list_record& recs)
{
	Table* table = initTable(name);
	for (const Record& rec : recs)
		if (rec.size() != table->schema().size()) 
			return false;

	std::ofstream file = getStreamOUT(name);
	if (!file.is_open()) {
		std::cerr << "No such table!" << std::endl;
		return false;
	}

	for(const Record& rec: recs)
		file << rec;

	file.close();
	delete table;
}

void DataBase::remove(const std::string& tableName, const std::string& query)
{
	Table* table = initTable(tableName);
	std::ifstream tableFile = getStreamIN(tableName);
	if (!table || !tableFile.is_open()) {
		std::cout << "Table missing!" << std::endl;
		return;
	}

	BinaryQueryTree bq_tree(table);
	bq_tree.init(query);

	while(table->read_chunk(tableFile, MAX_ROWS_RETURN)){
		
		tableFile.close();
		list_record found = bq_tree.search();

		removeRecords(tableName, found);

		table->clear();
		tableFile = getStreamIN(tableName);
	}

	tableFile.close();
	delete table;
	reset();
}

void DataBase::select(const std::string& table, const std::string query,
	const table_row cols, bool distinct, const std::string& orderBy, bool asc)
{
	if (cols.empty())	selectAll(table, query, distinct, orderBy, asc);
	else				selectSome(table, cols, query, distinct, orderBy, asc);
}

uint64_t DataBase::size(const std::string& name) const
{
	std::ifstream stream = getStreamIN(name);

	stream.seekg(0L, SEEK_END);
	uint64_t ans = stream.tellg();
	stream.close();

	return ans;
}

size_t DataBase::count(const std::string& name) const
{
	std::ifstream file = getStreamIN(name);
	if (!file.is_open()) return 0;

	size_t cnt = 0;
	Record cur;
	while (file >> cur) {
		cnt++;
	}

	file.close();
	return cnt;
}

void DataBase::removeRecords(const std::string& table, list_record& found)
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

		if (!same) to << cur;
		same = false;
	}

	from.close();
	to.close();

	std::string del = getTablePath(table);
	std::string tmp = getTablePath("temp");

	if (std::remove(del.c_str()) != 0) {		//delete old file
		std::cerr << "Remove failed!" << '\n';
		std::remove(tmp.c_str());
		return;
	}

	std::rename(tmp.c_str(), del.c_str());
}

void DataBase::selectAll(const std::string& tableName, const std::string& query, 
	bool distinct, const std::string& orderBy, bool asc)
{
	Table* table = initTable(tableName);
	std::ifstream tableFile = getStreamIN(tableName);
	if (!table || !tableFile.is_open()) {
		std::cout << "Table missing!" << std::endl;
		return;
	}

	BinaryQueryTree bq_tree(table);
	bq_tree.init(query);

	list_record found, searched;
	while (table->read_chunk(tableFile, MAX_ROWS_RETURN)) {
		
		searched = bq_tree.search();
		if (distinct) unify(found, searched);
		else append(found, searched);

		table->clear();
	}

	if (!orderBy.empty()) {
		size_t pos = table->schema().pos(orderBy);
		bool isDate = table->schema().date(orderBy);
		found.sort(Record::Compare(pos, isDate, asc));
	}

	std::cout << tableName << std::endl;
	table->print_schema();
	printSelected(found);

	tableFile.close();
	table->reset();
	delete table;

	save();
	reset();
}

void DataBase::selectSome(const std::string& tableName, const table_row& cols,
	const std::string& query, bool distinct, const std::string& orderBy, bool asc)
{
	Table* table = initTable(tableName);
	std::ifstream tableFile = getStreamIN(tableName);
	if (!table || !tableFile.is_open()) {
		std::cout << "Table missing!" << std::endl;
		return;
	}

	BinaryQueryTree bq_tree(table);
	bq_tree.init(query);

	list_record found, searched;
	while (table->read_chunk(tableFile, MAX_ROWS_RETURN)) {

		searched = bq_tree.search();
		if (distinct) unify(found, searched);
		else append(found, searched);

		table->clear();
	}

	std::cout << tableName << std::endl;
	if (!orderBy.empty()) {
		size_t pos = table->schema().pos(orderBy);
		bool isDate = table->schema().date(orderBy);
		found.sort(Record::Compare(pos, isDate, asc));
	}

	std::vector<bool> selectedColumns = table->schema().columns(cols);
	table->print_schema(selectedColumns);
	printSelected(found, selectedColumns);

	tableFile.close();
	table->reset();
	delete table;

	save();
	reset();
}

list_tableptr DataBase::tables()
{
	list_tableptr rtrn;
	std::string tableName;
	
	std::string name;
	Schema schema;

	while (true) {
		read_string(in, name);
		in >> schema;

		if (!in) break;
		rtrn.push_front(new Table(name, schema));
	}

	return rtrn;
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

void DataBase::reset()
{
	in.seekg(SEEK_SET);
	out.seekp(SEEK_SET);
}

void DataBase::printSelected(list_record& found)
{
	for (const Record& rec : found)
		rec.print();

	std::cout << getSize(found) << " rows selected!" << std::endl;
}

void DataBase::printSelected(list_record& found, const std::vector<bool>& cols)
{
	for (const Record& rec : found)
		rec.print(cols);

	std::cout << getSize(found) << " rows selected!" << std::endl;
}

std::ifstream DataBase::getStreamIN(const std::string& table) const
{
	std::string path = getTablePath(table);
	std::ifstream file(path.c_str(), std::ios::binary);
	return file;
}

std::ofstream DataBase::getStreamOUT(const std::string& table) const
{
	std::string path = getTablePath(table);
	std::ofstream file(path.c_str(), std::ios::binary | std::ios::app);
	return file;
}

std::string DataBase::getTablePath(const std::string& table) const
{
	return DB_RELATIVE_PATH + table + ".bin";
}
