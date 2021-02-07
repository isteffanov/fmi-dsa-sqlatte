#pragma once
#include <fstream>
#include <list>
#include <string>

#include "BinaryQueryTree.hpp"
#include "Table.hpp"
#include "helper.hpp"

/**
 * @brief This class has all the responsibility for 
 * managing the files and successfully executing 
 * the user's commands.
 * @details All the files for the database are being saved in
 *  database/ folder, which the user has to create themselves.
 */
class DataBase
{
	static const std::string DB_RELATIVE_PATH;
	static const std::string DB_FILE;

	std::ifstream in;
	std::ofstream out;

	static const int MAX_ROWS_RETURN = 5;
public:
	DataBase();
	DataBase(const DataBase&) = delete;
	const DataBase& operator=(const DataBase&) = delete;
	~DataBase();

	void list();
	void info(const std::string& name);

	bool create(const std::string& name, const table_row& names, const table_row& types);
	bool drop(const std::string& table);
	bool insert(const std::string& table, const list_record& rec);
	void remove(const std::string& table, const std::string& query);
	void select(const std::string& table, const std::string query, const table_row cols,
				bool distinct, const std::string& orderBy, bool asc);

private:
	uint64_t		size(const std::string& name) const;		//size in bytes of the file
	size_t			count(const std::string& name) const;		//number of records in the file
	void			removeRecords(const std::string& table, list_record& found);
	void			selectAll(const std::string& name, const std::string& query,
							bool distinct, const std::string& orderBy, bool asc);
	void			selectSome(const std::string& name, const table_row& cols,
							const std::string& query, bool distinct, const std::string& orderBy, bool asc);

	list_tableptr	tables(); //read all the names and schemas from the DB file
	Table*			initTable(const std::string& tableName); //get empty table with its name and schema

	void			save();	
	void			open();
	void			close();
	void			reset();	//move the file pointers to the beginning 

	void			removeDuplicates(list_record& found);
	void			printSelected(const list_record& found);
	void			printSelected(const list_record& found, const std::vector<bool>& cols);

	std::ifstream	getStreamIN(const std::string& table) const;
	std::ofstream	getStreamOUT(const std::string& table) const;
	std::string		getTablePath(const std::string& table) const;
};

