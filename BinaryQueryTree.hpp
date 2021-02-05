#pragma once
#include <string>
#include <list>

#include "Record.hpp"
#include "Table.hpp"
#include "helper.hpp"

class BinaryQueryTree {

private:

	struct Statement {
		std::string lhs;
		std::string rhs;

		std::string operation;

		Statement(std::string _lhs, std::string _operation, std::string _rhs)
			:lhs(_lhs), operation(_operation), rhs(_rhs) {}
	};

	struct Node {
		std::string type;

		Node* left;
		Node* right;

		Statement* statement;

		Node(std::string _type, Statement* _statement = nullptr, Node* _left = nullptr, Node* _right = nullptr)
			:type(_type), statement(_statement), left(_left), right(_right) {}
	};

	bool all;
	Table* table;
	Node*  root;

public:
	BinaryQueryTree(Table* table_ptr);
	BinaryQueryTree(const BinaryQueryTree& other) = delete;
	const BinaryQueryTree& operator=(const BinaryQueryTree& other) = delete;
	~BinaryQueryTree();

	void init(const std::string& query);
	std::list<Record> search();

private:

	void remove(Node*& root);

	void				initRecursive(Node*& root, const std::string& query);
	std::list<Record>	searchRecursive(Node* root);

	void				createStatement(std::string& lhs, std::string& operation, std::string& rhs, const std::string& query);

	bool				isWhitespaceOrQuote(char c) const;
	bool				isLetterOrNumber(char c) const;
	bool				isOperator(char c);

};

inline BinaryQueryTree::BinaryQueryTree(Table* table_ptr)
	:table(table_ptr), root(nullptr), all(false) {}

inline BinaryQueryTree::~BinaryQueryTree()
{
	remove(root);
}

inline void BinaryQueryTree::init(const std::string& query)
{
	if (query.empty()) all = true;
	else initRecursive(root, query);
}

inline std::list<Record> BinaryQueryTree::search() 
{
	if (all) return table->table();

	try {
		return searchRecursive(root);
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}
}


inline void BinaryQueryTree::remove(Node*& root)
{
	if (root) {
		if (root->left) remove(root->left);
		else if (root->right) remove(root->right);
		else delete root->statement;
	}

	root = nullptr;
}

inline void BinaryQueryTree::initRecursive(Node*& root, const std::string& query)
{
	std::string leftSub, rightSub;
	if (findMatch(query, "\\b(and)\\b") != "") {
		root = new Node("and", nullptr);
		leftSub = findMatch(query, "(.*?)\\band\\b");
		rightSub = findMatch(query, "^.*?\\band\\b(.*?)$");
	}
	
	else if (findMatch(query, "\\b(or)\\b") != "") {
		root = new Node("or", nullptr);
		leftSub = findMatch(query, "(.*?)\\bor\\b");
		rightSub = findMatch(query, "^.*?\\bor\\b(.*?)$");
	}

	if (root) {
		initRecursive(root->left, leftSub);
		initRecursive(root->right, rightSub);
	}
	else {
		std::string lhs;
		std::string operation;
		std::string rhs;

		createStatement(lhs, operation, rhs, query);

		root = new Node("statement", new Statement(lhs, operation, rhs));
	}
}

inline std::list<Record> BinaryQueryTree::searchRecursive(Node* root)
{
	if (root->type == "statement")
		return table->search(root->statement->lhs, root->statement->operation, root->statement->rhs);

	std::list<Record> lhs = searchRecursive(root->left);
	std::list<Record> rhs = searchRecursive(root->right);

	if (root->type == "and") {
		intersect(lhs, rhs);
		return std::list<Record>(lhs);
	}
	else if (root->type == "or") {
		unify(lhs, rhs);
		return std::list<Record>(lhs);
	}
	else throw std::exception("Something went wrong!");
}

inline void BinaryQueryTree::createStatement(std::string& lhs, std::string& op, std::string& rhs, const std::string& query)
{
	std::regex rem_space("([\\s]+)");
	std::string newQuery = std::regex_replace(query, rem_space, "");

	lhs = findMatch(newQuery, "^\\s*([\\w/]+)");
	op	= findMatch(newQuery, "^\\s*" + lhs + "\\s*([!<>=]+)");
	rhs = findMatch(newQuery, "^\\s*" + lhs + "\\s*" + op + "\\s*([\\w/]+)\\s*$");
	
	/*size_t cnt = 0;
	char cur = query[cnt];

	while (isWhitespaceOrQuote(cur))
		cur = query[++cnt];

	while (isLetterOrNumber(cur)) {
		lhs.push_back(cur);
		cur = query[++cnt];
	}

	while (isWhitespaceOrQuote(cur))
		cur = query[++cnt];

	while (isOperator(cur)) {
		operation.push_back(cur);
		cur = query[++cnt];
	}

	while (isWhitespaceOrQuote(cur))
		cur = query[++cnt];

	while (isLetterOrNumber(cur)) {
		rhs.push_back(cur);
		cur = query[++cnt];
	}*/
}

inline bool BinaryQueryTree::isWhitespaceOrQuote(char c) const
{
	return c == ' ' || c == '\'' || c == '"';
}

inline bool BinaryQueryTree::isLetterOrNumber(char c) const
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline bool BinaryQueryTree::isOperator(char c)
{
	return c == '<' || c == '>' || c == '!' || c == '=';
}


