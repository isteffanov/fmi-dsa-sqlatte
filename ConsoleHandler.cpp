#include "ConsoleHandler.hpp"

ConsoleHandler::ConsoleHandler()
{
	file.open("db.txt");
	db = new DataBase();
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
		else if (command == "remove_from")	remove();
		else if (command == "select_from") 	select();

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
	if (!insertIntoHelper()) {
		//TODO
	}
}

void ConsoleHandler::remove()
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
}

void ConsoleHandler::select()
{
	std::string table, where;
	std::cin >> table;

	std::cin >> where;
	if (where == "where") {

		std::string query;
		std::getline(std::cin, query);
		tolower(query);

		db->select(table, query);
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

bool ConsoleHandler::insertIntoHelper()
{
	std::string table;
	std::cin >> table;

	char c;

	std::cin >> c;
	if (c != '{') return false;

}

void ConsoleHandler::tolower(std::string& word)
{
	std::transform(word.begin(), word.end(), word.begin(),
		[](unsigned char c) { return std::tolower(c); });
}