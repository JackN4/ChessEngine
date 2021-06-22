#pragma once
#include "Board.h"
#include "BitOperations.h"
class Evaluator
{
	BitOperations bitOp;
	int vals[5] = { 100, 320, 330, 500, 900 };


public: int eval(Board &board) {
	int score = 0;
	for (int i = 0; i < 5; i++) {
		score += get_pop_count(board, 0, i + 2) * vals[i];
		score -= get_pop_count(board, 1, i + 2) * vals[i];
	}
	return score;
}

private: int get_pop_count(Board &board, int colour, int piece) {
	return bitOp.pop_count(board.bitboards[colour] & board.bitboards[piece]);
}
};

