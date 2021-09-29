#pragma once
#include "Board.h"
#include <string>
#include <iostream>


using std::cout;
using std::vector;

// This class displays a board a few different ways to help with debugging

class BoardDisplay
{
	char pieceLetter[6] = { 'p', 'n', 'b', 'r', 'q', 'k' }; //The characters used to represent each piece
	BitOperations bitOp; //Creates bit operations class
private: void display_BB(uint64_t BB) { //Displays 1 bitboard
	int pos;
	int sqr;
	for (int rank = 7; rank >= 0; rank--) { //Iterates thought each bit of the bitboard
		for (int file = 0; file <= 7; file++) {
			pos = rank * 8 + file; //Current pos
			sqr = (BB >> pos) & 1U; //Gets bit
			cout << sqr; //Outputs bit
		}
		cout << "\n";//Moves to next row
	}
	cout << "\n";
}

public: void display_seperate(Board board) { //Displays all bitboards in a board seperately
	cout << "All pieces" << "\n";
	display_BB(board.get_all_BB());
	cout << "White" << "\n";
	display_BB(board.bitboards[white]);
	cout << "Black" << "\n";
	display_BB(board.bitboards[black]);
	cout << "Pawns" << "\n";
	display_BB(board.bitboards[pawn]);
	cout << "Knights" << "\n";
	display_BB(board.bitboards[knight]);
	cout << "Bishop" << "\n";
	display_BB(board.bitboards[bishop]);
	cout << "Rook" << "\n";
	display_BB(board.bitboards[rook]);
	cout << "Queen" << "\n";
	display_BB(board.bitboards[queen]);
	cout << "King" << "\n";
	display_BB(board.bitboards[king]);
}

private: void display_board_arr(char* boardArr) { //Outputs whole board using characters for letters given the array of chars
	char sqr;
	for (int rank = 7; rank >= 0; rank--) {
		for (int file = 0; file <= 7; file++) {
			sqr = boardArr[8 * rank + file];
			if (sqr == 0) {
				cout << '.';
			}
			else {
				cout << sqr;
			}
		}
		cout << "\n";
	}
}

public: void display_board(Board board) { //Displays the board
	char boardArr[64] = { 0 };
	for (int i = 2; i < 8; i++) { //Iterates through each piece
		pair<int*, int> fullScan = bitOp.full_bitscan(board.bitboards[0] & board.bitboards[i]); //Find white pieces
		for (int j = 0; j < fullScan.second; j++) {
			boardArr[fullScan.first[j]] = pieceLetter[i - 2] ^ ' '; // Adds the capatilised piece type to array
		}
		fullScan = bitOp.full_bitscan(board.bitboards[1] & board.bitboards[i]); //Find black piece
		for (int j = 0; j < fullScan.second; j++) {
			boardArr[fullScan.first[j]] = pieceLetter[i - 2]; //Adds the piece type to array
		}
	}
	display_board_arr(boardArr); //Outputs array
}

private: void display_move(Move move) { //Displays a specific move
	char boardArr[64] = { 0 };
	boardArr[move.startPos] = pieceLetter[move.pieceType - 2]; //Puts piece character on startpos
	cout << move.move_to_lerf() << "\n";
	if (move.pieceCapture != 0) {
		boardArr[move.endPos] = pieceLetter[move.pieceCapture - 2];
	}
	else {
		boardArr[move.endPos] = 'X'; //Puts X on end position
	}
	display_board_arr(boardArr); //Outputs array
}

public: void display_all_moves(vector<Move> allMoves) { //Displays multiple moves using display_move function
	for (Move const &move : allMoves) {
		display_move(move);
		cout << "\n";
	}
}
};

