#pragma once
#include "MoveCreator.h"
#include "BoardDisplay.h"
#include <chrono>
using namespace std::chrono;

class Perft
{
public: int calculate_perft_bulk(Board board, int depth, bool debug) {
	MoveCreator moveGen = MoveCreator(board);
	list<Move> moves = moveGen.get_all_moves();
	BoardDisplay display;
	int total = 0;
	int movesNum;
	int time1, time2;
	time1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	string lerf;
	for (Move move : moves) {
		lerf = move.move_to_lerf();
		if (depth == 1) {
			movesNum = 1;
		}
		else {
			moveGen.board.make_move(move);
			movesNum = calculate_moves_bulk(moveGen, depth - 1);
			moveGen.board.unmake_move(move);
		}
		total += movesNum;
		cout << lerf << ": " << movesNum << "\n";
		if (debug) {
			display.display_board(board);
		}
	}
	time2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	float secondsTotal = ((float)(time2 - time1))/1000;
	float nodesSecond = total / secondsTotal;
	cout << "total: " << total << "\n";
	cout << "nodes per sceond: " << nodesSecond << "\n";
	return total;
}


private: int calculate_moves_bulk(MoveCreator &moveGen, int depth) {
	list<Move> moves = moveGen.get_all_moves();
	if (depth == 1) {
		return moves.size();
	}
	int total = 0;
	for (Move move : moves) {
		moveGen.board.make_move(move);
		total += calculate_moves_bulk(moveGen, depth - 1);
		moveGen.board.unmake_move(move);
	}
	return total;
}

public: int calculate_perft(Board board, int depth, bool debug) {
	MoveCreator moveGen = MoveCreator(board);
	list<Move> moves = moveGen.get_all_moves();
	BoardDisplay display;
	int total = 0;
	int movesNum;
	time_t seconds1, seconds2;
	seconds1 = time(NULL);
	for (Move move : moves) {
		moveGen.board.make_move(move);
		movesNum = calculate_moves(moveGen, depth - 1);
		moveGen.board.unmake_move(move);
		total += movesNum;
		cout << move.move_to_lerf() << ": " << movesNum << "\n";
		if (debug) {
			display.display_board(board);
		}
	}
	seconds2 = time(NULL);
	float secondsTotal = seconds2 - seconds1;
	float nodesSecond = total / secondsTotal;
	cout << "total: " << total << "\n";
	cout << "nodes per sceond: " << nodesSecond << "\n";
	return total;
}


private: int calculate_moves(MoveCreator& moveGen, int depth) {
	if (depth == 0) {
		return 1;
	}
	list<Move> moves = moveGen.get_all_moves();
	int total = 0;
	for (Move move : moves) {
		moveGen.board.make_move(move);
		total += calculate_moves(moveGen, depth - 1);
		moveGen.board.unmake_move(move);
	}
	return total;
}

};

