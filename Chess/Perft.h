#pragma once
#include "MoveCreator.h"
#include "BoardDisplay.h"

class Perft
{
public: int calculate_perft(Board board, int depth) {
	MoveCreator moveGen = MoveCreator(board);
	list<Move> moves = moveGen.get_all_moves();
	BoardDisplay display;
	int total = 0;
	int movesNum;
	for (Move move : moves) {
		if (depth == 1) {
			movesNum = 1;
		}
		else {
			moveGen.board.make_move(move);
			movesNum = calculate_moves(moveGen, depth - 1);
			moveGen.board.unmake_move(move);
		}
		total += movesNum;
		cout << move.move_to_lerf() << ": " << movesNum << "\n";
	}
	cout << "total: " << total << "\n";
	return total;
}


private: int calculate_moves(MoveCreator &moveGen, int depth) {
	list<Move> moves = moveGen.get_all_moves();
	if (depth == 1) {
		return moves.size();
	}
	int total = 0;
	for (Move move : moves) {
		moveGen.board.make_move(move);
		total += calculate_moves(moveGen, depth - 1);
		moveGen.board.unmake_move(move);
	}
	return total;
}
};

