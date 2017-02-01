#ifndef LRU_H
#define LRU_H

#include <list>
#include <string>

class LRU_NodeBase;

typedef shared_ptr <LRU_NodeBase> LRU_Node;
/*
LRU Node includes a LRU number and a page ID.
*/
class LRU_NodeBase {

private:
	int lruNum;
	string pageId;

public:
	LRU_NodeBase(int lruNum, string pageId){
		this->lruNum = lruNum;
		this->pageId = pageId;
	}
	
	~LRU_NodeBase(){}
};

/*
Implementation of LRU.
*/
class MyDB_LRU {

private:
	
	//A list of LRU Node.
	list<LRU_Node> lruTable;
	
	//An incremental number of the latest LRU number. Return LRU number of that page.
	static int incrementalLruNum;

public:
	//When a new page is loaded, add a LRU node to the tail. Return LRU number of that page.
	int addToTail(string pageId);
	
	//When a page is Moddified,
	int moveToTail(int LRU_number);
	
	//When evict a page, remove the LRU_Node from the head. Return the page Id.
	string evict();
	
	//Constructor of MyDB_LRU.
	MyDB_LRU();

	//Destructor of MyDB_LRU.
	~MyDB_LRU();
};
#endif
