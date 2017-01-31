#ifndef PAGE_BASE_C
#define PAGE_BASE_C

#include "MyDB_PageBase.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

MyDB_PageBase :: MyDB_PageBase (size_t pageSize, void * pageAddr, long i) {
	this->pageSize = pageSize;
	this->pageAddr = pageAddr;
	this->pageIndex = i;
	this->dirty = false;
	this->pinned = false;
	this->loadData();
}

MyDB_PageBase :: ~MyDB_PageBase () {}


void MyDB_PageBase :: wroteBytes () {
	this->dirty = true;
}

string MyDB_PageBase :: getPageID(){
	return this->pageID;
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

void MyDB_PageBase :: writeData(){
	const char *fileAddress = (this->whichTable)->getStorageLoc().c_str();
	int fd = open (fileAddress, O_CREAT | O_RDWR | O_SYNC, 0666);
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
	int close = close(fd);		// close the file descripter
	if (close != 0){
		cout << "Error happens in closing file" << endl;
	}
	
}

void MyDB_PageBase :: loadData(){
	const char *fileAddress = (this->whichTable)->getStorageLoc().c_str();
	int fd = open (fileAddress, O_CREAT | O_RDWR | O_SYNC, 0666);
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
				cout << "Error happens in reading file" << endl;
			}
		}
	}
	int close = close(fd);		// close the file descripter
	if (close != 0){
		cout << "Error happens in closing file" << endl;
	}

}

void MyDB_PageBase :: keep(){}
void MyDB_Page :: release(){}


#endif
