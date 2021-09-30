#pragma once
#include "Constants.h"
#include "Lookups.h"
#include "BitOperations.h"

using std::pair;

//This file creates any bitboards(BBs) or performs operations on BBs while generating moves


//{7,8,9,1,-7,-8,-9,-1}


struct MoveBBCreator {
BitOperations bitOp; 
Lookups lookup;

uint64_t get_attacks(uint64_t movesBB, uint64_t oppColourBB) {
	return movesBB & oppColourBB; //Gets attacks by finding the intersect of possible moves and opponent pieces
}

uint64_t get_quiet(uint64_t movesBB, uint64_t oppColourBB) {
	return movesBB & ~oppColourBB; //Gets non-captures by finding the intersect of possible moves and non-opponent pieces
}

int get_blocker_pos(uint64_t blockersBB, int directionIdx) { //Finds the position of a piece blocking a sliding move
	if (directionIdx < 4) { //If the direction is is positive the the blocker will be the lowest bit
		return bitOp.lsb_bitscan(blockersBB);
	}
	else { //Otherwise it will be the highest bit
		return bitOp.msb_bitscan(blockersBB);
	}
}
uint64_t get_sliding_BB(uint64_t allBB, uint64_t ownColourBB, int pos, int directionIdx) { //Gets sliding ray BB
	uint64_t rayBB = lookup.slideMoves[pos][directionIdx]; //Gets ray from lookup table
	uint64_t blockersBB = rayBB & allBB; //Finds any blockers
	if (blockersBB == 0) { //No blockers
		return rayBB;
	}
	int blockerPos = get_blocker_pos(blockersBB, directionIdx); //Gets the first blocker
	return (rayBB ^ lookup.slideMoves[blockerPos][directionIdx]) & ~ownColourBB; //Finds the possible moves before the blocker
}



uint64_t get_queen_BB(int pos, uint64_t allBB, uint64_t ownColourBB) { //Gets queen moves by finding sliding moves in all 8 direction
	uint64_t movesBB = 0;
	for (int i = 0; i < 8; i++) {
		movesBB |= get_sliding_BB(allBB, ownColourBB, pos, i);
	}
	return movesBB;
}

uint64_t get_bishop_BB(int pos, uint64_t allBB, uint64_t ownColourBB) { //Gets bishop moves by finding diagonal sliding moves
	int movesIdx[4] = { 0,2,4,6 };
	uint64_t movesBB = 0;
	for (int move : movesIdx) {
		movesBB |= get_sliding_BB(allBB, ownColourBB, pos, move);
	}
	return movesBB;
}

uint64_t get_rook_BB(int pos, uint64_t allBB, uint64_t ownColourBB) { //Gets rook moves by finding orthogonal sliding moves
	int movesIdx[4] = { 1,3,5,7 };
	uint64_t movesBB = 0;
	for (int move : movesIdx) {
		movesBB |= get_sliding_BB(allBB, ownColourBB, pos, move);
	}
	return movesBB;
}

uint64_t get_king_BB(int pos, uint64_t ownColourBB) { //Finds king moves using lookup and not being able to land on own piece
	return lookup.kingMoves[pos] & ~ownColourBB;
}

bool can_queenside_castle(uint64_t allBB, int colour) { //Checks if queenside castle is possible
	int rankLeft = colour * 56;
	for (int file = 1; file < 4; file++) { //Checks if each square between king and rook is empty
		if (((allBB >> (rankLeft + file)) & 1ULL) == 1) {
			return false;
		}
	}
	return true;
}

bool can_kingside_castle(uint64_t allBB, int colour) { //Checks if kingside castle is possible
	int rankLeft = colour * 56;
	for (int file = 4; file < 6; file++) { //Checks if each square between king and rook is empty
		if (((allBB >> (rankLeft + file)) & 1ULL) == 1) {
			return false;
		}
	}
	return true;
}

uint64_t get_knight_BB(int pos, uint64_t allBB, uint64_t ownColourBB) { //Returns possible knight moves
	return lookup.knightMoves[pos] & ~ownColourBB;
}

uint64_t get_knight_BB_empty(int pos) { //Get knight moves on an empty board
	return lookup.knightMoves[pos];
}


pair<uint64_t, uint64_t> get_white_pawn_quiet(uint64_t pawnBB, uint64_t availableBB) { //Gets white pawn non captures, returns 2 BBs, one for a single move forward, second for 2 squares forward
	pair<uint64_t, uint64_t> moves;
	moves.first = (pawnBB << 8) & availableBB; //Shifts all pawn 8 bits to move forward a rown
	moves.second = ((moves.first & lookup.rank3) << 8) & availableBB; //Any pawns on the 3rd rank can move another square forwards
	return moves;
}

pair<uint64_t, uint64_t> get_white_pawn_attack(uint64_t pawnBB, uint64_t oppBB) { //Gets white pawn capture, 2 BBs: Left diagonal captures, Right diagonal captures
	pair<uint64_t, uint64_t> moves;
	moves.first = ((pawnBB << 7) & lookup.notHFile) & oppBB; //Left diagonal. Uses the intersect with the oponents pieces (oppBB) to find attacks
	moves.second = ((pawnBB << 9) & lookup.notAFile) & oppBB; //Right diagonal
	return moves;
}

pair<uint64_t, uint64_t> get_black_pawn_quiet(uint64_t pawnBB, uint64_t availableBB) { //Same as white pawns but other direction
	pair<uint64_t, uint64_t> moves;
	moves.first = (pawnBB >> 8) & availableBB;
	moves.second = ((moves.first & lookup.rank6) >> 8) & availableBB;
	return moves;
}

pair<uint64_t, uint64_t> get_black_pawn_attack(uint64_t pawnBB, uint64_t oppBB) { //Same as white pawns but other direction
	pair<uint64_t, uint64_t> moves;
	moves.first = ((pawnBB >> 9) & lookup.notHFile) & oppBB;
	moves.second = ((pawnBB >> 7) & lookup.notAFile) & oppBB;
	return moves;
}

pair<uint64_t, uint64_t> get_promo_BB(uint64_t movesBB) { //Checks if pawns are being promoted by a move, returns 1 BB for unpromoted pawns and 1 for promoted pawns
	pair<uint64_t, uint64_t> BBs;
	BBs.second = movesBB & lookup.rank18; //Any pawn on the 1st or 8th rank will be promoted
	BBs.first = movesBB ^ BBs.second;
	return BBs;
}

bool sqr_empty(uint64_t BB,int pos) { //Checks if square is empty
	return ((BB >> pos) & 1ULL) == 0;
}

uint64_t get_king_pawn_attack_BB(int pos, int colour) { //Checks if pawns attacking king
	return lookup.pawnAttacks[pos][colour];
}

bool any_attackers(int* moves, uint64_t attackers, int pos, uint64_t allBB) { //Checks if a specific square has any attackers
	uint64_t blockersBB;
	int blockerPos;
	for (int i = 0; i < 4; i++) {//Checks 4 different directions
		blockersBB = lookup.slideMoves[pos][moves[i]] & allBB; //Gets if any pieces are in that direction
		if (blockersBB != 0) {
			blockerPos = get_blocker_pos(blockersBB, moves[i]);
			if (((1ULL << blockerPos) & attackers) != 0) { //If the first blocker is a piece that can attack in that direction then there is an attacker
				return true;
			}
		}
	}
	return false; //No attacker found
}

};

