#include "helper.hpp"

std::string findMatch(const std::string& str, const std::string& expr)
{
	std::regex reg(expr, std::regex::ECMAScript | std::regex::icase);
	std::smatch match;

	if (std::regex_search(str, match, reg))
		return match.str(1);

	return "";
}

std::vector<std::string> findMatches(const std::string& str, const std::string& expr)
{
	std::regex reg(expr, std::regex::icase);
	std::vector<std::string> rtrn;

	std::smatch match;
	std::sregex_iterator it(str.begin(), str.end(), reg);
	std::sregex_iterator end;

	while (it != end) {
		match = *it;
		if(match.str() != " ") rtrn.push_back(match.str());
		it++;
	}

	return rtrn;
}

//write string to file
std::ofstream& write_string(std::ofstream& out, const std::string& str) {

	size_t len = str.size();
	out.write((const char*)&len, sizeof(size_t));
	out.write(str.c_str(), len);

	return out;
}

std::fstream& write_string(std::fstream& out, const std::string& str) {

	size_t len = str.size();
	out.write((const char*)&len, sizeof(size_t));
	out.write(str.c_str(), len);

	return out;
}

// read string from file
std::ifstream& read_string(std::ifstream& in, std::string& str) {
	size_t len;
	in.read((char*)&len, sizeof(size_t));

	try {
		char* ptr = new char[len + 1];
		in.read(ptr, len);
		ptr[len] = '\0';
		str = std::string(ptr);

		delete[] ptr;
	}
	catch (...) {
		return in; //bad bit risen
	}

	return in;
}

std::fstream& read_string(std::fstream& in, std::string& str) {
	size_t len;
	in.read((char*)&len, sizeof(size_t));

	try {
		char* ptr = new char[len + 1];
		in.read(ptr, len);
		ptr[len] = '\0';
		str = std::string(ptr);

		delete[] ptr;
	}
	catch (...) {
		return in; //bad bit risen
	}

	return in;
}

void write_vector(std::ofstream& out, const std::vector<std::string>& vec)
{
	size_t size = vec.size();
	out.write((const char*)&size, sizeof(size_t));

	for (int i = 0; i < vec.size(); ++i)
		write_string(out, vec[i]);
}

//write vector of strings to file
void write_vector(std::fstream& out, const std::vector<std::string>& vec) {
	size_t size = vec.size();
	out.write((const char*)&size, sizeof(size_t));

	for (int i = 0; i < vec.size(); ++i)
		write_string(out, vec[i]);

}

void read_vector(std::ifstream& in, std::vector<std::string>& vec)
{
	if (in.good()) {
		size_t size;
		in.read((char*)&size, sizeof(size_t));

		std::string cur;
		for (int i = 0; i < size; ++i) {
			if (!read_string(in, cur)) break;
			vec.push_back(cur);
		}
	}
}

//read vector of strings from file
void read_vector(std::fstream& in, std::vector<std::string>& vec) {

	if (in.good()) {
		size_t size;
		in.read((char*)&size, sizeof(size_t));

		std::string cur;
		for (int i = 0; i < size; ++i) {
			read_string(in, cur);
			vec.push_back(cur);
		}
	}
}