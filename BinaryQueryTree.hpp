#pragma once
#include <string>
#include <list>

#include "Table.hpp"
#include "helper.hpp"

class Record;

class BinaryQueryTree {
private:
	struct Node {
		std::string type; //could be done with bitset

		Node* left;
		Node* right;

		Statement* statement;

		Node(std::string _type, Statement* _statement = nullptr, 
			Node* _left = nullptr, Node* _right = nullptr)
			:type(_type), statement(_statement), left(_left), right(_right) {}
	};

	Table* table;
	Node*  root;

public:
	const std::string AND = "AND";
	const std::string OR = "OR";

	BinaryQueryTree(Table* table_ptr);
	BinaryQueryTree(const BinaryQueryTree& other) = delete;
	const BinaryQueryTree& operator=(const BinaryQueryTree& other) = delete;
	~BinaryQueryTree();

	void				init(const std::string& query);
	std::list<Record>	search();

private:
	void				removeRecursive(Node*& root);
	void				initRecursive(Node*& root, const std::string& query);
	std::list<Record>	searchRecursive(Node* root);

	const Statement		createStatement(const std::string& query);
};

inline BinaryQueryTree::BinaryQueryTree(Table* table_ptr)
	:table(table_ptr), root(nullptr) {}

inline BinaryQueryTree::~BinaryQueryTree()
{
	removeRecursive(root);
}

inline void BinaryQueryTree::init(const std::string& query)
{
	if (!query.empty()) initRecursive(root, query);
}

inline std::list<Record> BinaryQueryTree::search() 
{
	if (!root) return table->table();

	try {
		return searchRecursive(root);
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}
}

inline void BinaryQueryTree::removeRecursive(Node*& root)
{
	if (root) {
		if (root->left) removeRecursive(root->left);
		else if (root->right) removeRecursive(root->right);
		else delete root->statement;
	}

	root = nullptr;
}

inline void BinaryQueryTree::initRecursive(Node*& root, const std::string& query)
{
	std::string leftSub, rightSub;
	if (findMatch(query, "\\b(" + AND +")\\b") != "") {
		root = new Node(AND, nullptr);

		leftSub = findMatch(query, "(.*?)\\b" + AND +"\\b");
		rightSub = findMatch(query, "^.*?\\b" + AND + "\\b(.*?)$");
	}
	
	else if (findMatch(query, "\\b(" + OR + ")\\b") != "") {
		root = new Node(OR, nullptr);

		leftSub = findMatch(query, "(.*?)\\b" + OR + "\\b");
		rightSub = findMatch(query, "^.*?\\b" + OR + "\\b(.*?)$");
	}

	if (root) {
		initRecursive(root->left, leftSub);
		initRecursive(root->right, rightSub);
	}
	else {
		Statement st = createStatement(query);
		root = new Node("statement", new Statement(st));
	}
}

inline std::list<Record> BinaryQueryTree::searchRecursive(Node* root)
{
	if (root->type == "statement")
		return table->search(*root->statement);

	std::list<Record> lhs = searchRecursive(root->left);
	std::list<Record> rhs = searchRecursive(root->right);

	if (root->type == AND) {
		intersect(lhs, rhs);
		return std::list<Record>(lhs);
	}
	else if (root->type == OR) {
		unify(lhs, rhs);
		return std::list<Record>(lhs);
	}
}

inline const Statement BinaryQueryTree::createStatement(const std::string& query)
{
	Statement st;
	std::regex rem_space("([\\s]+)");
	std::string newQuery = std::regex_replace(query, rem_space, "");

	st.lhs	= findMatch(newQuery, "^\\s*([\\w/]+)");
	st.op	= findMatch(newQuery, "^\\s*" + st.lhs + "\\s*([!<>=]+)");
	st.rhs	= findMatch(newQuery, "^\\s*" + st.lhs + "\\s*" + st.op + "\\s*([\\w/]+)\\s*$");

	return st;
}



