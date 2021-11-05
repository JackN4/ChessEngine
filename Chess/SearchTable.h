#pragma once
#include "Move.h"
#include <stdint.h>
#include <iostream>

using std::pair;
using std::make_pair;
using std::cout;

//This file defines the transposition table used when searching


// pv - all nodes searched
// cut (fail high, score>beta) this move is too good and the opponent would just avoid this position
// all (fail low, no score was >alpha) this position is not good enough as we can reach another better position, we will not make the move which allowed us to be put in this position

struct MoveStore { //Need to store a move in as smallest space possible to keep tranposition table as small as possible
	char start; //Stores start position of piece
	char end; //End position of piece
	char special; //Stores promotion, castling or en passant information
	public: MoveStore(Move move) {
		start = move.startPos;
		end = move.endPos;
		special = move.promoPiece; // only 1 of these 3 will be true about a special type of move
		special = move.castling;
		special = move.enPassant;
	}
	public: MoveStore() { //Creates empty move
		start = 0;
		end = 0;
		special = 0;
	}
};

struct EntrySearch {  //This defines the information stored in each entry of the table
	uint64_t key; //Zobrist key
	MoveStore bestMove; //Best move for board
	int depth; //Depth left to search
	int value; //End value
	int node; //What type of node it is (how it ended) : 1-pv(exact) 2-cut(lower bound) 3(upper bound)
public: EntrySearch(uint64_t keyIn, int depthIn, int valueIn, int nodeIn, Move bestMoveIn) { //Creates new entry
	key = keyIn;
	bestMove = MoveStore(bestMoveIn);
	depth = depthIn;
	value = valueIn;
	node = nodeIn;
}
public: EntrySearch(uint64_t keyIn, int depthIn, int valueIn, int nodeIn) { //Creates new entry without bestmove
	key = keyIn;
	bestMove = MoveStore();
	depth = depthIn;
	value = valueIn;
	node = nodeIn;
}
public: EntrySearch() {//Creates empty entry
	key = 0;
	depth = 0;
	value = 0;
	node = 0;
}
};

class SearchTable
{
	int tableSize = 0x2FFFFF; //Size of table
	EntrySearch* table = new EntrySearch[tableSize]; //Array for table
	public: void add(EntrySearch entry) {  //Adds new entry
		table[entry.key % tableSize] = entry;
	}
	public: pair<bool, EntrySearch> get_entry(uint64_t key) { //Gets entry
		EntrySearch entry = table[key % tableSize];
		if (entry.key == key) {
			return make_pair(true, entry); //the entry exists
		}
		else {		
			return make_pair(false, entry); //the entry doesn't exist
		}
	}
	public: void delete_table() { //Deletes table
		delete[] table;
	}
};



