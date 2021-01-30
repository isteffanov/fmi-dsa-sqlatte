#pragma once
#include <fstream>
#include <string>
#include <vector>

#include "read_write_helpers.hpp"

struct type_name_pair {
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
		if (_type == "int") f_type = TYPES::INT;
		else if (_type == "date") f_type = TYPES::DATE;
		else if (_type == "string") f_type = TYPES::STRING;
		else f_type = TYPES::INVALID;
	}

	const std::string& name() const { return f_name; }
	const std::string& type() const {
		if (f_type == TYPES::INT)			return "int";
		else if (f_type == TYPES::DATE)		return "date";
		else if (f_type == TYPES::STRING)	return "string";
		else								return "unknown";
	}

};

class Schema {
	std::vector<type_name_pair> f_schema;

public:
	Schema();
	Schema(std::vector<type_name_pair> _schema);
	Schema(std::vector<std::string> names, std::vector<std::string> types);


	const size_t						size() const;
	const size_t						pos(std::string name) const;
	const std::string&					find_type(std::string col) const;
	const std::vector<type_name_pair>&  schema() const;

 	const type_name_pair& operator[](size_t pos) const;

	friend std::fstream& operator>>(std::fstream& in, Schema& schema);
	friend std::fstream& operator<<(std::fstream& out, const Schema& schema);

};