#pragma once
#include "Move.h"
#include <stdint.h>
#include <iostream>

using std::pair;
using std::make_pair;
using std::cout;




// pv - all nodes searched
// cut (fail high, score>beta) this move is too good and the opponent would just avoid this position
// all (fail low, no score was >alpha) this position is not good enough as we can reach another better position, we will not make the move which allowed us to be put in this position

struct MoveStore { //efficient way to store move
	char start;
	char end;
	char special;
	public: MoveStore(Move move) {
		start = move.startPos;
		end = move.endPos;
		special = move.promoPiece; // only 1 of these 3 will be true about a special type of move
		special = move.castling;
		special = move.enPassant;
	}
	public: MoveStore() {
		start = 0;
		end = 0;
		special = 0;
	}
};

struct EntrySearch { //add age
	uint64_t key;
	MoveStore bestMove;
	int depth;
	int value;
	int node; //1-pv(exact) 2-cut(lower bound) 3(upper bound)
public: EntrySearch(uint64_t keyIn, int depthIn, int valueIn, int nodeIn, Move bestMoveIn) {
	key = keyIn;
	bestMove = MoveStore(bestMoveIn);
	depth = depthIn;
	value = valueIn;
	node = nodeIn;
}
public: EntrySearch(uint64_t keyIn, int depthIn, int valueIn, int nodeIn) {
	key = keyIn;
	bestMove = MoveStore();
	depth = depthIn;
	value = valueIn;
	node = nodeIn;
}
public: EntrySearch() {
	key = 0;
	depth = 0;
	value = 0;
	node = 0;
}
};

class SearchTable
{
	int tableSize = 0x2FFFFF;
	EntrySearch* table = new EntrySearch[tableSize];
	public: void add(EntrySearch entry) { //changes index to new entry so always replaces . NEED TO CHANGE REPLACEMENT
		table[entry.key % tableSize] = entry;
	}
	public: pair<bool, EntrySearch> get_entry(uint64_t key) {
		EntrySearch entry = table[key % tableSize];
		if (entry.key == key) {
			return make_pair(true, entry); //the entry exists
		}
		else {		
			return make_pair(false, entry); //the entry doesn't exist
		}
	}
	public: void delete_table() {
		delete[] table;
	}
};



