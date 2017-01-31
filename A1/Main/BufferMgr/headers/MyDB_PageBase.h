
#ifndef PAGE_H
#define PAGE_H

#include <memory>

class MyDB_PageBase {

private: 
	
	string pageID;			// id of the page

	long pageIndex;			// index of the page in its table

	size_t pageSize;		// size of a page

	void * pageAddr;		// memory address of a page in buffer's data pool
	
	bool pinned;			// true if a page is pinned

	bool dirty;				// true if the data has been modified, and need to be written back before eviction

	MyDB_TablePtr tablePtr;	// pointer to the table which contains this page
	

public:
	
	// getters and setters
	string getPageID();		
	size_t getPageSize();
	void * getPageAddr();

	bool getPinned();
	void setPinned(bool pinned);
	
	MyDP_TablePtr getTable();
	void setTable(MyDB_TablePtr);

	void wroteBytes();

	// write or load data to/from disk
	void writeData();
	void loadData();

	void keep();
	void release();


	//Constructor for a page
	MyDB_Page (size_t pageSize, void * pageAddr, long i);
	
	//Deconstructor for a page
	~MyDB_Page ();
	
}

#endif
