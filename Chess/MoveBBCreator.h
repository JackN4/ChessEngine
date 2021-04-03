#pragma once
#include "Constants.h"
#include "Lookups.h"
#include "BitOperations.h"




//{7,8,9,1,-7,-8,-9,-1}


struct MoveBBCreator {
BitOperations bitOp;
Lookups lookup;

uint64_t get_attacks(uint64_t movesBB, uint64_t oppColourBB) {
	return movesBB & oppColourBB;
}

uint64_t get_quiet(uint64_t movesBB, uint64_t oppColourBB) {
	return movesBB & ~oppColourBB;
}

int get_blocker_pos(uint64_t blockersBB, int directionIdx) {
	if (directionIdx < 4) {
		return bitOp.lsb_bitscan(blockersBB);
	}
	else {
		return bitOp.msb_bitscan(blockersBB);
	}
}
uint64_t get_sliding_BB(uint64_t allBB, uint64_t ownColourBB, int pos, int directionIdx) {
	uint64_t rayBB = lookup.slideMoves[pos][directionIdx];
	uint64_t blockersBB = rayBB & allBB;
	if (blockersBB == 0) {
		return rayBB;
	}
	int blockerPos = get_blocker_pos(blockersBB, directionIdx);
	return (rayBB ^ lookup.slideMoves[blockerPos][directionIdx]) & ~ownColourBB;
}



uint64_t get_queen_BB(int pos, uint64_t allBB, uint64_t ownColourBB) {
	uint64_t movesBB = 0;
	for (int i = 0; i < 8; i++) {
		movesBB |= get_sliding_BB(allBB, ownColourBB, pos, i);
	}
	return movesBB;
}

uint64_t get_bishop_BB(int pos, uint64_t allBB, uint64_t ownColourBB) {
	int movesIdx[4] = { 0,2,4,6 };
	uint64_t movesBB = 0;
	for (int move : movesIdx) {
		movesBB |= get_sliding_BB(allBB, ownColourBB, pos, move);
	}
	return movesBB;
}

uint64_t get_rook_BB(int pos, uint64_t allBB, uint64_t ownColourBB) {
	int movesIdx[4] = { 1,3,5,7 };
	uint64_t movesBB = 0;
	for (int move : movesIdx) {
		movesBB |= get_sliding_BB(allBB, ownColourBB, pos, move);
	}
	return movesBB;
}

uint64_t get_king_BB(int pos, uint64_t ownColourBB) {
	return lookup.kingMoves[pos] & ~ownColourBB;
}

bool can_queenside_castle(uint64_t allBB, int colour) {
	int rankLeft = colour * 56;
	for (int file = 1; file < 4; file++) {
		if (((allBB >> (rankLeft + file)) & 1ULL) == 1) {
			return false;
		}
	}
	return true;
}

bool can_kingside_castle(uint64_t allBB, int colour) {
	int rankLeft = colour * 56;
	for (int file = 4; file < 6; file++) {
		if (((allBB >> (rankLeft + file)) & 1ULL) == 1) {
			return false;
		}
	}
	return true;
}

uint64_t get_knight_BB(int pos, uint64_t ownColourBB, uint64_t allBB) { //addded all so would have same arguments should probably fix
	return lookup.knightMoves[pos] & ~ownColourBB;
}

uint64_t get_knight_BB_empty(int pos) { //addded all so would have same arguments should probably fix
	return lookup.knightMoves[pos];
}



uint64_t get_pawn_quiet_BB(int pos, uint64_t allBB, int colour) {
	return lookup.pawnQuiet[pos][colour] & ~allBB;
}

uint64_t get_pawn_attack_BB(int pos, uint64_t oppColourBB, int colour) {
	return lookup.pawnAttacks[pos][colour] & oppColourBB;
}

bool sqr_empty(uint64_t BB,int pos) {
	return ((BB >> pos) & 1ULL) == 0;
}

uint64_t get_king_pawn_attack_BB(int pos, int colour) {
	return lookup.pawnAttacks[pos][colour];
}

bool any_attackers(int* moves, uint64_t attackers, int pos, uint64_t allBB) {
	uint64_t blockersBB;
	int blockerPos;
	for (int i = 0; i < 4; i++) {
		blockersBB = lookup.slideMoves[pos][moves[i]] & allBB;
		if (blockersBB != 0) {
			blockerPos = get_blocker_pos(blockersBB, moves[i]);
			if (((1ULL << blockerPos) & attackers) != 0) {
				return true;
			}
		}
	}
	return false;
}


};

