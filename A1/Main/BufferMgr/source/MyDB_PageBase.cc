
MyDB_PageBase :: MyDB_PageBase (size_t pageSize, void * pageAddr) {
	this->pageSize = pageSize;
	this->pageAddr = pageAddr;
	this->dirty = false;
	this->pinned = false;
}

MyDB_PageBase :: ~MyDB_PageBase () {}

void * MyDB_PageBase :: getBytes () {
	return this->pageAddr;
}

void MyDB_PageBase :: wroteBytes () {
	this->dirty = false;
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

void MyDB_PageBase :: keep(){}
void MyDB_Page :: release(){}
