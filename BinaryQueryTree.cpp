#include "BinaryQueryTree.hpp"

BinaryQueryTree::BinaryQueryTree(Table* table_ptr)
	:table(table_ptr), root(nullptr) {}

BinaryQueryTree::~BinaryQueryTree()
{
	remove(root);
}

void BinaryQueryTree::init(const std::string& query)
{
	initRecursive(root, query);
}

std::list <Record> BinaryQueryTree::search() const
{
	return searchRecursive(root);
}


void BinaryQueryTree::remove(Node*& root)
{
	if (root->left) remove(root->left);
	else if (root->right) remove(root->right);
	else delete root->statement;

	root = nullptr;
}

void BinaryQueryTree::initRecursive(Node*& root, const std::string& query)
{
	size_t pos;

	pos = query.find("and");
	if (pos != std::string::npos) {
		root = new Node("and", nullptr);
	}
	else {

		pos = query.find("or");
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

std::list<Record> BinaryQueryTree::searchRecursive(Node* root) const
{
	if (root->type == "statement")
		return table->search(root->statement->lhs, root->statement->operation, root->statement->rhs);

	std::list<Record> lhs = searchRecursive(root->left);
	std::list<Record> rhs = searchRecursive(root->right);

	if (root->type == "and") return findCrosssection(lhs, rhs);
	else if (root->type == "or") return findUnion(lhs, rhs);
	else throw std::exception("opa");
}

void BinaryQueryTree::createStatement(std::string& lhs, std::string& operation, std::string& rhs, const std::string& query)
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

std::list<Record> BinaryQueryTree::findCrosssection(std::list<Record> lhs, std::list<Record> rhs) const
{
	std::list<Record> rtrn;

	for (const Record& curLHS : lhs)
		for (const Record& curRHS : rhs)
			if (curLHS == curRHS) rtrn.push_front(curLHS);


	return rtrn;
}

std::list<Record> BinaryQueryTree::findUnion(std::list<Record> lhs, std::list<Record> rhs) const
{
	std::list<Record> rtrn = lhs;

	for (const Record& curRHS : rhs)
		if (!isPresent(rtrn, curRHS)) rtrn.push_front(curRHS);

	return rtrn;
}

bool BinaryQueryTree::isPresent(std::list<Record> list, Record cmp) const
{
	for (const Record& entry : list)
		if (entry == cmp) return true;

	return false;
}

bool BinaryQueryTree::isWhitespaceOrQuote(char c) const
{
	return c == ' ' || c == '\'' || c == '"';
}

bool BinaryQueryTree::isLetterOrNumber(char c) const
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool BinaryQueryTree::isOperator(char c)
{
	return c == '<' || c == '>' || c == '!' || c == '=';
}