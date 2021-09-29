#pragma once
#include "Constants.h"
#include <stdint.h>
#include <string>
#include <iostream>

//This file creates lookup tables that are used when generating moves
using std::cout;
using std::string;
using N::Piece;


const uint64_t notGHFile = 0x3f3f3f3f3f3f3f3f; //all bits are 1 but G and H file - used as mask
const uint64_t notABFile = 0xfcfcfcfcfcfcfcfc; //all bits are 1 but A and B file - used as mask

class Lookups
{
public:	const uint64_t notAFile = 0xfefefefefefefefe; //Mask for not the A file
public:	const uint64_t notHFile = 0x7f7f7f7f7f7f7f7f; //Mask for not the H file
public: const uint64_t rank3 = 0xff0000; //Mask for just 3rd rank
public: const uint64_t rank6 = 0xff0000000000; //Mask for just 6th rank
public: const uint64_t rank18 = 0xff000000000000ff; //Mask for 1st and 8th rank
	//Sets up the arrays for the lookup bitboards
public: uint64_t slideMoves[64][8]; //64 squares {7,8,9,1,-7,-8,-9,-1}  -7 
public:	uint64_t pawnAttacks[64][2]; //64 square, 2 colours
public:	uint64_t kingMoves[64];
public:	uint64_t knightMoves[64];

public: Lookups() { //Creates all lookups
	make_slide_moves_lookup();
	make_knight_lookup();
	make_king_lookup();
	make_pawn_attacks_lookup();
}

private:
	void make_king_lookup() { //Makes lookups for king
		uint64_t movesBB; 
		int pos;
		for (int i = 0; i < 64; i++) { //Iterates through each square
			movesBB = 0;
			int moves[8] = { 7,8,9,1,-7,-8,-9,-1 }; //King can move in all 8 directions
			for (int move : moves) { //Iterates all moves
				pos = i + move; //Gets position after move
				if (pos >= 0 && pos < 64) { //Checks move is on board
					movesBB |= 1ULL << pos; //Adds move to lookup BB
				}
			}
			if(i % 8 == 0){//These stop any moves that would wrap round to other side of board
				movesBB &= notHFile; 
			}
			if (i % 8 == 7) {
				movesBB &= notAFile;
			}
			kingMoves[i] = movesBB; //Adds bitboard to the array for the specific square
		}
	}

private:
	void make_knight_lookup() { //Makes lookups for knight
		uint64_t movesBB;		
		int pos;
		for (int i = 0; i < 64; i++) { //Iterates all square
			int moves[8] = { 17,10,-6, -15, -17, -10, 6 ,15 }; //All 8 moves the knight can make
			movesBB = 0;
			for (int move : moves) {
				pos = i + move;//Gets position after move
				if (pos >= 0 && pos < 64) {//Checks move is on board
					movesBB |= 1ULL << pos;//Adds move to lookup BB
				}
			}
			if (i % 8 == 0){//These stop any moves that would wrap round to other side of board
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
			knightMoves[i] = movesBB; //Adds bitboard to the array for the specific square
		}
	}


private:
	void make_pawn_attacks_lookup() {//Creates lookup just for diagonal pawn attacks
		uint64_t attacks;
		int pos;
		for (int i = 0; i < 64; i++) { //Iterates every square
			attacks = 0;
			pos = i + 7; //New pos for white left diagonal attack
			if (pos % 8 != 7) { //Checks move hasnt wrapped round board
				attacks |= 1ULL << pos; //Adds to BB
			}
			pos = i + 9;//new pos for white right diagonal attack
			if (pos % 8 != 0) { //Same as above
				attacks |= 1ULL << pos;
			}
			pawnAttacks[i][0] = attacks; //Adds white moves to lookup
			attacks = 0; //Same process but for black instead of white
			pos = i - 7;
			if (pos >= 0 && pos < 64 && pos % 8 != 0) {
				attacks |= 1ULL << pos;
			}
			pos = i - 9;
			if (pos >= 0 && pos < 64 && pos % 8 != 7) {
				attacks |= 1ULL << pos;
			}
			pawnAttacks[i][1] = attacks;
		}
	}

private:
	void make_slide_moves_lookup() { //Makes moves for sliding pieces (bishop, rook, queen)
		int moveDir[8] = { 7, 8, 9, 1, -7, -8, -9, -1 }; //8 sliding directions
		int xDirs[8] = { -1,0,1,1,1,0,-1,-1 }; //Does the direction move left(-1) or right(1) 
		for (int i = 0; i < 64; i++) { //Iterates every square
			for (int j = 0; j < 8; j++) { //And every direction
				uint64_t BB = 0; 
				int move = moveDir[j]; 
				int xDir = xDirs[j];//Gets direction and x direction
				int newPos = i; 
				for (int count = 1; count < 8; count++) { //The furthest a piece could slide is 8 squares
					newPos += move; //Moves one more move in direction
					if ((newPos < 64 && newPos >= 0) && !(xDir > 0 && newPos % 8 == 0) && !(xDir < 0 && newPos % 8 == 7)) { //Checks if the move has reached the edge of the board
						BB |= (1ULL << newPos); //If it has the whole ray of that move is added to bitboard
					}
					else {
						break;
					}
				}
				slideMoves[i][j] = BB;//Moves added to lookup table
			}
		}
	}
};

