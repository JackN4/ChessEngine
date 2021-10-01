#pragma once
#include "MoveCreator.h"
#include "BoardDisplay.h"
#include "PerftTable.h"
#include <chrono>
#include <iostream>
#include <fstream>
using namespace std::chrono;

//This file is used to test the accuracy and speed of move generation and make moves
class Perft
{
	
public: uint64_t calculate_perft_bulk(Board board, int depth, bool debug, bool print = true) { //Calculates perft with bulk counting (so it doesn't make and unmake all the moves)
	MoveCreator moveGen = MoveCreator(board);    //Generates all moves for the current board
	vector<Move> moves = moveGen.get_all_moves();
	BoardDisplay display;
	uint64_t total = 0;
	int movesNum;
	int time1, time2;
	time1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	string lerf;
	for (Move &move : moves) {
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
		if (print) {
			cout << lerf << ": " << movesNum << "\n";
		}
		if (debug) {
			display.display_board(board);
		}
	}
	if (print) {
		time2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		float secondsTotal = ((float)(time2 - time1)) / 1000;
		float nodesSecond = total / secondsTotal;
		cout << "total: " << total << "\n";
		cout << "nodes per sceond: " << nodesSecond << "\n";
	}
	return total;
}


private: uint64_t calculate_moves_bulk(MoveCreator &moveGen, int depth) {
	vector<Move> moves = moveGen.get_all_moves();
	if (depth == 1) {
		return moves.size();
	}
	uint64_t total = 0;
	for (Move &move : moves) {
		moveGen.board.make_move(move);
		total += calculate_moves_bulk(moveGen, depth - 1);
		moveGen.board.unmake_move(move);
	}
	return total;
}

public: uint64_t calculate_perft(Board board, int depth, bool debug) {
	MoveCreator moveGen = MoveCreator(board);
	
	vector<Move> moves = moveGen.get_all_moves();
	BoardDisplay display;
	uint64_t total = 0;
	int movesNum;
	int time1, time2;
	time1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	for (Move &move : moves) {
		moveGen.board.make_move(move);
		movesNum = calculate_moves(moveGen, depth - 1);
		moveGen.board.unmake_move(move);
		total += movesNum;
		cout << move.move_to_lerf() << ": " << movesNum << "\n";
		if (debug) {
			display.display_board(board);
		}
	}
	time2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	float secondsTotal = ((float)(time2 - time1)) / 1000;
	float nodesSecond = total / secondsTotal;
	cout << "total: " << total << "\n";
	cout << "nodes per sceond: " << nodesSecond << "\n";
	return total;
}


private: uint64_t calculate_moves(MoveCreator &moveGen, int depth) {
	if (depth == 0) {
		return 1;
	}
	vector<Move> moves = moveGen.get_all_moves();
	uint64_t total = 0;
	for (Move &move : moves) {
		moveGen.board.make_move(move);
		total += calculate_moves(moveGen, depth - 1);
		moveGen.board.unmake_move(move);
	}
	return total;
}

/*public: void perft_file(string fileName) {
	std::ifstream file(fileName);
	string line;
	int spaceCount = 0, depth, expected, actual;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		Board board;
		string fen;
		string section;
		while (std::getline(iss, section, ' ')) {
			spaceCount++;
			if (spaceCount < 6) {
				fen += section;
			}
			else if (spaceCount == 6) {
				depth = std::stoi(section);
			}
			else if (spaceCount == 7) {
				expected = std::stoi(section);
			}
		}
		board.create_from_FEN(fen);
		cout << "FEN: " << fen << "\n";
		actual = calculate_perft_bulk(board, depth, false, false);
		if (actual == expected) {
			cout << "success!" << "\n";
		}
		else {
			cout << "failed:" << "\n";
			cout << "expected" << expected << "\n";
			cout << "actual" << actual << "\n";
		}
		cout << "\n";
	}
}*/

public: uint64_t calculate_perft_TT(Board board, int depth, bool debug, bool print = true) {
	MoveCreator moveGen = MoveCreator(board);
	vector<Move> moves = moveGen.get_all_moves();
	PerftTable table = PerftTable();
	uint64_t total = 0;
	int movesNum;
	int time1, time2;
	time1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	string lerf;
	for (Move& move : moves) {
		lerf = move.move_to_lerf();
		if (depth == 1) {
			movesNum = 1;
		}
		else {
			moveGen.board.make_move(move);
			pair<bool, uint64_t> count = table.get_count(moveGen.board.zobristKey, depth);
			if (count.first) {
				movesNum = count.second;
			}
			else {
				movesNum = calculate_moves_TT(moveGen, depth - 1, table);
			}
			moveGen.board.unmake_move(move);
		}
		total += movesNum;
		if (print) {
			cout << lerf << ": " << movesNum << "\n";
		}
	}
	if (print) {
		time2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		float secondsTotal = ((float)(time2 - time1)) / 1000;
		float nodesSecond = total / secondsTotal;
		cout << "total: " << total << "\n";
		cout << "nodes per sceond: " << nodesSecond << "\n";
	}
	table.delete_table();
	return total;
}


private: uint64_t calculate_moves_TT(MoveCreator& moveGen, int depth, PerftTable& table) {
	pair <bool, int> entry = table.get_count(moveGen.board.zobristKey, depth);
	if (entry.first) {
 		return entry.second;
	}
	vector<Move> moves = moveGen.get_all_moves();
	uint64_t total = 0;
	uint64_t oldKey = moveGen.board.zobristKey;
	if (depth == 1) {
		total = moves.size();
	}
	else{
		for (Move& move : moves) {
			moveGen.board.make_move(move);
			total += calculate_moves_TT(moveGen, depth - 1, table);
			moveGen.board.unmake_move(move);
		}
	}
	table.add(EntryPerft(moveGen.board.zobristKey, total, depth)); // dont need to do this if found
	return total;
}
};

