#pragma once
#include "Board.h"
#include <string>
#include <iostream>


using std::cout;

class BoardDisplay
{
	char pieceLetter[6] = { 'p', 'n', 'b', 'r', 'q', 'k' };
	BitOperations bitOp;
private: void display_BB(uint64_t BB) {
	int pos;
	int sqr;
	for (int rank = 7; rank >= 0; rank--) {
		for (int file = 7; file >= 0; file++) {
			pos = rank * 8 + file;
			sqr = (BB >> pos) & 1U;
			cout << sqr;
		}
		cout << "\n";
	}
	cout << "\n";
}

public: void display_seperate(Board board) {
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

private: void display_board_arr(char* boardArr) {
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

public: void display_board(Board board) {
	char boardArr[64] = { 0 };
	for (int i = 2; i < 8; i++) {
		for (int wPiece : bitOp.full_bitscan(board.bitboards[0] & board.bitboards[i])) {
			boardArr[wPiece] = pieceLetter[i - 2] ^ ' '; // capitalise
		}
		for (int wPiece : bitOp.full_bitscan(board.bitboards[1] & board.bitboards[i])) {
			boardArr[wPiece] = pieceLetter[i - 2];
		}
	}
	display_board_arr(boardArr);
}

private: void display_move(Move move) {
	char boardArr[64] = { 0 };
	boardArr[move.startPos] = pieceLetter[move.pieceType - 2];
	cout << move.move_to_lerf() << "\n";
	if (move.pieceCapture != 0) {
		boardArr[move.endPos] = pieceLetter[move.pieceCapture - 2];
	}
	else {
		boardArr[move.endPos] = 'X';
	}
	display_board_arr(boardArr);
}

public: void display_all_moves(list<Move> allMoves) {
	for (Move const &move : allMoves) {
		display_move(move);
		cout << "\n";
	}
}
};

