#include "DataBase.hpp"

const std::string DataBase::DB_RELATIVE_PATH = "database/";
const std::string DataBase::DB_FILE = DB_RELATIVE_PATH + "db.bin";

/**
 * @details When the database is initialized, a database file is
 *  created, which stores all the tables' names and schemas.
 */
DataBase::DataBase()
{
	open();
}

DataBase::~DataBase()
{
	close();
}

/**
 * @brief Prints all the table's names.
 */
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

/**
 * @brief Prints information about a given table.
 */
void DataBase::info(const std::string& name)
{
	Table* table = initTable(name);
	
	if (table) {
		table->info(count(name), size(name));
		delete table;
	}
	else std::cerr << "No such table!" << std::endl;
}

/**
 * @details The table's info is splitted between two files.
 *	The table's name and schema are saved in the database file.
 *	The table's contents are saved in a dedicated file for it.
 */
bool DataBase::create(const std::string& name, const table_row& names, const table_row& types)
{
	if(!write_string(out, name)) return false;
	if (!(out << Schema(names, types)) ) return false;

	std::ofstream file = getStreamOUT(name); //create the table file
	
	if (!file.is_open()) return false;
	file.close();

	save();
	reset();

	std::cout << "Table " << name << " created!" << std::endl;
	return true;
}

/**
 * @brief Deletes a given table.
 * @details Firstly, the table's file is deleted, then
 * the database file is overwritten, omitting the 
 * deleted table's data.
 */
bool DataBase::drop(const std::string& table)
{
	std::string toRemove = getTablePath(table);
	if (std::remove(toRemove.c_str()) != 0) 
		return false;
	
	std::ofstream temp = getStreamOUT("temp");

	std::string name;
	Schema schema;

	//copy each table's info from the db file to the temp file
	while (read_string(in, name) && in >> schema) {
		if (name == table) continue;

		write_string(temp, name);
		temp << schema;
	}

	std::string tempName = getTablePath("temp");

	close();
	temp.close();

	if (std::remove(DB_FILE.c_str()) != 0) {
		std::remove(tempName.c_str());
		return false;
	}
	
	std::rename(tempName.c_str(), DB_FILE.c_str());
	open();
	return true;
}

/**
 * @brief Adds rows to a table's file
 */
bool DataBase::insert(const std::string& name, const list_record& recs)
{
	Table* table = initTable(name);
	//validate the received records
	for (const Record& rec : recs) {
		if (rec.size() != table->schema().size()) 
			return false;
		if (!table->validate(rec)) 
			return false;
	}

	std::ofstream file = getStreamOUT(name);
	if (!file.is_open()) {
		std::cerr << "No such table!" << std::endl;
		return false;
	}

	size_t size = 0;
	for (const Record& rec : recs) {
		file << rec;
		size++;
	}

	file.close();
	delete table;

	std::cout << "Inserted " << size << " records!" << std::endl;
	return true;
}

/**
 * @brief Removes the rows from a given table which match the query.
 * @details The query is passed to a BinaryQueryTree, which 
 *	is searched and then the resulting records are removed.
 * @see BinaryTreeNote
 */

void DataBase::remove(const std::string& tableName, const std::string& query)
{
	Table* table = initTable(tableName);
	std::ifstream tableFile = getStreamIN(tableName);
	if (!table || !tableFile.is_open()) {
		std::cout << "Table missing!" << std::endl;
		return;
	}

	BinaryQueryTree bq_tree(table, query);

	//read the table file chuck by chunk 
	//and load the matching records
	list_record searched, found;
	while(table->readChunk(tableFile, MAX_ROWS_RETURN)){

		searched = bq_tree.search();
		append(found, searched);
	}

	tableFile.close();
	delete table;
	reset();

	removeRecords(tableName, found);	
}

/**
 * @param cols is a vector, containing all the columns' names 
 *	user wants to see.
 * @param distinct If true, the user does not want dublucates.
 * @param orderBy Is a column name, by which the user wants their data sorted.
 * @param asc If true, the user wants their data sorted in ascending
 *	order. Defaults to true.
*/
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

/**
 * @return the number of tables in the database.
 */
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

	//move other content to new file
	Record cur;
	bool same = false;
	while (from >> cur) {								
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

	BinaryQueryTree bq_tree(table, query);

	//read the table file chuck by chuck  
	//until we have all the matches in a list
	list_record found, searched;
	while (table->readChunk(tableFile, MAX_ROWS_RETURN)) {
		
		try {
			searched = bq_tree.search();
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return;
		}
		append(found, searched);

		table->clear();
	}
	if (distinct) removeDuplicates(found);

	//sort the found matches if requested
	if (!orderBy.empty()) {
		size_t pos = table->schema().pos(orderBy);
		bool isDate = table->schema().date(orderBy);
		found.sort(Record::Compare(pos, isDate, asc));
	}

	std::cout << tableName << std::endl;
	table->schema().print();
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

	BinaryQueryTree bq_tree(table, query);

	//read the table file chuck by chuck  
	//until we have all the matches in a list
	list_record found, searched;
	while (table->readChunk(tableFile, MAX_ROWS_RETURN)) {

		try {
			searched = bq_tree.search();
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return;
		}
		append(found, searched);

		table->clear();
	}
	if (distinct) removeDuplicates(found);

	//sort the found matches if requested
	if (!orderBy.empty()) {
		size_t pos = table->schema().pos(orderBy);
		bool isDate = table->schema().date(orderBy);
		found.sort(Record::Compare(pos, isDate, asc));
	}
	std::vector<bool> selectedColumns = table->schema().columns(cols);

	std::cout << tableName << std::endl;
	table->schema().print(selectedColumns);
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

	reset();
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

	reset();
	if (name != "") return new Table(name, schema);
	else return nullptr;
}

void DataBase::save()	
{
	close();
	open();	
}

void DataBase::open()
{
	in.open(DB_FILE.c_str(), std::ios::binary);
	out.open(DB_FILE.c_str(), std::ios::binary | std::ios::app);
}

void DataBase::close()
{
	in.close();
	out.close();
}

void DataBase::reset()
{
	in.seekg(SEEK_SET);
	out.seekp(SEEK_SET);
}

void DataBase::removeDuplicates(list_record& found)
{
	list_record temp;

	size_t size = getSize(found);

	bool dublicate;
	for (list_record_it it = found.begin(); it != found.end(); ++it) {
		dublicate = false;
		list_record_it next = it;
		while (++next != found.end()) {
			if (*it == *next) {
				dublicate = true;
				break;
			}
		}
		if (!dublicate) temp.push_front(*it);
	}

	found.clear();
	found = temp;
}

void DataBase::printSelected(const list_record& found)
{
	for (const Record& rec : found)
		rec.print();

	std::cout << getSize(found) << " rows selected!" << std::endl;
}

void DataBase::printSelected(const list_record& found, const std::vector<bool>& cols)
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
