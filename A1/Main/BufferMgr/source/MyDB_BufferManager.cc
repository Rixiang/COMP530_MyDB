
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
	if (whichTable == NULL){
		return pageHandle;
	}

	const char *fileAddress = whichTable -> getStorageLoc().c_str();

	// check whether the requested page is in the buffer
	// if is already in the page
	string pageId = whichTable -> getName().c_str() + to_string(i);
	unordered_map<string, MyDB_PageHandle>:: iterator got = pageTable.find(pageId);
	if (got == pageTable.end()){
        cout << pageId << " not found\n";
        
		// get the file descripter
		int fd = open (fileAddress, O_CREAT | O_RDWR | O_SYNC, 0666);
		off_t offset = i * pageSize;
		// map the page with the virtual address space
		char *addr = mmap(NULL, pageSize, PROT_READ, MAP_PRIVATE, fd, offset);
	    if (addr == MAP_FAILED){
	    	handle_error("mmap");
    	}
    	// create a page handle to the page requested
    	pageHandle = make_shared<MyDB_PageHandleBase>(fd, fileAddress, i);

    	// add the page handle into the page table in the buffer
    	pageTable.insert(make_pair<string, MyDB_PageHandle>(pageId, pageHandle));
    	
    	// add the page id into the LRU table

	}
	else{
    	pageHandle = got -> second;
    }
	
	return pageHandle;		
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	return nullptr;		
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr, long) {
	return nullptr;		
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	return nullptr;		
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pSize, size_t numP, string tempF) {
	// initialize private class members
	pageSize = pSize;
	numPages = numP;
	tempFile = tempF;

}

MyDB_BufferManager :: ~MyDB_BufferManager () {
}
	
#endif
