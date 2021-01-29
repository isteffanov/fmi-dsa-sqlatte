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


