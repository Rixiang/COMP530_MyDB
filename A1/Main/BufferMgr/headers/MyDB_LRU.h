
#ifndef LRU_H
#define LRU_H


#include<string>
#include<map>

using namespace std;

class MyDB_LRU{
    public:
        //LRU. When a new page is loaded, add a LRU node to the tail. Return LRU number of that page.
        int addToLruTail(string pageId);
        //LRU. When a page is accessed, update LRU number and position in LRU table.
        int moveToLruTail(int lru);

        //When evict a page, remove the from the head. Return the page Id.
        string evictFromLruHead();

        //Remove a page from a LRU. Happens when a unpinned page is get by getpinnedpage.
        string removeFromLru(int lru);

        MyDB_LRU();

        ~MyDB_LRU();

private:
	    unsigned int incLruNum;
        std::map<int, string> lruTable;
};

#endif
