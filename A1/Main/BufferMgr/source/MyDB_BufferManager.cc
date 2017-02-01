
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utility>
#include <functional>
#include <iostream>

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
	MyDB_PageHandle pageHandle = nullptr;
	if (whichTable == nullptr){
		return pageHandle;
	}

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = whichTable -> getName().c_str() + to_string(i);
	unordered_map<string, MyDB_PageHandle>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found\n";

        void * address;
        if (emptySlotQueue.empty()){	// if the buffer is full, first evict one page
        	string evictedPageId = evictFromLruHead();

        	auto search = pageTable.find(evictedPageId);
        	if(search != pageTable.end()) {
		        address = search->second->getBytes();
		        // evict this page from buffer
		        search->second->destroyPageHandle();
		        if (search->second->getPage() != nullptr){
		        	search->second->getPage()->destroyPage();
		        }
		        pageTable.erase(evictedPageId);
		    }else {
		        cout << "Page to be evicted not found\n";
		    }
        }else{
        	// get address for one empty slot to put the data
			address = this->emptySlotQueue.front();
			this->emptySlotQueue.pop();
        }
		
    	// create a page handle as well as read file from disk
    	pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, pageId, this->pageSize, address, i);

    	// add the page handle into the page table in the buffer
    	pageTable.insert({pageId, pageHandle});
    	
    	// add the page id into the LRU table
		addToLruTail(pageId);

	}
	else{
		// create a page handle pointing to the existing pageBase Object
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second->getPage(), pageId, this->pageSize, nullptr, i);

		// update LRU

		moveToLruTail(pageHandle->getLRU());
    }
	
	return pageHandle;		
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	return nullptr;		
	
	// if new page
	//lru.addToTail(this->pageId);
	// else
	//lru.moveToTail(pageHandle->getLRU());
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i) {
    MyDB_PageHandle pageHandle = nullptr;
	if (whichTable == nullptr){
		return pageHandle;
	}

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = whichTable -> getName().c_str() + to_string(i);
	unordered_map<string, MyDB_PageHandle>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found\n";

        void * address;
        if (emptySlotQueue.empty()){	// if the buffer is full, first evict one page
        	string evictedPageId = evictFromLruHead();

        	auto search = pageTable.find(evictedPageId);
        	if(search != pageTable.end()) {
		        address = search->second->getBytes();
		        // evict this page from buffer
		        search->second->destroyPageHandle();
		        if (search->second->getPage() != nullptr){
		        	search->second->getPage()->destroyPage();
		        }
		        pageTable.erase(evictedPageId);
		    }else {
		        cout << "Page to be evicted not found\n";
		    }
        }else{
        	// get address for one empty slot to put the data
			address = this->emptySlotQueue.front();
			this->emptySlotQueue.pop();
        }
		
    	// create a page handle as well as read file from disk
    	pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, pageId, this->pageSize, address, i);

    	// add the page handle into the page table in the buffer
    	pageTable.insert({pageId, pageHandle});
    	
        // No LRU update.
	}
	else{
		// create a page handle pointing to the existing pageBase Object
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second->getPage(), pageId, this->pageSize, nullptr, i);

        // No LRU update.
    }
	
	return pageHandle;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	return nullptr;		
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    //Unpin the page and put in lruTable.
    unpinMe->unpinPage();
    addToLruTail(unpinMe->getPageId());
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pSize, size_t numP, string tempF) {
	// initialize private class members
	this->pageSize = pSize;
	this->numPages = numP;
	this->tempFile = tempF;
	this->incLruNum = 0;


	this->dataPool = new char*[numP];
	for (int i = 0; i < numP; i++){
		this->dataPool[i] = new char[pSize];
		this->emptySlotQueue.push(this->dataPool[i]);
	}


}

MyDB_BufferManager :: ~MyDB_BufferManager () {

	for (int i = 0; i < this->numPages; i++){
		delete this->dataPool[i];
	}
	delete dataPool;

    lruTable.clear();
}

//LRU. When a new page is loaded, add a LRU node to the tail. Return LRU number of that page.
int MyDB_BufferManager :: addToLruTail(string pageId){
    unsigned int lruNumber = ++incLruNum;
    lruTable.insert ( std::pair<int,string>(lruNumber, pageId) );
    return lruNumber;
}

//LRU. When a page is accessed, update LRU number and position in LRU table.
int MyDB_BufferManager :: moveToLruTail(int lru){
    unsigned int lruNumber = ++incLruNum;
    std::map<int,string>::iterator it;
    it = lruTable.find(lru);
    if (it != lruTable.end()) {
        lruTable.insert ( std::pair<int,string>(lruNumber, it->second) );
        lruTable.erase(it);
    }
    return lruNumber;
}
    
//When evict a page, remove the from the head. Return the page Id.
string MyDB_BufferManager :: evictFromLruHead() {
    std::map<int,string>::iterator it = lruTable.begin();
    string pageId = it->second;
    this->lruTable.erase(it);
    return pageId;
}
#endif
