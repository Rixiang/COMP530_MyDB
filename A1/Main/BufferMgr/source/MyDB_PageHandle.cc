
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"
#include <iostream>

void * MyDB_PageHandleBase :: getBytes () {
	return this->page->getPageAddr();
}

void MyDB_PageHandleBase :: wroteBytes () {
	this->page->wroteBytes();
}

MyDB_PageHandleBase :: MyDB_PageHandleBase (MyDB_PagePtr page, MyDB_TablePtr tablePtr, string id, size_t pageSize, void * pageAddr, long i){
	if (page != nullptr){
		page->increaseCountHandle();
		this->page = page;
	}else{
		page = make_shared<MyDB_PageBase>(id, tablePtr, pageSize, pageAddr, i);
		this->page = page;
	}
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	this -> destroyPageHandle();
}

void MyDB_PageHandleBase :: destroyPageHandle(){
	page->decreaseCountHandle();
	if (this->page->getCountHandle() == 0){
		if (this->page->getPinned() == true){
			unpin();
		}
		//emptySlotQueue;
		this->page->destroyPage();
	}
}


int MyDB_PageHandleBase :: getLRU () {
	return this->page->getLRU();
}

void MyDB_PageHandleBase :: setLRU (int lru) {
	this->page->setLRU(lru);
}

string MyDB_PageHandleBase :: getPageId () {
	return this->page->getPageId();
}

MyDB_PagePtr MyDB_PageHandleBase :: getPage () {
	return this->page;
}

void MyDB_PageHandleBase :: unpinPage () {
	this->page->setPinned(false);
}
#endif

