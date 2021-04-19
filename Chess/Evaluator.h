#pragma once
#include "Board.h"
#include "BitOperations.h"
class Evaluator
{
	Board board;
	BitOperations bitOp;
	int vals[5] = { 100, 320, 330, 500, 900 };

public: Evaluator(Board &boardIn) {
	board = boardIn;
}
public: int eval() {
	int score = 0;
	for (int i = 0; i < 5; i++) {
		score += get_pop_count(0, i + 2) * vals[i];
		score -= get_pop_count(1, i + 2) * vals[i];
	}
	return score;
}

private: int get_pop_count(int colour, int piece) {
	return bitOp.pop_count(board.bitboards[colour] & board.bitboards[piece]);
}
};

