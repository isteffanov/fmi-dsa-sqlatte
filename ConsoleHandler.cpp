#include "ConsoleHandler.hpp"

ConsoleHandler::ConsoleHandler()
{
	db = new DataBase();
}

ConsoleHandler::~ConsoleHandler()
{
	delete db;
}

void ConsoleHandler::begin()
{
	do {
		std::cout << "SQLatte> ";
		std::cin >> command;

		tolower(command);

		if      (command == "create_table")	create_table();
		else if (command == "drop_table")	drop_table();
		else if (command == "list_tables")	list_tables();
		else if (command == "table_info")	table_info();
		else if (command == "insert_into")	insert();
		else if (command == "remove")	remove();
		else if (command == "select") 	select();

	} while (command != "quit");

	
}


void ConsoleHandler::create_table()
{
	if (!createTableHelper()) {
		std::cerr << "Wrong input format!" << std::endl;
		std::cin.clear();
	}
}

void ConsoleHandler::drop_table()
{
	std::string table;
	std::cin >> table;

	std::string answer;
	do {
		std::cout << "Are you sure you want to drop that table? [y/N]: ";
		std::cin >> answer;

		tolower(answer);

		if (answer == "yes" || answer == "y") {
			db->drop(table);
			break;
		}
		else if (answer == "no" || answer == "n") return;

		std::cout << "\nPlease reply with 'yes' or 'no'!\n";
	} while (true);
}

void ConsoleHandler::list_tables() const
{ 
	db->list();
}

void ConsoleHandler::table_info() const
{
	std::string table;
	std::cin >> table;

	db->info(table);
}

void ConsoleHandler::insert()
{
	std::string table;
	std::cin >> table;

	std::list<table_row> rows;
	if (!insertIntoHelper(rows)) {
		std::cerr << "Wrong format! Please check the schema!" << std::endl;
		return;
	}

	for (const table_row& row : rows)
		db->insert(table, row);
}

void ConsoleHandler::remove()
{
	if (!removeHelper()) {
		std::cerr << "Wrong input format!" << std::endl;
		std::cin.clear();
	}
}

void ConsoleHandler::select()
{
	if (!selectHelper()) {
		std::cerr << "Wrong input format!" << std::endl;
		std::cin.clear();
	}	
}

bool ConsoleHandler::createTableHelper()
{
	std::string name;
	table_row names, types;

	std::cin >> name;

	std::string line, cur;
	std::getline(std::cin, line);

	bool type = false;
	for (char c : line) {
		if (c == ' ' || c == '(') continue;
		if (c == ')' && type) {
			types.push_back(cur);
			break;
		}

		if (c == ':') {

			if (!type) {
				names.push_back(cur);
				type = !type;

				cur.erase();
			}
			else return false;
		}
		else if (c == ',') {

			if (type) {
				types.push_back(cur);
				type = !type;

				cur.erase();
			}
			else return false;

		}
		else cur.insert(cur.end(), c);
	}

	db->create(name, names, types);
	return true;
}

bool ConsoleHandler::selectHelper()
{
	std::string line;
	std::getline(std::cin, line);
	tolower(line);

	size_t whereBegin = line.find("where");
	if (whereBegin == std::string::npos) return false;

	std::string details = line.substr(0, whereBegin);
	std::string query = line.substr(whereBegin + strlen("where"));

	size_t fromBegin = details.find("from");
	if (fromBegin == std::string::npos) return false;

	std::string table = details.substr(fromBegin + strlen("from"));
	removeWhitespace(table);
	details = details.substr(0, fromBegin);

	bool all = false;
	std::string cur;
	std::vector<std::string> cols;

	std::string::iterator it = details.begin();

	while (*it == ' ')
		++it;

	if (*it == '*') all = true;
	else {

		while (true) {

			while (it != details.end() && *it == ' ') 
				++it;
			
			while (isLetterOrUnderscore(*it))
				cur.push_back(*it++);

			cols.push_back(cur);
			cur.clear();

			while (it != details.end() && *it == ' ') 
				++it;

			if (it != details.end() && *it == ',') {
				++it;
				continue;
			}

			if (it == details.end() || *it == ' ') break;

			return false;
		}
	}

	if (all)	db->selectAll(table, query);
	else		db->selectSome(table, cols, query);
}

bool ConsoleHandler::removeHelper()
{
	std::string table, where;
	std::cin >> table;

	std::cin >> where;
	if (where == "where") {

		std::string query;
		std::getline(std::cin, query);
		tolower(query);

		db->remove(table, query);
	}

	return true;
}

bool ConsoleHandler::insertIntoHelper(std::list<table_row>& rows)
{
	char c;
	std::string cur_col;
	table_row cur_row;

	std::cin >> c;
	while (c != '}') {

		std::cin >> c;
		if (c != '(') return false;
			
		while (c != ')'){

			std::cin >> c;
			while (c == '"' || c == '\'' || c == ' ') //skip whitespace and quotes
				std::cin >> c;

			while (c != ',' && c != ')') {							//read the value
				cur_col.push_back(c);
				std::cin >> c;
			}

			cur_row.push_back(cur_col);
			cur_col.clear();
		}
		
		rows.push_back(cur_row);
		cur_row.clear();

		std::cin >> c;				
	}

	return true;
}

void ConsoleHandler::removeWhitespace(std::string& str)
{
	std::string cpy = str;
	str.clear();

	for (std::string::iterator it = cpy.begin(); it != cpy.end(); ++it)
		if (*it != ' ') str.push_back(*it);	
}

bool ConsoleHandler::isLetterOrUnderscore(char c) const
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

void ConsoleHandler::tolower(std::string& word)
{
	std::transform(word.begin(), word.end(), word.begin(),
		[](unsigned char c) { return std::tolower(c); });
}