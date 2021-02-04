#pragma once
#include<list>
#include<vector>
#include<string>

class Record;
class Table;

typedef  std::list<Table*>::iterator list_tableptr_it;
typedef  std::list<Record>::iterator list_record_it;
using table_row = std::vector<std::string>;