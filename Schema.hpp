#pragma once
#include <fstream>
#include <string>
#include <vector>

#include "helper.hpp"

struct type_name_pair {
private:
	static const std::string INT;
	static const std::string STRING;
	static const std::string DATE;
	static const std::string UNKNOWN;

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
		switch (f_type)
		{
		case type_name_pair::TYPES::INT:
			f_type = TYPES::INT;
			break;
		case type_name_pair::TYPES::DATE:
			f_type = TYPES::DATE;
			break;
		case type_name_pair::TYPES::STRING:
			f_type = TYPES::STRING;
			break;
		default:
			f_type = TYPES::INVALID;
		}
	}

	const type_name_pair& operator=(const type_name_pair& other);

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
	Schema(std::vector<std::string> names, std::vector<std::string> types);

 	const type_name_pair& operator[](size_t pos) const;

	const size_t						size() const;
	const size_t						pos(const std::string& name) const;
	const bool							date(const std::string& name) const;
	const std::string&					type(const std::string& name) const;
	const std::vector<bool>				columns(const std::vector<std::string>& names) const;
	const std::vector<type_name_pair>&  schema() const;


	friend std::ifstream& operator>>(std::ifstream& in, Schema& schema);
	friend std::ofstream& operator<<(std::ofstream& out, const Schema& schema);

};