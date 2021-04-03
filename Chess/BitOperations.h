#pragma once
#include <bitset>
#include <list> 
using std::list;

const uint64_t debruijn = 0x03f79d71b4cb0a89;
const int debruijnArrFwd[64] = {
	0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};

const int debruijnArrRvs[64] = {
	0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

struct BitOperations {
int lsb_bitscan(uint64_t BB) {
	return debruijnArrFwd[((BB & -(int64_t)BB) * debruijn) >> 58];
}

int msb_bitscan(uint64_t BB) {
		BB |= BB >> 1;
		BB |= BB >> 2;
		BB |= BB >> 4;
		BB |= BB >> 8;
		BB |= BB >> 16;
		BB |= BB >> 32;
		return debruijnArrRvs[(BB * debruijn) >> 58];
	}


list <int> full_bitscan(uint64_t BB) {
		int lsb;
		list <int> result;
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

list <int>  get_bits_pos(uint64_t BB) {
	int popCount = pop_count(BB);
		if (popCount == 0) {
			list <int> result;
			return result;
		}
		else if (popCount == 1) {
			list <int> result = { debruijnArrFwd[(BB * debruijn) >> 58] }; //uses Debruijn algorithm to find LSB with a BB with only 1 set bit
			return result;
		}
		else {
			return full_bitscan(BB);
		}

	}



};




