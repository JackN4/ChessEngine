#pragma once
#include "Board.h"
#include "MoveCreator.h"
#include "Evaluator.h"
#include <limits>
#include <stdint.h>

 

class Search
{
	int depthStart = 6;
	int max = 100000;
	Evaluator evaluator;

public: Move negamax_start(Board &board) {
	MoveCreator moveGen = MoveCreator(board);
	Move bestMove;
	int bestScore = std::numeric_limits<int>::min();
	int score;
	for (Move& move : moveGen.get_all_moves()) {
		moveGen.board.make_move(move);
		score = -(negamax(moveGen, depthStart, -max, max));
		moveGen.board.unmake_move(move);
		if(score > bestScore){
			bestScore = score;
			bestMove = move;
		}
	}
	cout << "eval:" << bestScore << "\n";
	return bestMove;
}

private: int negamax(MoveCreator &moveGen, int depth, int alpha, int beta) { //add checkmates
	if (depth == 0) {
		if (moveGen.board.toMove == white) {
			return evaluator.eval(moveGen.board);
		}
		else {
			return -(evaluator.eval(moveGen.board));
		}
	}
	int score;
	vector<Move> allMoves = moveGen.get_all_moves();
	if (allMoves.size() == 0) {
		if (moveGen.checkers != 0) {;
			return -max;
		}
		else {
			return 0;
		}
	}
	for (Move& move : allMoves) {
		moveGen.board.make_move(move);
		score = -(negamax(moveGen, depth - 1, -beta, -alpha));
		moveGen.board.unmake_move(move);
		if (score >= beta) {
			return beta;
		}
		if (score > alpha) {
			alpha = score;
		}
	}

	return alpha;
}
};

