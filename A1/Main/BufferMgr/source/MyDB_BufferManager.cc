
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
        	string evictedPageId = evictFromLruHead();

        	auto search = pageTable.find(evictedPageId);
        	if(search != pageTable.end()) {
		        address = search->second->getBytes();
		        // evict this page from buffer
		        search->second->page->destroyPage();
		    }else {
		        cout << "Page to be evicted not found\n";
		    }

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
		addToLruTail(pageId);

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
    MyDB_PageHandle pageHandle = nullptr;
	if (whichTable == nullptr){
		return pageHandle;
	}

	const char *fileAddress = whichTable -> getStorageLoc().c_str();

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = whichTable -> getName().c_str() + to_string(i);
	unordered_map<string, MyDB_PageHandle>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found\n";
        
		
		// map the page with the virtual address space
		char *addr = mmap(NULL, pageSize, PROT_READ, MAP_PRIVATE, fd, offset);
	    	if (addr == MAP_FAILED){
	    		handle_error("mmap");
    		}

    	// create a page handle to the page requested
    	pageHandle = make_shared<MyDB_PageHandleBase>(nullptr, pageId, this->pageSize, , i);

    	// add the page handle into the page table in the buffer
    	pageTable.insert(make_pair<string, MyDB_PageHandle>(pageId, pageHandle));
    	
    	// add the page id into the LRU table
		addToLruTail(pageId);

	}
	else{
    	pageHandle = make_shared<MyDB_PageHandleBase>(got->second->page, pageId, this->pageSize, i);

		// NO need to put pinned page in LRU
    }
	
	return pageHandle;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	return nullptr;		
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    //Unpin the page and put in lruTable.
    unpinMe->unpinPage()
    addToLruTail(unpinMe->getPageId());
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pSize, size_t numP, string tempF) {
	// initialize private class members
	this->pageSize = pSize;
	this->numPages = numP;
	this->empFile = tempF;
	this->pageTable = NULL;
	this->lru = new MyDB_LRU();
	this->incLruNum = 0;


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
    lruTable.clear()
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
    lruTable.erase(it);
}
#endif


