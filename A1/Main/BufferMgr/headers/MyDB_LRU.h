
#ifndef LRU_H
#define LRU_H
#include <list>
#include <string>

class LRU_Node {

private:
	int lruNum;
	string pageID;

public:
	
}

class MyDB_LRU {

private:
	list<LRU_Node> lruTable;
	static int incrementalLruNum;

public:
	int addToTail(string pageID);
	int moveToTail(int LRU_number);
	string evict();
	MyDB_LRU();
	~MyDB_LRU();
}
#endif
