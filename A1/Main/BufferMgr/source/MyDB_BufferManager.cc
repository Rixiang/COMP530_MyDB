
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

    cout << "\n";
    cout << "MyDB-getPage (MyDB_TablePtr whichTable, long i)\n" << i << endl;
    cout << "buffer manageer lruptr:  " << lru <<endl;

	MyDB_PageHandle pageHandle = nullptr;
	if (whichTable == nullptr){
		return pageHandle;
	}

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = whichTable -> getName().c_str();
	pageId.append("jzhlx");
	pageId.append(to_string(i));

	unordered_map<string, MyDB_PageHandleBase>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found in the pageHandle table\n";

        void * address;
        if (emptySlotQueue.empty()){	// if the buffer is full, first evict one page
            address = evict();
        }else{
            cout << "getpaget(...): q.size > 0 no evict: " << endl;
            cout << "getpaget(...): q size before pop: " << emptySlotQueue.size() << endl;
        	// get address for one empty slot to put the data
            cout << "getpaget(...): emptyslot " << emptySlotQueue.front() << endl;
			address = this->emptySlotQueue.front();
			this->emptySlotQueue.pop();
            cout << "getpaget(...): q size after pop: " << emptySlotQueue.size() << endl;
        }

        cout << "getpaget(...): before create pagehanlde use count: " << pageHandle.use_count() << endl;
    	// create a page handle as well as read file from disk
    	pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, whichTable, pageId, this->pageSize, address, i, false, lru, &emptySlotQueue);

        cout << "getpaget(...): after create pagehanlde use count: " << pageHandle.use_count() << endl;
    	// add the page handle into the page table in the buffer
    	pageTable.insert({pageId, *pageHandle});
        cout << "getpaget(...): insert pagehanldebase use count: " << pageHandle.use_count() << endl;
    	
    	// add the page id into the LRU table
		lru -> addToLruTail(pageId);

	}
	else{
        cout << "the page is already in the table."<<endl;
		// create a page handle pointing to the existing pageBase Object
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second.getPage(), whichTable, pageId, this->pageSize, nullptr, i, false, lru, &emptySlotQueue);

		// update LRU
        if(pageHandle->getPage()->getPinned()==true) {
            //From Pinned to Unpinned.
            cout << "before unpin it:"<< pageHandle->getPage()->getPinned() <<endl;
            pageHandle->unpinPage();
            cout << "waht is get from lru?"<< pageHandle->getPage()->getLRU() <<endl;
            cout << "after unpin it:"<< pageHandle->getPage()->getPinned() <<endl;
        } else {
            //Stay Unpinned.
		    lru -> moveToLruTail(pageHandle->getLRU());
        }
    }
	
    cout << "getpaget(...): pageHandle: " << pageHandle << endl;
    cout << "getpaget(...): pageHandle count: " << pageHandle.use_count() << endl;
	return pageHandle;		
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
    MyDB_PageHandle pageHandle = nullptr;

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = this -> anonymousTable -> getName().c_str();
	pageId.append("jzhlx");
	pageId.append(to_string(anonymousNextAvail));

	unordered_map<string, MyDB_PageHandleBase>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found in the pageHandle table\n";

        void * address;
        if (emptySlotQueue.empty()){	// if the buffer is full, first evict one page
            evict();
        }else{
        	// get address for one empty slot to put the data
			address = this->emptySlotQueue.front();
			this->emptySlotQueue.pop();
        }
		
    	// create a page handle as well as read file from disk
    	//pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, anonymousTable, pageId, this->pageSize, address, anonymousNextAvail, lru);
    	pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, anonymousTable, pageId, this->pageSize, address, anonymousNextAvail, true, lru, &emptySlotQueue);

    	// add the page handle into the page table in the buffer
    	pageTable.insert({pageId, *pageHandle});
    	
    	// add the page id into the LRU table
		lru -> addToLruTail(pageId);

	}
	else{
		// create a page handle pointing to the existing pageBase Object
    	//pageHandle = make_shared<MyDB_PageHandleBase>(got->second.getPage(), anonymousTable, pageId, this->pageSize, nullptr, anonymousNextAvail, lru);
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second.getPage(), anonymousTable, pageId, this->pageSize, nullptr, anonymousNextAvail, true, lru, &emptySlotQueue);

		// update LRU
        if(pageHandle->getPage()->getPinned()==true) {
            //Previouly Pinned page, change to Unpinned.
            cout << "before unpin it:"<< pageHandle->getPage()->getPinned() <<endl;
            pageHandle->unpinPage();
            cout << "waht is get from lru?"<< pageHandle->getPage()->getLRU() <<endl;
            cout << "after unpin it:"<< pageHandle->getPage()->getPinned() <<endl;
        } else {
            //Previously Unpinned page, stay Unpinned.
		    lru -> moveToLruTail(pageHandle->getLRU());
        }
    }
    anonymousNextAvail++;
	
	return pageHandle;
}


MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i) {
    cout << "\n";
    cout << "MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i)\n" << i <<endl;
    cout << "buffer manageer lruptr:  " << lru <<endl;

    MyDB_PageHandle pageHandle = nullptr;
	if (whichTable == nullptr){
		return pageHandle;
	}

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = whichTable -> getName().c_str();
	pageId.append("jzhlx");
	pageId.append(to_string(i));

	unordered_map<string, MyDB_PageHandleBase>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found in the pageHandle table\n";

        void * address ;
        cout << "getpaget(...): q size: " << emptySlotQueue.size() << endl;
        if (emptySlotQueue.empty()){	// if the buffer is full, first evict one page
            address = evict();
        }else{
            cout << "getpaget(...): q.size > 0 no evict: " << endl;
            cout << "getpaget(...): q size before pop: " << emptySlotQueue.size() << endl;
        	// get address for one empty slot to put the data
            cout << "getpaget(...): emptyslot " << emptySlotQueue.front() << endl;
			address = this->emptySlotQueue.front();
			this->emptySlotQueue.pop();
            cout << "getpaget(...): q size after pop: " << emptySlotQueue.size() << endl;
        }
		
        cout << "getpaget(...): before create pagehanlde use count: " << pageHandle.use_count() << endl;

    	// create a page handle as well as read file from disk
    	pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, whichTable, pageId, this->pageSize, address, i, false, lru, &emptySlotQueue);
        pageHandle->getPage()->setPinned(true);
        cout << "getpaget(...): after create pagehanlde use count: " << pageHandle.use_count() << endl;

    	// add the page handle into the page table in the buffer
    	pageTable.insert({pageId, *pageHandle});
        cout << "getpaget(...): insert pagehanldebase use count: " << pageHandle.use_count() << endl;
    	
        // No LRU update.
	}
	else{
        cout << "the page is already in the table."<<endl;
		// create a page handle pointing to the existing pageBase Object
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second.getPage(), whichTable, pageId, this->pageSize, nullptr, i, false, lru, &emptySlotQueue);

		// Previous Unpinned, set it to pinned.
        cout << "is "<< pageId <<" pinned? "<< pageHandle->getPage()->getPinned() <<endl;
        if(pageHandle->getPage()->getPinned()==false) {
            cout << "before pin it:"<< pageHandle->getPage()->getPinned() <<endl;
            pageHandle->getPage()->setPinned(true);
            cout << "waht is get from lru?"<< pageHandle->getPage()->getLRU() <<endl;
            cout << "after pin it:"<< pageHandle->getPage()->getPinned() <<endl;
            lru -> removeFromLru(pageHandle->getPage()->getLRU());
        }
    }
	
    cout << "getpaget(...): pageHandle: " << pageHandle << endl;
    cout << "getpaget(...): pageHandle count: " << pageHandle.use_count() << endl;
	return pageHandle;
}


MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {

    MyDB_PageHandle pageHandle = nullptr;

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = this -> anonymousTable -> getName().c_str();
	pageId.append("jzhlx");
	pageId.append(to_string(anonymousNextAvail));

	unordered_map<string, MyDB_PageHandleBase>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found in the pageHandle table\n";

        void * address ;
        if (emptySlotQueue.empty()){	// if the buffer is full, first evict one page
            address = evict();
        }else{
            cout << "getpaget(...): q.size > 0 no evict: " << endl;
            cout << "getpaget(...): q size before pop: " << emptySlotQueue.size() << endl;
        	// get address for one empty slot to put the data
            cout << "getpaget(...): emptyslot " << emptySlotQueue.front() << endl;
			address = this->emptySlotQueue.front();
			this->emptySlotQueue.pop();
            cout << "getpaget(...): q size after pop: " << emptySlotQueue.size() << endl;
        }
		
        cout << "getpaget(...): before create pagehanlde use count: " << pageHandle.use_count() << endl;

    	// create a page handle as well as read file from disk
    	//pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, whichTable, pageId, this->pageSize, address, i, false, lru, &emptySlotQueue);
    	pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, anonymousTable, pageId, this->pageSize, address, anonymousNextAvail, true, lru, &emptySlotQueue);
        pageHandle->getPage()->setPinned(true);
        cout << "getpaget(...): after create pagehanlde use count: " << pageHandle.use_count() << endl;

    	// add the page handle into the page table in the buffer
    	pageTable.insert({pageId, *pageHandle});
        cout << "getpaget(...): insert pagehanldebase use count: " << pageHandle.use_count() << endl;
    	
        // No LRU update.
	}
	else{
        cout << "the page is already in the table."<<endl;
		// create a page handle pointing to the existing pageBase Object
    	//pageHandle = make_shared<MyDB_PageHandleBase>(got->second.getPage(), whichTable, pageId, this->pageSize, nullptr, i, false, lru, &emptySlotQueue);
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second.getPage(), anonymousTable, pageId, this->pageSize, nullptr, anonymousNextAvail, true, lru, &emptySlotQueue);

		// Previous Unpinned, set it to pinned.
        cout << "is "<< pageId <<" pinned? "<< pageHandle->getPage()->getPinned() <<endl;
        if(pageHandle->getPage()->getPinned()==false) {
            cout << "before pin it:"<< pageHandle->getPage()->getPinned() <<endl;
            pageHandle->getPage()->setPinned(true);
            cout << "waht is get from lru?"<< pageHandle->getPage()->getLRU() <<endl;
            cout << "after pin it:"<< pageHandle->getPage()->getPinned() <<endl;
            lru -> removeFromLru(pageHandle->getPage()->getLRU());
        }
    }
    anonymousNextAvail++;
	return pageHandle;
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    //Unpin the page and put in lruTable.
    unpinMe->unpinPage();
    //lru -> addToLruTail(unpinMe->getPageId());
}

void * MyDB_BufferManager ::  evict(){
    void * address = nullptr;
    cout << "getpaget(...): q.size = 0 evict: " << endl;
    string evictedPageId = lru -> evictFromLruHead();
    cout << "getpaget(...): q.size = 0 evict pageID: " << evictedPageId <<endl;
    auto search = pageTable.find(evictedPageId);
    if(search != pageTable.end()) {
        cout << "getpaget(...): found in buffer table evict pageID: " << evictedPageId <<endl;
        address = search->second.getBytes();
        cout << "getpaget(...): evict pageID, addree: " << evictedPageId << " "<< address <<endl;
        // evict this page from buffer
        //search->second.destroyPageHandle();
        if (search->second.getPage() != nullptr){
            cout << "getpaget(...): evict pageID, getPage: " << evictedPageId << " "<< search->second.getPage() <<endl;
            search->second.getPage()->destroyPage();
            cout << "getpaget(...): q size before erase: " << emptySlotQueue.size() << endl;
            pageTable.erase(evictedPageId);
            cout << "getpaget(...): q size after erase: " << emptySlotQueue.size() << endl;
        }
    }else {
        cout << "Page to be evicted not found\n";
    }
    return address;
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
