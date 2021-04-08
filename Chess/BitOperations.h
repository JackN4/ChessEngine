#pragma once
#include <bitset>
#include <list> 
#include <intrin.h>

#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse64)
using std::pair;

struct BitOperations {
int lsb_bitscan(uint64_t BB) {
	unsigned long index;
	_BitScanForward64(&index, BB);
	return index;
}


int msb_bitscan(uint64_t BB) {
	unsigned long index;
	_BitScanReverse64(&index, BB);
	return index;
}


pair<int[64],  int> full_bitscan(uint64_t BB) { //array, length of array
	pair<int[64], int> result;
	result.second = 0;
	int lsb;
	while (BB != 0) {
		lsb = lsb_bitscan(BB);
		result.first[result.second] = lsb;
		BB -= 1ULL << lsb;
		result.second++;
	}
	return result;
}


int pop_count(uint64_t BB) {
	return std::bitset<64>(BB).count();
}



};




