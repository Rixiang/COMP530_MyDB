#ifndef LRC_C
#define LRC_C
#include "MyDB_LRU.h"

int MyDB_LRU :: addToTail(string pageID) {
	LRU_Node newTail = new LRU_Node(++incrementalLruNum, pageID);
	lruTable.push_back(newTail);
	return incrementalLruNum;
}

int MyDB_LRU :: moveToTail(int LRU_number) {
	std::list<LRU_Node>::iterator it;
	for (it = lruTable.begin(); it != lruTable.end(); it++) {
		if(*it->lruNum == LRU_number) {
			*it->lruNum = ++incrementalLruNum;
			lruTable.splice( lruTable.end(), lruTable, it );
			break;
		}
	}	
	return incrementalLruNum;
}

string MyDB_LRU :: evict() {
	string pageID = lruTable.front()->pageID;
	lruTable.pop_front();
	return pageID;
}

MyDB_LRU :: MyDB_LRU {
	this->lruTable = new std::list<LRU_Node>;
	this->incrementalLruNum = -1;
}

MyDB_LRU :: ~MyDB_LRU {
	~lruTable();
}

#endif
