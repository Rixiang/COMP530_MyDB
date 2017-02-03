
#ifndef PAGE_H
#define PAGE_H

#include <memory>
#include <string>
#include <queue>
#include "MyDB_Table.h"
#include <iostream>

using namespace std;

class MyDB_PageBase;
typedef shared_ptr <MyDB_PageBase> MyDB_PagePtr;

class MyDB_PageBase {

private: 
	
	string pageId;			// id of the page

	long pageIndex;			// index of the page in its table

	size_t pageSize;		// size of a page

	void * pageAddr;		// memory address of a page in buffer's data pool
	
	bool pinned;			// true if a page is pinned

	bool dirty;			// true if the data has been modified, and need to be written back before eviction

	bool anonymous;			// true if anonymous

	MyDB_TablePtr tablePtr;	// pointer to the table which contains this page

	int countHandle;		// count number of page handles pointint to this page
	
	int pageLRU;		// LRU number of this page

	queue<off_t> * emptySlotTmpFQueuePtr;		// queue for available slots in the temp file for anonymous pages to be written back
	
public:
	
	// getters and setters
	string getPageId();	

	size_t getPageSize();

	void * getPageAddr();

	long getPageIndex();

	bool getAnonymous();

	int getCountHandle();

	bool getPinned();
	void setPinned(bool pinned);

	int getLRU();
	void setLRU(int lru);

	// increase countHandle, when a new pageHandle is pointed to this page
	void increaseCountHandle();
	// decrease countHandle, when a pageHandle pointed to this page
	void decreaseCountHandle();

	MyDB_TablePtr getTable();
	void setTable(MyDB_TablePtr);

	void wroteBytes();

	// write or load data to/from disk
	void writeData();
	void loadData();

	void destroyPage();

	//Constructor for a page
	MyDB_PageBase (string id, MyDB_TablePtr tablePtr, size_t pageSize, void * pageAddr, 
					long i, bool anonymous, queue<off_t> *emptySlotTmpFQueue);
	
	//Deconstructor for a page
	~MyDB_PageBase ();
	
};

#endif
