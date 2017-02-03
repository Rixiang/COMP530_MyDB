
#ifndef LRU_C
#define LRU_C

#include<iostream>
#include "MyDB_LRU.h"

using namespace std;

//LRU. When a new page is loaded, add a LRU node to the tail. Return LRU number of that page.
int MyDB_LRU :: addToLruTail(string pageId){
    unsigned int lruNumber = ++incLruNum;
    lruTable.insert ( std::pair<int,string>(lruNumber, pageId) );
    return lruNumber;
}

//LRU. When a page is accessed, update LRU number and position in LRU table.
int MyDB_LRU :: moveToLruTail(int lru){
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
string MyDB_LRU :: evictFromLruHead() {
    if( lruTable.size() == 0 ) {
        return "";
    }
    std::map<int,string>::iterator it = lruTable.begin();
    string pageId = it->second;
    cout << "Call LRU EVICT: lru size:" << lruTable.size() <<endl;
    cout << "\nCall LRU EVICT: lrunumber:" << it->first <<endl;
    cout << "Call LRU EVICT: id in page:"<< it->second <<endl;
    this->lruTable.erase(it);
    cout << "Call LRU EVICT: pageID:" << pageId << endl;
    return pageId;
}

string MyDB_LRU :: removeFromLru(int lru) {
    cout << "Call REmove from lru: lru:" << lru <<endl;
    string pageId = "";
    if( lru == 0 || lruTable.size() == 0 ) {
        return "";
    }
    std::map<int,string>::iterator it = lruTable.find(lru);
    if (it != lruTable.end()) {
        pageId = it->second;
        cout << "\nCall LRU remove: lru size:" << lruTable.size() <<endl;
        cout << "Call LRU remove: lrunumber:" << it->first <<endl;
        cout << "Call LRU remove: id in page:"<< it->second <<endl;
        lruTable.erase(it);
    }
    return pageId;
}

MyDB_LRU :: MyDB_LRU(){
    this->incLruNum = 0;
}

MyDB_LRU :: ~MyDB_LRU(){
    lruTable.clear();
}
#endif
