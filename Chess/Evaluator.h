#pragma once
#include "Board.h"
#include "BitOperations.h"

//This file evaluates the current position on the board

class Evaluator
{
	BitOperations bitOp;
public:int vals[5] = { 100, 320, 330, 500, 900 }; //Piece values
	int PSTs[2][4][64] = { { //These tables give scores dependent on where a specific piece of a specific colour is on the board
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


public: int eval(Board& board) { //Function is given a board and returns an evaluation
	int score = 0;
	for (int i = 0; i < 5; i++) {
		score += get_pop_count(board, 0, i + 2) * vals[i]; //Adds values of all white pieces
		score -= get_pop_count(board, 1, i + 2) * vals[i]; //Subtracts value of all black pieces
	}
	score += get_pst_val(board)/2; //Gets score based on piece positions
	if (board.toMove == white) { //Returns score relative to current player
		return score;
	}
	else {
		return -score;
	}
}

private: int get_pop_count(Board& board, int colour, int piece) { //Gets number of specific piece of specific colour on the board
	return bitOp.pop_count(board.bitboards[colour] & board.bitboards[piece]);
}

private: int get_pst_val(Board& board) { //Sums all the position based scores
	int score = 0;
	score += get_pst_BB(board, white, pawn,0);
	score -= get_pst_BB(board, black, pawn,0);
	score += get_pst_BB(board, white, knight,1);
	score -= get_pst_BB(board, black, knight,1);
	score += get_pst_BB(board, white, bishop,2);
	score -= get_pst_BB(board, black, bishop,2);
	score += get_pst_BB(board, white, king, 3);
	score -= get_pst_BB(board, black, king, 3);
	return score;
}

private: int get_pst_BB(Board& board, int colour, Piece piece, int pstNum) {
	int score = 0;
	pair<int*, int> fullScan = bitOp.full_bitscan(board.get_piece_BB(colour, piece)); //Gets the positions of all the piece
	int pos;
	for (int i = 0; i < fullScan.second; i++) { //iterates through all the pieces
		pos = fullScan.first[i];
		score += PSTs[colour][pstNum][pos]; //Adds their value to the score
	}
	return score;
}

};

