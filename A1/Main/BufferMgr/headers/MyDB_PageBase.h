
#ifndef PAGE_H
#define PAGE_H

#include <memory>
#include <string>
#include "MyDB_Table.h"

using namespace std;

class MyDB_PageBase;
typedef shared_ptr <MyDB_PageBase> MyDB_PagePtr;

class MyDB_PageBase {

private: 
	
	string pageID;			// id of the page

	long pageIndex;			// index of the page in its table

	size_t pageSize;		// size of a page

	void * pageAddr;		// memory address of a page in buffer's data pool
	
	bool pinned;			// true if a page is pinned

	bool dirty;			// true if the data has been modified, and need to be written back before eviction

	MyDB_TablePtr tablePtr;	// pointer to the table which contains this page

	int countHandle;		// count number of page handles pointint to this page
	
	int pageLRU;		// LRU number of this page
	
public:
	
	// getters and setters
	string getPageId();		
	size_t getPageSize();
	void * getPageAddr();

	bool getPinned();
	void setPinned(bool pinned);

	int getLRU();
	void setLRU(int lru);
	
    int getCountHandle();
	void increaseCountHandle();
	void decreaseCountHandle();

	MyDB_TablePtr getTable();
	void setTable(MyDB_TablePtr);

	void wroteBytes();

	// write or load data to/from disk
	void writeData();
	void loadData();

	

	void destroyPage();


	//Constructor for a page
	MyDB_PageBase (string id, size_t pageSize, void * pageAddr, long i);
	
	//Deconstructor for a page
	~MyDB_PageBase ();
	
};

#endif
