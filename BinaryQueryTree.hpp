#pragma once
#include <string>
#include <list>

#include "Record.hpp"
#include "Table.hpp"

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

	Table* table;
	Node*  root;

public:
	BinaryQueryTree(Table* table_ptr);
	BinaryQueryTree(const BinaryQueryTree& other) = delete;
	const BinaryQueryTree& operator=(const BinaryQueryTree& other) = delete;
	~BinaryQueryTree();

	void init(const std::string& query);
	std::list<Record> search() const;

private:

	void remove(Node*& root);

	void				initRecursive(Node*& root, const std::string& query);
	std::list<Record>	searchRecursive(Node* root) const;

	void				createStatement(std::string& lhs, std::string& operation, std::string& rhs, const std::string& query);

	std::list<Record>	findCrosssection(std::list<Record> lhs, std::list<Record> rhs) const;
	std::list<Record>	findUnion(std::list<Record> lhs, std::list<Record> rhs) const;

	bool				isPresent(std::list<Record> list, Record cmp) const;
	bool				isWhitespaceOrQuote(char c) const;
	bool				isLetterOrNumber(char c) const;
	bool				isOperator(char c);
};

inline BinaryQueryTree::BinaryQueryTree(Table* table_ptr)
	:table(table_ptr), root(nullptr) {}

inline BinaryQueryTree::~BinaryQueryTree()
{
	remove(root);
}

inline void BinaryQueryTree::init(const std::string& query)
{
	initRecursive(root, query);
}

inline std::list <Record> BinaryQueryTree::search() const
{
	return searchRecursive(root);
}


inline void BinaryQueryTree::remove(Node*& root)
{
	if (root->left) remove(root->left);
	else if (root->right) remove(root->right);
	else delete root->statement;

	root = nullptr;
}

inline void BinaryQueryTree::initRecursive(Node*& root, const std::string& query)
{
	size_t pos;

	pos = query.find(" and ");
	if (pos != std::string::npos) {
		root = new Node("and", nullptr);
	}
	else {

		pos = query.find(" or ");
		if (pos != std::string::npos) {
			root = new Node("or", nullptr);
		}
	}

	if (root) {
		initRecursive(root->left, query.substr(0, pos));
		initRecursive(root->right, query.substr(pos + root->type.size()));
	}
	else {
		std::string lhs;
		std::string operation;
		std::string rhs;

		createStatement(lhs, operation, rhs, query);

		root = new Node("statement", new Statement(lhs, operation, rhs));
	}
}

inline std::list<Record> BinaryQueryTree::searchRecursive(Node* root) const
{
	if (root->type == "statement")
		return table->search(root->statement->lhs, root->statement->operation, root->statement->rhs);

	std::list<Record> lhs = searchRecursive(root->left);
	std::list<Record> rhs = searchRecursive(root->right);

	if (root->type == "and") return findCrosssection(lhs, rhs);
	else if (root->type == "or") return findUnion(lhs, rhs);
	else throw std::exception("opa");
}

inline void BinaryQueryTree::createStatement(std::string& lhs, std::string& operation, std::string& rhs, const std::string& query)
{
	size_t cnt = 0;
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
	}
}

inline std::list<Record> BinaryQueryTree::findCrosssection(std::list<Record> lhs, std::list<Record> rhs) const
{
	std::list<Record> rtrn;

	for (const Record& curLHS : lhs)
		for (const Record& curRHS : rhs)
			if (curLHS == curRHS) rtrn.push_front(curLHS);


	return rtrn;
}

inline std::list<Record> BinaryQueryTree::findUnion(std::list<Record> lhs, std::list<Record> rhs) const
{
	std::list<Record> rtrn = lhs;

	for (const Record& curRHS : rhs)
		if (!isPresent(rtrn, curRHS)) rtrn.push_front(curRHS);

	return rtrn;
}

inline bool BinaryQueryTree::isPresent(std::list<Record> list, Record cmp) const
{
	for (const Record& entry : list)
		if (entry == cmp) return true;

	return false;
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
