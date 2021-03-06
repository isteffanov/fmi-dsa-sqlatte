/**
 * @brief The class to handle the date logic in the project.
 * @details It is based on the library date.h: [https://github.com/HowardHinnant/date].
 * It uses the library's date::year_month_day type to handle all the comparisons and validation.
 */

#pragma once
#include <fstream>
#include "helper.hpp"
#include "date/date.h"

class Date {
	int day;
	int month;
	int year;

public:
	Date(const std::string& date = "1/1/1970");

	bool operator==(const Date& other);
	bool operator!=(const Date& other);
	bool operator<(const Date& other);
	bool operator>(const Date& other);
	bool operator<=(const Date& other);
	bool operator>=(const Date& other);

private:
	const date::year_month_day object() const; //used for comparison
};


