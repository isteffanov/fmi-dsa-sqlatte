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

		if (command == "create_table") create_table();
		else if (command == "drop_table") drop_table();
		else if (command == "list_tables") list_tables();
		else if (command == "table_info") table_info();
		//else if (command == "select") select();
		//else if (command == "insert") insert();
		//else if (command == "remove") remove();

	} while (command != "quit");
}

void ConsoleHandler::create_table()
{
	try {
		create_table_wrap();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
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

void ConsoleHandler::list_tables()
{
	db->list();
}

void ConsoleHandler::table_info()
{
	std::string table;
	std::cin >> table;

	db->info(table);
}

void ConsoleHandler::create_table_wrap()
{
	std::string name;
	std::vector<std::string> names, types;

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
		//else throw std::exception("Wrong input format!");

		if (c == ':') {

			if (!type) {
				names.push_back(cur);
				type = !type;

				cur.erase();
			}
			else throw std::exception("Wrong input format!");
		}
		else if (c == ',' ) {

			if (type) {
				types.push_back(cur);
				type = !type;

				cur.erase();
			}
			else throw std::exception("Wrong input format!");
			
		}
		else cur.insert(cur.end(), c);
	}

	db->create(name, names, types);
}

void ConsoleHandler::tolower(std::string& word)
{
	std::transform(word.begin(), word.end(), word.begin(),
		[](unsigned char c) { return std::tolower(c); });

}

