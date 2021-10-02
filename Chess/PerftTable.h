#pragma once
#include <stdint.h>
#include <iostream>
#include <utility> 

using std::pair;
using std::make_pair;

//This file creates a transposition table to be used by perftt to increase perft speed and debug zobrist keys

struct EntryPerft{
		uint64_t key; //Zobrist key
		uint64_t count; //Number of moves
		int depth; //Depth board was searched to
	public: EntryPerft(uint64_t keyIn, uint64_t countIn, int depthIn) { //Create new entry
		key = keyIn;
		count = countIn;
		depth = depthIn;
	}
	public: EntryPerft() { //Creates empty entry
		key = 0;
		count = 0;
		depth = 0;
		}
};




class PerftTable
{	
	int tableSize = 0x2FFFFF; //The number of entries one table can have
	EntryPerft* table = new EntryPerft[tableSize]; //Creates array
	public: void add(EntryPerft entry) { //TODO: change replacement stratergy
		table[entry.key % tableSize] = entry; //Adds new entry to table
	}
	public: pair<bool, uint64_t> get_count(uint64_t key, int depth) { //Gets count from entry
		EntryPerft entry = table[key % tableSize]; //Gets entry
		if (entry.key == key && entry.depth == depth) { //If the entry key and depth match the one you are looking for
			return make_pair(true, entry.count); //Return the count
		}
		return make_pair(false, 0); //Returns false to indicate it couldn't be found
	}
	public: void delete_table() {
		delete[] table; //Deletes table after use
	}
};


