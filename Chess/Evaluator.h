#pragma once
#include "Board.h"
#include "BitOperations.h"
class Evaluator
{
	Board board;
	BitOperations bitOp;
	int vals[5] = { 100, 320, 330, 500, 900 };
	int PSTs[2][4][64] = { {
		{0,  0,  0,  0,  0,  0,  0,  0,
	50, 50, 50, 50, 50, 50, 50, 50,
	10, 10, 20, 30, 30, 20, 10, 10,
	 5,  5, 10, 27, 27, 10,  5,  5,
	 0,  0,  0, 25, 25,  0,  0,  0,
	 5, -5,-10,  0,  0,-10, -5,  5,
	 5, 10, 10,-25,-25, 10, 10,  5,
	 0,  0,  0,  0,  0,  0,  0,  0},
	 {-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-50,-40,-20,-30,-30,-20,-40,-50},
	{-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-20,-10,-40,-10,-10,-40,-10,-20},
	{-30, -40, -40, -50, -50, -40, -40, -30,
  -30, -40, -40, -50, -50, -40, -40, -30,
  -30, -40, -40, -50, -50, -40, -40, -30,
  -30, -40, -40, -50, -50, -40, -40, -30,
  -20, -30, -30, -40, -40, -30, -30, -20,
  -10, -20, -20, -20, -20, -20, -20, -10,
   20,  20,   0,   0,   0,   0,  20,  20,
   20,  30,  10,   0,   0,  10,  30,  20}
	},
	{ {0, 0, 0, 0, 0, 0, 0, 0,
	5, 10, 10, -25, -25, 10, 10, 5,
	5, -5, -10, 0, 0, -10, -5, 5,
	0, 0, 0, 25, 25, 0, 0, 0,
	5, 5, 10, 27, 27, 10, 5, 5,
	10, 10, 20, 30, 30, 20, 10, 10,
	50, 50, 50, 50, 50, 50, 50, 50,
	0, 0, 0, 0, 0, 0, 0, 0,
	},
	{-50, -40, -20, -30, -30, -20, -40, -50,
	-40, -20, 0, 5, 5, 0, -20, -40,
	-30, 5, 10, 15, 15, 10, 5, -30,
	-30, 0, 15, 20, 20, 15, 0, -30,
	-30, 5, 15, 20, 20, 15, 5, -30,
	-30, 0, 10, 15, 15, 10, 0, -30,
	-40, -20, 0, 0, 0, 0, -20, -40,
	-50, -40, -30, -30, -30, -30, -40, -50,
	},
	{-20, -10, -40, -10, -10, -40, -10, -20,
	-10, 5, 0, 0, 0, 0, 5, -10,
	-10, 10, 10, 10, 10, 10, 10, -10,
	-10, 0, 10, 10, 10, 10, 0, -10,
	-10, 5, 5, 10, 10, 5, 5, -10,
	-10, 0, 5, 10, 10, 5, 0, -10,
	-10, 0, 0, 0, 0, 0, 0, -10,
	-20, -10, -10, -10, -10, -10, -10, -20,
	},
	{20, 30, 10, 0, 0, 10, 30, 20,
	20, 20, 0, 0, 0, 0, 20, 20,
	-10, -20, -20, -20, -20, -20, -20, -10,
	-20, -30, -30, -40, -40, -30, -30, -20,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30,
	}}
	};

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

private: int get_pst_val(Board& board) {
	int score = 0;
	bitOp.full_bitscan(board.get_piece_BB(white, pawn));
	return score;
}

/*private: int get_pst_BB(uint64_t& BB, int colour, int pieceNum) {
	
}*/
};

