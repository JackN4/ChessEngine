#pragma once
#include "Constants.h"
#include <stdint.h>
#include <string>
#include <iostream>

using std::cout;
using std::string;
using N::Piece;
const uint64_t notAFile = 0xfefefefefefefefe;
const uint64_t notHFile = 0x7f7f7f7f7f7f7f7f;
const uint64_t notGHFile = 0x3f3f3f3f3f3f3f3f;
const uint64_t notABFile = 0xfcfcfcfcfcfcfcfc;

class Lookups
{
public: uint64_t slideMoves[64][8]; //64 squares {7,8,9,1,-7,-8,-9,-1}  -7 not working
public:	uint64_t pawnAttacks[64][2];
public:	uint64_t pawnQuiet[64][2];
public:	uint64_t kingMoves[64];
public:	uint64_t knightMoves[64];

public: Lookups() {
	make_pawn_attacks_lookup();
	make_pawn_quiet_lookup();
	make_slide_moves_lookup();
	make_knight_lookup();
	make_king_lookup();
}
private: void display_BB(uint64_t BB) {
	int pos;
	int sqr;
	for (int rank = 7; rank >= 0; rank--) {
		for (int file = 0; file <= 7; file++) {
			pos = rank * 8 + file;
			sqr = (BB >> pos) & 1U;
			cout << sqr;
		}
		cout << "\n";
	}
	cout << "\n";
}
/*private: void debug() {
	for (int i = 0; i < 64; i++) {
		cout << "i:" << i << "\n";
		display_BB(knightMoves[i]);
	}
}*/
private:
	void make_king_lookup() {
		uint64_t movesBB;
		for (int i = 0; i < 64; i++) {
			movesBB = 0;
			int moves[8] = { 7,8,9,1,-7,-8,-9,-1 };
			for (int move : moves) {
				movesBB |= 1ULL << (i+move);
			}
			if(i % 8 == 0){
				movesBB &= notHFile;
			}
			if (i % 8 == 7) {
				movesBB &= notAFile;
			}
			kingMoves[i] = movesBB;
		}
	}

private:
	void make_knight_lookup() {
		uint64_t movesBB;		
		int pos;
		for (int i = 0; i < 64; i++) {
			int moves[8] = { 17,10,-6, -15, -17, -10, 6 ,15 };
			movesBB = 0;
			for (int move : moves) {
				pos = i + move;
				movesBB |= 1ULL << pos;
			}
			if (i % 8 == 0){
				movesBB &= notGHFile;
			}
			else if (i % 8 == 1) {
				movesBB &= notHFile;
			}
			else if (i % 8 == 6) {
				movesBB &= notAFile;
			}
			else if (i % 8 == 7) {
				movesBB &= notABFile;
			}
			knightMoves[i] = movesBB;
		}
	}


private:
	void make_pawn_attacks_lookup() {// works
		uint64_t attacks;
		int pos;
		for (int i = 0; i < 64; i++) {
			attacks = 0;
			pos = i + 7;
			if (pos % 8 != 7) {
				attacks |= 1ULL << pos;
			}
			pos = i + 9;
			if (pos % 8 != 0) {
				attacks |= 1ULL << pos;
			}
			pawnAttacks[i][0] = attacks;
			attacks = 0;
			pos = i - 7;
			if (pos % 8 != 0) {
				attacks |= 1ULL << pos;
			}
			pos = i - 9;
			if (pos % 8 != 7) {
				attacks |= 1ULL << pos;
			}
			pawnAttacks[i][1] = attacks;
		}
	}

private:
	void make_pawn_quiet_lookup() { //works
		uint64_t moves;
		for (int i = 0; i < 64; i++) {
			moves = 0; // white
			moves |= 1ULL << (i+8);
			if (i / 8 == 1) {
				moves |= 1ULL << (i+16);
			}
			pawnQuiet[i][0] = moves;
			moves = 0; //black
			moves |= 1ULL << (i-8);
			if (i / 8 == 6) {
				moves |= 1ULL << (i-16);
			}
			pawnQuiet[i][1] = moves;
		}
	}

private:
	void make_slide_moves_lookup() { // works
		int moveDir[8] = { 7, 8, 9, 1, -7, -8, -9, -1 };
		int xDirs[8] = { -1,0,1,1,1,0,-1,-1 };
		for (int i = 0; i < 64; i++) {
			for (int j = 0; j < 8; j++) {
				uint64_t BB = 0;
				int move = moveDir[j];
				int xDir = xDirs[j];
				int newPos = i;
				for (int count = 1; count < 8; count++) {
					newPos += move;
					if ((newPos < 64 && newPos >= 0) && !(xDir > 0 && newPos % 8 == 0) && !(xDir < 0 && newPos % 8 == 7)) {
						BB |= (1ULL << newPos);
					}
					else {
						break;
					}
				}
				slideMoves[i][j] = BB;
			}
		}
	}

private: void make_king_pawn_attacks_lookup() {

}
};

