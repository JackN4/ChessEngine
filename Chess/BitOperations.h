#pragma once
#include <bitset>
#include <list> 
#include <intrin.h>

#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse64)
using std::pair;

//This file manages many common bitwise operations

struct BitOperations {
int lsb_bitscan(uint64_t BB) { //Finds the position of the least significant bit in a bit board
	unsigned long index;
	_BitScanForward64(&index, BB); //uses built in function - only works when ran in 64 bit
	return index;
}


int msb_bitscan(uint64_t BB) { //Finds the position of the most significant bit in a bit board
	unsigned long index;
	_BitScanReverse64(&index, BB); //uses built in function - only works when ran in 64 bit
	return index;
}


pair<int[64],  int> full_bitscan(uint64_t BB) { //Finds the position of all bits in a bitboard, returns an array, of max length 64, and the length of array that is used
	pair<int[64], int> result;
	result.second = 0;
	int lsb;
	while (BB != 0) { //If theres still bits left
		lsb = lsb_bitscan(BB); //Finds lsb
		result.first[result.second] = lsb; //Adds position to array
		BB -= 1ULL << lsb; //Removes lsb from bitboard
		result.second++; //Increments length of used array
	}
	return result;
}


int pop_count(uint64_t BB) { //Finds number of 1 bits in bitboard
	return std::bitset<64>(BB).count(); //uses built in function
}



};




