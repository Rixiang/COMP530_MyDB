
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


MyDB_BufferManager :: MyDB_BufferManager (size_t pSize, size_t numP, string tempF) {
	// initialize private class members
	this->pageSize = pSize;
	this->numPages = numP;
	this->tempFile = tempF;
	//this->incLruNum = 0;
    this->lru = new MyDB_LRU();


	this->dataPool = new char*[numP];
	for (int i = 0; i < numP; i++){
		this->dataPool[i] = new char[pSize];
		this->emptySlotQueue.push(this->dataPool[i]);
	}

    this->anonymousTable = make_shared<MyDB_Table>("Anonymous", "AnonymousTmp");
    this->anonymousNextAvail = 0;

}

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
	MyDB_PageHandle pageHandle = nullptr;
	if (whichTable == nullptr){
		return pageHandle;
	}
    cout << "a:::::" << pageHandle.use_count() <<endl;

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = whichTable -> getName().c_str() + to_string(i);
	unordered_map<string, MyDB_PageHandleBase>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found in the pageHandle table\n";

        void * address;
        if (emptySlotQueue.empty()){	// if the buffer is full, first evict one page
        	string evictedPageId = lru -> evictFromLruHead();
        	auto search = pageTable.find(evictedPageId);
        	if(search != pageTable.end()) {
		        address = search->second.getBytes();
		        cout << "page to be evicted is with address: " << address << endl;
		        // evict this page from buffer
		        search->second.destroyPageHandle();
		        if (search->second.getPage() != nullptr){
		        	search->second.getPage()->destroyPage();
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
    	pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, whichTable, pageId, this->pageSize, address, i, lru);

        cout << "b:::::" << pageHandle.use_count() <<endl;
    	// add the page handle into the page table in the buffer
    	pageTable.insert({pageId, *pageHandle});
        cout << "c:::::" << pageHandle.use_count() <<endl;
    	
    	// add the page id into the LRU table
		lru -> addToLruTail(pageId);

	}
	else{
		// create a page handle pointing to the existing pageBase Object
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second.getPage(), whichTable, pageId, this->pageSize, nullptr, i, lru);

		// update LRU
		lru -> moveToLruTail(pageHandle->getLRU());
    }
	
        cout << "d:::::" << pageHandle.use_count() <<endl;
	return pageHandle;		
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
    MyDB_PageHandle pageHandle = nullptr;

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = this -> anonymousTable -> getName().c_str() + to_string(anonymousNextAvail);
	unordered_map<string, MyDB_PageHandleBase>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found\n";

        void * address;
        if (emptySlotQueue.empty()){	// if the buffer is full, first evict one page
        	string evictedPageId = lru -> evictFromLruHead();

        	auto search = pageTable.find(evictedPageId);
        	if(search != pageTable.end()) {
		        address = search->second.getBytes();
		        // evict this page from buffer
		        search->second.destroyPageHandle();
		        if (search->second.getPage() != nullptr){
		        	search->second.getPage()->destroyPage();
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
    	pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, anonymousTable, pageId, this->pageSize, address, anonymousNextAvail, lru);

    	// add the page handle into the page table in the buffer
    	pageTable.insert({pageId, *pageHandle});
    	
    	// add the page id into the LRU table
		lru -> addToLruTail(pageId);

	}
	else{
		// create a page handle pointing to the existing pageBase Object
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second.getPage(), anonymousTable, pageId, this->pageSize, nullptr, anonymousNextAvail, lru);

		// update LRU

		lru -> moveToLruTail(pageHandle->getLRU());
    }

    anonymousNextAvail += pageHandle -> getPage() -> getPageSize();
	
	return pageHandle;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i) {
    MyDB_PageHandle pageHandle = nullptr;
	if (whichTable == nullptr){
		return pageHandle;
	}

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = whichTable -> getName().c_str() + to_string(i);
	unordered_map<string, MyDB_PageHandleBase>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found in the pageHandle table\n";

        void * address;
        if (emptySlotQueue.empty()){	// if the buffer is full, first evict one page
        	string evictedPageId = lru -> evictFromLruHead();

        	auto search = pageTable.find(evictedPageId);
        	if(search != pageTable.end()) {
		        address = search->second.getBytes();
		        // evict this page from buffer
		        search->second.destroyPageHandle();
		        if (search->second.getPage() != nullptr){
		        	search->second.getPage()->destroyPage();
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
    	pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, whichTable, pageId, this->pageSize, address, i, lru);

    	// add the page handle into the page table in the buffer
    	pageTable.insert({pageId, *pageHandle});
    	
        // No LRU update.
	}
	else{
		// create a page handle pointing to the existing pageBase Object
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second.getPage(), whichTable, pageId, this->pageSize, nullptr, i, lru);

        // No LRU update.
    }
	
	return pageHandle;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    MyDB_PageHandle pageHandle = nullptr;

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = this -> anonymousTable -> getName().c_str() + to_string(anonymousNextAvail);
	unordered_map<string, MyDB_PageHandleBase>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found\n";

        void * address;
        if (emptySlotQueue.empty()){	// if the buffer is full, first evict one page
			cout << pageId << " before strange\n";
        	string evictedPageId = lru -> evictFromLruHead();
        	cout << pageId << " strange\n";

        	auto search = pageTable.find(evictedPageId);
        	if(search != pageTable.end()) {

		        address = search->second.getBytes();
		        // evict this page from buffer
		        search->second.destroyPageHandle();
		        if (search->second.getPage() != nullptr){
		        	search->second.getPage()->destroyPage();
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
		pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, anonymousTable, pageId, this->pageSize, address, anonymousNextAvail, lru);	

    	// add the page handle into the page table in the buffer
    	pageTable.insert({pageId, *pageHandle});
    	
    	//No need add the page id into the LRU table
	}
	else{
		// create a page handle pointing to the existing pageBase Object
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second.getPage(), anonymousTable, pageId, this->pageSize, nullptr, anonymousNextAvail, lru);

		// No need update LRU
    }

    anonymousNextAvail += pageHandle -> getPage() -> getPageSize();
	
	return pageHandle;

}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    //Unpin the page and put in lruTable.
    unpinMe->unpinPage();
    //lru -> addToLruTail(unpinMe->getPageId());
}


MyDB_BufferManager :: ~MyDB_BufferManager () {

	for (int i = 0; i < this->numPages; i++){
		delete this->dataPool[i];
	}
	delete dataPool;

    //lruTable.clear();
    delete lru;
}

#endif
