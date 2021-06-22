#pragma once
#include <bitset>
#include <vector>
#include <intrin.h>

#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse64)
using std::pair;
using std::vector;

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


vector<int> full_bitscan(uint64_t BB) { //array, length of array
	vector<int> result;
	int lsb;
	while (BB != 0) {
		lsb = lsb_bitscan(BB);
		result.push_back(lsb);
		BB -= 1ULL << lsb;
	}
	return result;
}


int pop_count(uint64_t BB) {
	return std::bitset<64>(BB).count();
}



};




