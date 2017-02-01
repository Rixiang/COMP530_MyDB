
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>
#include <sys/mman.h> // for mmap()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
	MyDB_PageHandle pageHandle = nullptr;
	if (whichTable == nullptr){
		return pageHandle;
	}

	const char * fileAddress = whichTable -> getStorageLoc().c_str();

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = whichTable -> getName().c_str() + to_string(i);
	unordered_map<string, MyDB_PageHandle>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found\n";

        void * address;
        if (emptySlotQueue.empty()){	// if the buffer is full, first evict one page

        }else{
        	// get address for one empty slot to put the data
			address = this->emptySlotQueue.front();
			this->emptySlotQueue.pop();
        }
		
    	// create a page handle to the page requested
    	pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, pageId, this->pageSize, address, i);

    	// add the page handle into the page table in the buffer
    	pageTable.insert(make_pair<string, MyDB_PageHandle>(pageId, pageHandle));
    	
    	// add the page id into the LRU table
		lru.addToTail(pageId);

	}
	else{
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second->page, pageId, this->pageSize, nullptr, i);

		// update LRU
		lru.moveToTail(pageHandle.getLRU());
    }
	
	return pageHandle;		
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	return nullptr;		
	
	// if new page
	lru.addToTail(pageId);
	// else
	lru.moveToTail(pageHandle.getLRU());
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr, long) {
	return nullptr;		
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	return nullptr;		
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
	//
	lru.addToTail(pageID);
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pSize, size_t numP, string tempF) {
	// initialize private class members
	this->pageSize = pSize;
	this->numPages = numP;
	this->empFile = tempF;
	this->pageTable = NULL;
	this->lru = new MyDB_LRU();

	this->bufferPool = new char*[numP];
	for (int i = 0; i < numP; i++){
		this->bufferPool[i] = new char[pSize];
		this->emptySlotQueue.push(this->bufferPool[i]);
	}

}

MyDB_BufferManager :: ~MyDB_BufferManager () {
	for (int i = 0; i < this->numPages; i++){
		delete this->bufferPool[i];
	}
	delete bufferPool;

	delete lru;
}
	
#endif




