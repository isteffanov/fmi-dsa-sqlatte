#pragma once
#include <fstream>
#include "helper.hpp"
#include "date/date.h"

class Date {
	int day;
	int month;
	int year;

public:
	Date(const std::string& date);

	bool operator==(const Date& other);
	bool operator!=(const Date& other);
	bool operator<(const Date& other);
	bool operator>(const Date& other);
	bool operator<=(const Date& other);
	bool operator>=(const Date& other);

private:
	const date::year_month_day object() const;
};


