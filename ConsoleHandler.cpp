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
	std::string command;
	do {
		std::cout << "SQLatte> ";
		std::cin >> command;

		tolower(command);

		if      (command == "createtable")	create_table();
		else if (command == "droptable")	drop_table();
		else if (command == "listtables")	list_tables();
		else if (command == "tableinfo")	table_info();
		else if (command == "insert")	insert();
		else if (command == "remove")	remove();
		else if (command == "select") 	select();

	} while (command != "quit");

	std::cout << "Goodbye, Master! :(";
}

/**
 * @details The accepted format is: CREATETABLE <table name> (<name1>:<type1>, <name2>:<type2>, ...)
 */
void ConsoleHandler::create_table()
{
	if (!createTableHelper()) {
		std::cerr << "Wrong input format!" << std::endl;
		std::cin.clear();
	}
}

/**
 * @details The accepted format is: DROPTABLE <table name>.
 * @note Prompts the user for confirmation.
 */

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

/**
 * @details The command does not require additional parameters
 */
void ConsoleHandler::list_tables() const
{ 
	db->list();
}

/**
 * @details The accepted format is: LISTTABLES <table name>.
 */
void ConsoleHandler::table_info() const
{
	std::string table;
	std::cin >> table;

	db->info(table);
}

/**
 * @details The accepted format is: INSERT INTO <table name>
 * {(entry1_param1, entry1_param2, ... ), (entry2_param1, entry2_param2, ...), ...}
 */
void ConsoleHandler::insert()
{
	if (!(insertIntoHelper())) 
		std::cerr << "Wrong format!" << std::endl;
}

/**
 * @details The accepted format is: REMOVE FROM <table name> WHERE <query>
 */
void ConsoleHandler::remove()
{
	if (!removeHelper()) {
		std::cerr << "Wrong input format!" << std::endl;
		std::cin.clear();
	}
}

/**
 * @details The accepted format is: SELECT [DISTINCT] <column_name1, column_name2, ...|*> 
 *  FROM <table name> [WHERE <query>] [ORDER BY <column_name> [asc|desc]]
 * @note '*' means all columns
 */
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

	//check if the input is valid
	std::string assertSchema = "(\\s*\\w+\\s*:\\s*\\w+\\s*(,|\\)\\s*$)";
	if (!assert_input(line, "(^\\s*\\w+\\s*\\(" + assertSchema + ")+)[^$]")) 
		return false;

	std::string name = findMatch(line, "^\\s*(\\w+)\\s*");
	if (name.empty()) return false;

	std::regex rem("(,)");
	line = std::regex_replace(line, rem, "");

	line = findMatch(line, "\\((.*?)\\)");

	table_row names = findMatches(line, "(\\w+(?=:))");
	table_row types = findMatches(line, ":\\s*(\\w+)");

	if (names.size() != types.size() || names.empty() || types.empty()) return false;

	std::regex rem3(":\\s*");
	for (std::string& type : types)
		type = std::regex_replace(type, rem3, "");  //escape ":" because the lib
													//does not support look behind

	db->create(name, names, types);
}

bool ConsoleHandler::insertIntoHelper()
{
	std::string line;
	std::getline(std::cin, line);

	std::string table = findMatch(line, "^\\s*into\\s+(\\w+)");
	line = findMatch(line, "\\{(.*?)\\}"); //escape {}
	list_record rows;

	table_row entries = findMatches(line, "\\(([\\w\\s/,]+)\\)");
	if (entries.empty()) return false;

	std::regex brack("(\\(|\\))");
	for (std::string& entry : entries) {
		entry = std::regex_replace(entry, brack, "");
		rows.push_front(findMatches(entry, "([\\w\\s/]+$|[\\w\\s/]+(?=,))\\s*"));
	}

	if (rows.empty()) return false;
	return db->insert(table, rows);
}

bool ConsoleHandler::removeHelper()
{
	std::string line;
	std::getline(std::cin, line);

	//check if the input is valid
	std::string assertWhere = "where(\\s+\\w+\\s*[!=<>]{1,2}\\s*[\\w/]+\\s*[$|(and|or)])+";
	if (!assert_input(line, "(^\\s*from\\s+\\w+\\s+" + assertWhere + "[^$])"))
		return false;

	std::string table = findMatch(line, "^[ ]*from[ ]+(\\w+)[ ]+where[ ]+[\\w\\s><!=/]+");
	std::string query = findMatch(line, "^[ ]*from[ ]+[\\w]+[ ]+where[ ]+([\\w\\s><!=/]+)");

	if (table == "" || query == "") return false;

	db->remove(table, query);
	return true;
}

bool ConsoleHandler::selectHelper()
{
	std::string line;
	std::getline(std::cin, line);
	
	//check if the input is valid
	std::string assertOrder = "(order\\s+by\\s+\\w+\\s*(asc|desc)?)?$";
	std::string assertWhere = "where(\\s+\\w+\\s*[!=<>]{1,2}\\s*[\\w/]+\\s+(and|or|" + assertOrder + "))+";
	std::string assertFrom = "from\\s+\\w+\\s+(" + assertWhere + ")";

	if (!assert_input(line, "(^\\s*(distinct)?\\s*(\\*|\\s*\\w+\\s*(,|" + assertFrom + "))[^$])"))
		return false;

	//break down the input
	std::string distinct	= findMatch(line, "^\\s*(distinct)");
	std::string table		= findMatch(line, "\\s*from\\s+(\\w+)");
	std::string query		= findMatch(line, "\\s*from\\s+" + table + "\\s+where([\\w\\s!=<>/]+)$");
	std::string order		= findMatch(line, "\\s*from\\s+" + table + ".*?(order\\s+by[\\s\\w]*)$");
	std::string byWhat		= findMatch(order, "\\s*order\\s+by\\s+(\\w+)\\s*(asc|desc)?$");
	std::string cend		= findMatch(order, "\\s*order\\s+by\\s+" + byWhat + "\\s+(asc|desc)$");


	bool isDistinct = (!distinct.empty());
	bool asc = (cend == "asc");

	std::string select;
	if (isDistinct)	select = findMatch(line, "^\\s*distinct([\*\\w\\s,]*)from\\s*" + table +
											"(\\s+where\\s*" + query + ")?\\s*$");
	else			select = findMatch(line, "^\\s*([\*\\w\\s,]*)from\\s*" + table +
											"(\\s+where\\s*" + query + ")?\\s*");

	if (!order.empty()) query = findMatch(query, "\\s*(.*?)" + order + "$");
	
	//get selected columns, if * it will be empty
	table_row cols = findMatches(select, "\\w+");

	//assert that if there is a column to sort by
	//it is selected 
	if (!cols.empty() && !byWhat.empty()) {
		bool found = false;
		for (const std::string& col : cols)
			if (col == byWhat) found = true;

		if (!found) {
			std::cout << "Cannot order by column which is not selected! ";
			return false;
		}
	}

	db->select(table, query, cols, isDistinct, byWhat, asc);
}

/**
 * @param line is a string to be validated
 * @param expr is a regex string, used to validation
 */

bool ConsoleHandler::assert_input(const std::string& line, const std::string& expr)
{
	return ( findMatch(line, expr) != "");
}

void ConsoleHandler::tolower(std::string& word)
{
	std::transform(word.begin(), word.end(), word.begin(),
		[](unsigned char c) { return std::tolower(c); });
}
