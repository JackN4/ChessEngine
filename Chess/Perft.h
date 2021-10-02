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
	
public: uint64_t calculate_perft_bulk(Board board, int depth, bool debug, bool print = true) { //Starts perft with bulk counting (so it doesn't make and unmake all the moves)
	MoveCreator moveGen = MoveCreator(board);    
	vector<Move> moves = moveGen.get_all_moves(); //Generates all moves for the current board
	BoardDisplay display;
	uint64_t total = 0; 
	int movesNum;
	int time1, time2; //Start time and end time used to calculate speed of move generation
	time1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	string lerf;
	for (Move &move : moves) { //Iterates through moves
		lerf = move.move_to_lerf(); 
		if (depth == 1) { 
			movesNum = 1;
		}
		else {
			moveGen.board.make_move(move); //Makes move
			movesNum = calculate_moves_bulk(moveGen, depth - 1); //Recursively applies perft bulk
			moveGen.board.unmake_move(move); //Unmakes move
		}
		total += movesNum; //Adds number of moves to tal
		if (print) {
			cout << lerf << ": " << movesNum << "\n"; //Prints out how many further moves are generate recursively from this one
		}
		if (debug) {
			display.display_board(board);
		}
	}
	if (print) {
		time2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		float secondsTotal = ((float)(time2 - time1)) / 1000; //Gets number of seconds
		float nodesSecond = total / secondsTotal; //Gets move generated per second
		cout << "total: " << total << "\n"; //Prints total moves
		cout << "nodes per sceond: " << nodesSecond << "\n"; //Prints moves per second
	}
	return total;
}


private: uint64_t calculate_moves_bulk(MoveCreator &moveGen, int depth) { //Recursively called function to perform bulk perft
	vector<Move> moves = moveGen.get_all_moves();
	if (depth == 1) { //If depth is 1 just use number of moves
		return moves.size();
	}
	uint64_t total = 0;
	for (Move &move : moves) {
		moveGen.board.make_move(move); //Makes move
		total += calculate_moves_bulk(moveGen, depth - 1); //Recursively calls function
		moveGen.board.unmake_move(move); //Unmakes move
	}
	return total; //Returns total number of moves
}

public: uint64_t calculate_perft(Board board, int depth, bool debug) { //Starts perft with no bulk counting
	MoveCreator moveGen = MoveCreator(board);
	vector<Move> moves = moveGen.get_all_moves(); //Get all moves
	BoardDisplay display;
	uint64_t total = 0;
	int movesNum;
	int time1, time2;
	time1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	for (Move &move : moves) { //Iterates through moves
		moveGen.board.make_move(move); //Makes move
		movesNum = calculate_moves(moveGen, depth - 1); //Recursively calls no bulk counting perft
		moveGen.board.unmake_move(move); //Unmakes move
		total += movesNum; 
		cout << move.move_to_lerf() << ": " << movesNum << "\n"; //Prints num of move
		if (debug) {
			display.display_board(board);
		}
	}
	time2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	float secondsTotal = ((float)(time2 - time1)) / 1000;
	float nodesSecond = total / secondsTotal; //Gets moves per second
	cout << "total: " << total << "\n"; //Prints total moves
	cout << "nodes per sceond: " << nodesSecond << "\n"; //Prints nodes per second
	return total;
}


private: uint64_t calculate_moves(MoveCreator &moveGen, int depth) { //Recursively called perft non-bulk
	if (depth == 0) {
		return 1; //When depth is 0 just return the 1 for the current board
	}
	vector<Move> moves = moveGen.get_all_moves(); //Gets all moves
	uint64_t total = 0;
	for (Move &move : moves) { //Iterates through moves
		moveGen.board.make_move(move); //Makes move
		total += calculate_moves(moveGen, depth - 1); //Recursively calls function
		moveGen.board.unmake_move(move); //Unmakes move
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

public: uint64_t calculate_perft_TT(Board board, int depth, bool debug, bool print = true) { //Starts perft with bulk counting 
	MoveCreator moveGen = MoveCreator(board);
	vector<Move> moves = moveGen.get_all_moves(); //Gets moves
	PerftTable table = PerftTable(); //Creates a transposition perft table
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
			moveGen.board.make_move(move); //Make move
			movesNum = calculate_moves_TT(moveGen, depth - 1, table); //call recursive perft TT
			moveGen.board.unmake_move(move); //Unmakes move
		}
		total += movesNum; //Increase total
		if (print) {
			cout << lerf << ": " << movesNum << "\n";
		}
	}
	if (print) { 
		time2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		float secondsTotal = ((float)(time2 - time1)) / 1000;
		float nodesSecond = total / secondsTotal; 
		cout << "total: " << total << "\n"; //Prints total
		cout << "nodes per sceond: " << nodesSecond << "\n"; //Prints moves per second
	}
	table.delete_table(); //Deletes table to prevent memory issus
	return total;
}


private: uint64_t calculate_moves_TT(MoveCreator& moveGen, int depth, PerftTable& table) { //Recursively called perft with transposition table
	pair <bool, int> entry = table.get_count(moveGen.board.zobristKey, depth); //Gets entry from tranposition table
	if (entry.first) { //If entry exists
 		return entry.second; //Return count from entry
	}
	vector<Move> moves = moveGen.get_all_moves(); //Gets all moves
	uint64_t total = 0;
	if (depth == 1) { //If depth is 1 just return the number of possible moves
		total = moves.size();
	}
	else{
		for (Move& move : moves) { //Iterates through moves
			moveGen.board.make_move(move); //Makes move
			total += calculate_moves_TT(moveGen, depth - 1, table); //Recursively calls function
			moveGen.board.unmake_move(move);//Unmakes move
		}
	}
	table.add(EntryPerft(moveGen.board.zobristKey, total, depth)); // dont need to do this if found
	return total;
}
};

