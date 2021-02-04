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
			db->remove_table(table);
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

	std::list<Record> rows;
	if (!insertIntoHelper(rows)) {
		std::cerr << "Wrong format! Please check the schema!" << std::endl;
		return;
	}

	
	db->insert_records(table, rows);
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

	return db->create_table(name, names, types);
}

bool ConsoleHandler::selectHelper()
{
	std::string line;
	std::getline(std::cin, line);
	
	std::string dist = runRegex(line, "^\\s*(distinct)");
	std::string table = runRegex(line, "\\s*from\\s+(\\w+)");
	std::string query = runRegex(line, "\\s*from\\s+" + table + "\\s+where([\\w\\s!=<>]+)$");
	std::string order = runRegex(line, "\\s*from\\s+" + table + "[\\w\\s!=<>]*(order\\s+by[\\s\\w]*)$");
	std::string byWhat = runRegex(order, "\\s*order\\s+by\\s+(\\w+)\\s*(asc|desc)?$");
	std::string cend = runRegex(order, "\\s*order\\s+by\\s+" + byWhat + "\\s+(asc|desc)$");


	bool distinct = (!dist.empty());
	bool asc = (cend == "asc");

	std::string select;
	if (distinct)	select = runRegex(line, "\\s*distinct([\\w\\s,]*)from\\s*" + table +
											"\\s+where\\s*" + query + "\\s*$");
	else			select = runRegex(line, "([\\w\\s,]*)from\\s*" + table +
											"\\s+where\\s*" + query + "\\s*$");

	if (!order.empty()) query = runRegex(query, "(.*?)" + order + "$");
	
	table_row cols = getSelectedColumns(select);

	if (!cols.empty()) {
		bool found = false;
		for (const std::string& col : cols)
			if (col == byWhat) found = true;

		if (!found) return false;
	}

	db->select(table, query, cols, distinct, byWhat, asc);
}

bool ConsoleHandler::removeHelper()
{
	std::string line;
	std::getline(std::cin, line);
	tolower(line);

	std::regex reg_table("^[ ]*from[ ]+(\\w+)[ ]+where[ ]+[\\w\\s><!=]+");
	std::regex reg_query("^[ ]*from[ ]+[\\w]+[ ]+where[ ]+([\\w\\s><!=]+)");

	std::string table = findMatch(line, reg_table);
	std::string query = findMatch(line, reg_query);
	
	if (table == "" || query == "") return false;
	db->removeQuery(table, query);

	return true;
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
		
		rows.push_back(Record(cur_row));
		cur_row.clear();

		std::cin >> c;				
	}

	return true;
}

std::string ConsoleHandler::runRegex(const std::string& line, const std::string& expr)
{
	std::regex reg(expr, std::regex_constants::icase);
	return findMatch(line, reg);
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

std::string ConsoleHandler::findMatch(const std::string& str, const std::regex& reg) {
	std::smatch match;

	if (std::regex_search(str, match, reg)) 
		return match[1];

	return "";
}

std::vector<std::string> ConsoleHandler::findMatches(std::string& str, std::regex& reg)
{
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