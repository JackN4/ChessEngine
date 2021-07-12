#pragma once
#include <stdint.h>
#include <iostream>
#include <utility> 

using std::pair;
using std::make_pair;

struct Entry{
		uint64_t key;
		uint64_t count;
		int depth;
	public: Entry(uint64_t keyIn, uint64_t countIn, int depthIn) {
		key = keyIn;
		count = countIn;
		depth = depthIn;
	}
public: Entry() {
	key = 0;
	count = 0;
	depth = 0;
	}
};




class PerftTable
{	
	int tableSize = 0x2FFFFF;
	Entry* table = new Entry[tableSize];
	public: void add(Entry entry) { //changes index to new entry so always replaces
		table[entry.key % tableSize] = entry;
	}
	public: pair<bool, uint64_t> get_count(uint64_t key, int depth) {
		Entry entry = table[key % tableSize];
		if (entry.key == key && entry.depth == depth) {
			//std::cout << key << ":" << entry.key << "\n";
			//std::cout << "found" << "\n";
			return make_pair(true, entry.count);
		}
		return make_pair(false, 0);
	}
	public: void delete_table() {
		delete[] table;
	}
};


