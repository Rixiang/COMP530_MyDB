
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

MyDB_PageHandleBase :: MyDB_PageHandleBase (MyDB_PagePtr page, MyDB_TablePtr tablePtr, string id, size_t pageSize, void * pageAddr, long i,bool anonymous, MyDB_LRU *lru, queue<void *> *poolptr){
	if (page != nullptr){
		this->page = page;
	}else{
		//MyDB_PageBase pageBase = new MyDB_PageBase(id, tablePtr, pageSize, pageAddr, i);
		page = make_shared<MyDB_PageBase>(id, tablePtr, pageSize, pageAddr, i, anonymous);
		this->page = page;
	}
	page->increaseCountHandle();

    this->lru = lru;
    this->poolptr = poolptr;
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	this -> destroyPageHandle();
}

void MyDB_PageHandleBase :: destroyPageHandle(){
	page->decreaseCountHandle();
	if (this->page->getCountHandle() == 0){
        if(this->page->getAnonymous()){
            this -> poolptr->push(this->page->getPageAddr());
		    this->page->destroyPage();
        } else if(this->page->getPinned()){
            this -> unpinPage();
        }
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
    this->page->setLRU( lru -> addToLruTail(this->page->getPageId()) );
}
#endif

