
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
		//page->countHandle ++;
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
    cout << "call handle base destructoe with id;" << this->getPageId() <<endl;
    cout << "handle number before destroy:" << this->page->getCountHandle() << endl;
	this -> destroyPageHandle();
    cout << "handle number after destroy:" << this->page->getCountHandle() << endl;
}

void MyDB_PageHandleBase :: destroyPageHandle(){
	if (this->page->getCountHandle() > 0){
	    page->decreaseCountHandle();
    }
    cout << "destroy page with id:" << this->page->getPageId() << endl;
    cout << "handle number:" << this->page->getCountHandle() << endl;
	if (this->page->getCountHandle() == 0){
        cout << "anonymoust;" << this->page->getAnonymous() << endl;
        cout << "pinned;" << this->page->getPinned() << endl;

        if(this->page->getAnonymous()){
            cout << "destroy unnam page" <<endl;
            cout << "pool addres:" << poolptr <<endl;
            cout << "pool size befoe destroy;" << poolptr->size () <<endl;
            this -> poolptr->push(this->page->getPageAddr());
            cout << "pool size after destroy;" << poolptr->size () <<endl;
		    this->page->destroyPage();
            cout << "testhere" <<endl;
        } else if(this->page->getPinned()){
            cout << "do unpin"  << endl;
            this -> unpinPage();
        }
        
        //unPinned named page
        //Do nothing
        //Pinned unnamed page
        //UnPinned unnamed page
	}
	//delete this;
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
    lru -> addToLruTail(this->page->getPageId());
}
#endif

