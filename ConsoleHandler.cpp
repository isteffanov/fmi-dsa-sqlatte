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
		else if (command == "insert")	insert();
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

			if (db->drop(table)) 
				std::cout << "Table dropped successfully!" << std::endl;
			else 
				std::cout << "Could not drop table!" << std::endl;

			return;
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
	if (!(insertIntoHelper())) {
		std::cerr << "Wrong format!" << std::endl;
		return;
	}
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
	std::string line;
	std::getline(std::cin, line);

	std::string name = findMatch(line, "^\\s*(\\w+)\\s+");

	std::regex rem("(,)");
	line = std::regex_replace(line, rem, "");

	line = findMatch(line, "\\((.*?)\\)");

	std::vector<std::string> names = findMatches(line, "(\\w+(?=:))");
	std::vector<std::string> types = findMatches(line, ":\\s*(\\w+)");

	std::regex rem3(":\\s*");
	for (std::string& type : types)
		type = std::regex_replace(type, rem3, "");  //i could not escape ":" otherwise
	
	return db->create(name, names, types);
}

bool ConsoleHandler::insertIntoHelper()
{
	std::string line;
	std::getline(std::cin, line);

	std::string table = findMatch(line, "^\\s*into\\s+(\\w+)");
	line = findMatch(line, "\\{(.*?)\\}");
	std::list<Record> rows;

	std::regex rem2("( [ ]+)");
	line = std::regex_replace(line, rem2, " ");

	std::vector<std::string> entries = findMatches(line, "\((?=[^,])([\\w\\s/',]+)\)");

	for (std::string& entry : entries)
		rows.push_back(findMatches(entry, "([\\w\\s/]+$|[\\w\\s/]+(?=,))\\s*"));

	return db->insert(table, rows);
}

bool ConsoleHandler::removeHelper()
{
	std::string line;
	std::getline(std::cin, line);

	std::string table = findMatch(line, "^[ ]*from[ ]+(\\w+)[ ]+where[ ]+[\\w\\s><!=]+");
	std::string query = findMatch(line, "^[ ]*from[ ]+[\\w]+[ ]+where[ ]+([\\w\\s><!=]+)");

	if (table == "" || query == "") return false;

	db->remove(table, query);
	return true;
}

bool ConsoleHandler::selectHelper()
{
	std::string line;
	std::getline(std::cin, line);
	
	std::string dist = findMatch(line, "^\\s*(distinct)");
	std::string table = findMatch(line, "\\s*from\\s+(\\w+)");
	std::string query = findMatch(line, "\\s*from\\s+" + table + "\\s+where([\\w\\s!=<>/]+)$");
	std::string order = findMatch(line, "\\s*from\\s+" + table + "\\s+(order\\s+by[\\s\\w]*)$");
	std::string byWhat = findMatch(order, "\\s*order\\s+by\\s+(\\w+)\\s*(asc|desc)?$");
	std::string cend = findMatch(order, "\\s*order\\s+by\\s+" + byWhat + "\\s+(asc|desc)$");


	bool distinct = (!dist.empty());
	bool asc = (cend == "asc");

	std::string select;
	if (distinct)	select = findMatch(line, "\\s*distinct([\*\\w\\s,]*)from\\s*" + table +
											"(\\s+where\\s*" + query + ")?\\s*$");
	else			select = findMatch(line, "\\s*([\*\\w\\s,]*)from\\s*" + table +
											"(\\s+where\\s*" + query + ")?\\s*");

	if (!order.empty()) query = findMatch(query, "(.*?)" + order + "$");
	
	table_row cols = getSelectedColumns(select);

	if (!cols.empty() && !byWhat.empty()) {
		bool found = false;
		for (const std::string& col : cols)
			if (col == byWhat) found = true;

		if (!found) return false;
	}

	db->select(table, query, cols, distinct, byWhat, asc);
}

const table_row ConsoleHandler::getSelectedColumns(std::string& select)
{
	std::string cur;
	table_row cols;
	if (select.empty()) return cols;

	std::string::iterator it = select.begin();

	while (*it == ' ')
		++it;

	if (*it != '*') {

		while (true) {

			while (it != select.end() && *it == ' ')
				++it;

			while (isLetterOrUnderscore(*it))
				cur.push_back(*it++);

			cols.push_back(cur);
			cur.clear();

			while (it != select.end() && *it == ' ')
				++it;

			if (it != select.end() && *it == ',') {
				++it;
				continue;
			}

			if (it == select.end() || *it == ' ') break;
			
		}
	}

	return cols;
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