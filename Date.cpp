#include "Date.hpp"

/**
 * @param The constructor accepts a string in the format dd/mm/yyyy.
 * Zeroes for the single digit numbers are accepted, but not mandatory.
 * @throw An exception is thrown if the string passed has no valid convertions to
 * date::year_month_day, or if the date itself is invalid i.e. 30/02/2021
 */
Date::Date(const std::string& date)
{
	std::string y = findMatch(date, "\\d{1,2}/\\d{1,2}/(\\d+)");
	std::string m = findMatch(date, "\\d{1,2}/(\\d{1,2})/\\d+");
	std::string d = findMatch(date, "(\\d{1,2})/\\d{1,2}/\\d+");

	if (y.empty() || m.empty() || d.empty()) 
		throw std::exception("Wrong date format included!");
	
	year	= std::stoi(y);
	month	= std::stoi(m);
	day		= std::stoi(d);

	if (!object().ok()) throw std::exception("Invalid date!");
}

bool Date::operator==(const Date& other)
{
	return this->object() == other.object();
}

bool Date::operator!=(const Date& other)
{
	return this->object() != other.object();
}

bool Date::operator<(const Date& other)
{
	return this->object() < other.object();
}

bool Date::operator>(const Date& other)
{
	return this->object() > other.object();
}

bool Date::operator<=(const Date& other)
{
	return this->object() <= other.object();
}

bool Date::operator>=(const Date& other)
{
	return this->object() >= other.object();
}

const date::year_month_day Date::object() const
{
	return  date::year{ year } / month / day;
}

