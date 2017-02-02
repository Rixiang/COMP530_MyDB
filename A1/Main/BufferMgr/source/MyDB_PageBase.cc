#ifndef PAGE_BASE_C
#define PAGE_BASE_C

#include "MyDB_PageBase.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

MyDB_PageBase :: MyDB_PageBase (string id, MyDB_TablePtr tablePtr, size_t pageSize, void * pageAddr, long i, bool anonymous) {
	this->pageId = id;
	this->pageSize = pageSize;
	this->pageAddr = pageAddr;
	this->pageIndex = i;
	this->dirty = false;
	this->pinned = false;
	this->countHandle = 1;
	this->tablePtr = tablePtr;
	this->pageLRU = 1;
	this->loadData();
    this->anonymous = anonymous;
}

bool MyDB_PageBase :: getAnonymous(){
	return this->anonymous;
}

void MyDB_PageBase :: wroteBytes () {
	this->dirty = true;
}

string MyDB_PageBase :: getPageId(){
	return this->pageId;
}

size_t MyDB_PageBase :: getPageSize(){
	return this->pageSize;
}

void * MyDB_PageBase :: getPageAddr(){
	return this->pageAddr;
}

bool MyDB_PageBase :: getPinned(){
	return this->pinned;
}

void MyDB_PageBase :: setPinned(bool pinned){
	this->pinned = pinned;
}

int MyDB_PageBase :: getLRU(){
	return this->pageLRU;
}

void MyDB_PageBase :: setLRU(int lru){
	this->pageLRU = lru;
}

int MyDB_PageBase :: getCountHandle() {
    return this->countHandle;
} 

void MyDB_PageBase :: increaseCountHandle(){
    this->countHandle++;
}

void MyDB_PageBase :: decreaseCountHandle(){
    this->countHandle--;
}

void MyDB_PageBase :: writeData(){
	const char *fileAddress = (this->tablePtr)->getStorageLoc().c_str();
	int fd = open (fileAddress, O_CREAT | O_RDWR | O_FSYNC, 0666);	// 'O_FSYNC' to let writes not buffered by the operating system
	if (fd == -1){
		cout << "Error happens in opening file" << endl;
	}else{
		off_t offset = this->pageIndex * this->pageSize;
		off_t offsetLoc = lseek(fd, offset, SEEK_SET);		// reposition the file descripter to the specified page
		if (offsetLoc == -1){
			cout << "Error happens in lseeking file" << endl;
		}else{
			ssize_t numBytes = write(fd, this->pageAddr, this->pageSize);
			if (numBytes == -1){
				cout << "Error happens in writing file" << endl;
			}
		}
	}
	int closeId = close(fd);		// close the file descripter
	if (closeId != 0){
		cout << "Error happens in closing file" << endl;
	}
}

void MyDB_PageBase :: loadData(){

	const char *fileAddress = ((this->tablePtr)->getStorageLoc()).c_str();

	int fd = open (fileAddress, O_CREAT | O_RDWR | O_FSYNC, 0666);

	if (fd == -1){
		cout << "Error happens in opening file" << endl;
	}else{
		off_t offset = this->pageIndex * this->pageSize;
		off_t offsetLoc = lseek(fd, offset, SEEK_SET);		// reposition the file descripter to the specified page
		if (offsetLoc == -1){
			cout << "Error happens in lseeking file" << endl;
		}else{
			ssize_t numBytes = read(fd, this->pageAddr, this->pageSize);
			if (numBytes == -1){
				cout << pageAddr << endl;
				cout << "Error happens in reading file" << endl;
			}
		}
	}
	int closeId = close(fd);		// close the file descripter
	if (closeId != 0){
		cout << "Error happens in closing file" << endl;
	}

}

void MyDB_PageBase :: destroyPage(){
	if (this->dirty == true){
		writeData();		// write page back to disk if page has been modified
	}
	//delete this;
}

MyDB_PageBase :: ~MyDB_PageBase () {
	/*if (this->countHandle == 0){
		this->destroyPage();
	}*/
}

#endif
