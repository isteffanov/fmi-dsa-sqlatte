#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

#include "helper.hpp"

struct type_name_pair {
	static const std::string INT;
	static const std::string STRING;
	static const std::string DATE;
	static const std::string UNKNOWN;
private:
	static enum class TYPES {
		INT = 0,
		DATE,
		STRING,

		INVALID = -1
	};

	std::string f_name;
	TYPES f_type;

public:
	type_name_pair(std::string _name = "", std::string _type = "")
		:f_name(_name)
	{
		if (_type == INT) f_type = TYPES::INT;
		else if (_type == DATE)	f_type = TYPES::DATE;
		else if (_type == STRING) f_type = TYPES::STRING;
		else f_type = TYPES::INVALID;
	}
	
	const std::string& name() const { return f_name; }
	const std::string& type() const {
		switch (f_type)
		{
		case type_name_pair::TYPES::INT:
			return INT;
		case type_name_pair::TYPES::DATE:
			return DATE;
		case type_name_pair::TYPES::STRING:
			return STRING;
		default:
			return UNKNOWN;
		}
	}
};

class Schema {
	std::vector<type_name_pair> f_schema;

public:
	Schema();
	Schema(std::vector<type_name_pair> _schema);
	Schema(table_row names, table_row types);

 	const type_name_pair& operator[](size_t pos) const;

	const std::vector<type_name_pair>&  schema() const;
	
	const size_t				size() const;
	const size_t				pos(const std::string& name) const;
	const bool					date(const std::string& name) const;		//does the column store dates
	const std::string&			type(const std::string& name) const;		
	const std::vector<bool>		columns(const table_row& names) const;		//which columns from the schema are needed

	void						print() const;
	void						print(const std::vector<bool>& cols) const;

	friend std::ifstream& operator>>(std::ifstream& in, Schema& schema);
	friend std::ofstream& operator<<(std::ofstream& out, const Schema& schema);

};