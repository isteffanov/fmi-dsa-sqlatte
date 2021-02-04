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
	std::string table;
	std::cin >> table;

	std::list<Record> rows;
	if (!insertIntoHelper(rows)) {
		std::cerr << "Wrong format!" << std::endl;
		return;
	}
	
	db->insert(table, rows);
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

	std::string name = findMatch(line, "^\\s+(\\w+)\\s+");

	std::vector<std::string> names = findMatches(line, "\((\\s*\\w+\\s*(?=:))\)");
	std::vector<std::string> types = findMatches(line, "\(:\\s*(\\w+)\\s*(?=[,)])\\s*\)");

	for (std::string& type : types)				//escape ':' as the c++ regex lib
		type = type.substr(type.find(":") + 1); //does not support look behind

	return db->create(name, names, types);
}

bool ConsoleHandler::insertIntoHelper(std::list<Record>& rows)
{
	char c;
	std::string cur_col;
	table_row cur_row;

	std::cin >> c;
	while (c != '}') {

		std::cin >> c;
		if (c != '(') return false;

		while (c != ')') {

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

		rows.push_back(Record(cur_row));
		cur_row.clear();

		std::cin >> c;
	}

	return true;
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
	std::string query = findMatch(line, "\\s*from\\s+" + table + "\\s+where([\\w\\s!=<>]+)$");
	std::string order = findMatch(line, "\\s*from\\s+" + table + "[\\w\\s!=<>]*(order\\s+by[\\s\\w]*)$");
	std::string byWhat = findMatch(order, "\\s*order\\s+by\\s+(\\w+)\\s*(asc|desc)?$");
	std::string cend = findMatch(order, "\\s*order\\s+by\\s+" + byWhat + "\\s+(asc|desc)$");


	bool distinct = (!dist.empty());
	bool asc = (cend == "asc");

	std::string select;
	if (distinct)	select = findMatch(line, "\\s*distinct([\\w\\s,]*)from\\s*" + table +
											"\\s+where\\s*" + query + "\\s*$");
	else			select = findMatch(line, "([\\w\\s,]*)from\\s*" + table +
											"\\s+where\\s*" + query + "\\s*$");

	if (!order.empty()) query = findMatch(query, "(.*?)" + order + "$");
	
	table_row cols = getSelectedColumns(select);

	if (!cols.empty()) {
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

std::string ConsoleHandler::findMatch(const std::string& str, const std::string& expr) {
	std::regex reg(expr);
	std::smatch match;

	if (std::regex_search(str, match, reg)) 
		return match[1];

	return "";
}

std::vector<std::string> ConsoleHandler::findMatches(std::string& str, const std::string& expr)
{
	std::regex reg(expr);
	std::vector<std::string> rtrn;

	std::smatch match;
	std::sregex_iterator it(str.begin(), str.end(), reg);
	std::sregex_iterator end;

	while (it != end) {
		match = *it;
		rtrn.push_back(match.str());
		it++;
	}

	return rtrn;
}

void ConsoleHandler::tolower(std::string& word)
{
	std::transform(word.begin(), word.end(), word.begin(),
		[](unsigned char c) { return std::tolower(c); });
}