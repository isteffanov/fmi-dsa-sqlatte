#pragma once
#include <string>
#include <forward_list>

#include "Table.hpp"
#include "helper.hpp"

class Record;

class BinaryQueryTree {

	const std::string AND = "AND";
	const std::string OR = "OR";

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
	BinaryQueryTree(Table* table_ptr, const std::string& query);
	BinaryQueryTree(const BinaryQueryTree& other) = delete;
	const BinaryQueryTree& operator=(const BinaryQueryTree& other) = delete;
	~BinaryQueryTree();

	list_record	search();

private:
	void		init(const std::string& query); 
	void		removeRecursive(Node*& root);
	void		initRecursive(Node*& root, const std::string& query);
	list_record	searchRecursive(Node* root);

	const Statement	createStatement(const std::string& query);
};

inline BinaryQueryTree::BinaryQueryTree(Table* table_ptr, const std::string& query)
	:table(table_ptr), root(nullptr)
{
	init(query);
}

inline BinaryQueryTree::~BinaryQueryTree()
{
	removeRecursive(root);
}

inline void BinaryQueryTree::init(const std::string& query)
{
	if (!query.empty()) initRecursive(root, query);
}

inline list_record BinaryQueryTree::search() 
{
	if (!root) return table->table();

	return searchRecursive(root);
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
	//the leafs represent simple statements
	//every other node is either 'and' or 'or'
	//and its value is return is dependent on
	//its children

	//if one of 'and' or 'or' is found
	//the recursion continues building
	//until a simple statement is found

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

inline list_record BinaryQueryTree::searchRecursive(Node* root)
{
	//traversing the tree [root - left - right]
	//building the return of the query
	//from the bottom up, crossing or unifying
	//the list depending on the operator

	if (root->type == "statement")
		return table->search(*root->statement);

	list_record lhs = searchRecursive(root->left);
	list_record rhs = searchRecursive(root->right);

	if (root->type == AND) 
		return intersect(lhs, rhs);
	
	else if (root->type == OR) 
		return unify(lhs, rhs);
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



