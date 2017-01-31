
#ifndef PAGE_H
#define PAGE_H

#include <memory>

class MyDB_PageBase {

private: 
	
	string pageID;

	//Size of a page
	size_t pageSize;

	//Address of a page
	void * pageAddr;
	
	//Flag refers to if a page is pinned.
	bool pinned;

	
	bool dirty;

	MyDB_TablePtr tablePtr;


	
	

public:
	
	string getPageID();
	size_t getPageSize();
	void * getPageAddr();

	bool getPinned();
	void setPinned(bool pinned);

	void *getBytes();
	void wroteBytes();

	void keep();
	void release();
	
	MyDP_TablePtr getTable();
	void setTable(MyDB_TablePtr);


	//Constructor for a page
	MyDB_Page (size_t pageSize, void * pageAddr);
	
//Deconstructor for a page
	~MyDB_Page ();
	
}

#endif
