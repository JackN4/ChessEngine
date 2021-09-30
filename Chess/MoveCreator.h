#pragma once
#include <vector>
#include "MoveBBCreator.h"
#include "Board.h"

using std::pair;
using std::vector;

//This file generates moves given a board

using function = uint64_t(MoveBBCreator::*)(int, uint64_t, uint64_t);


const uint64_t allSet = 0xffffffffffffffff; //BB with all 1s

struct Pinned //This structure stores all neccessary information about a pinned piece
{
	Piece pieceType; //Type of piece
	int pinnedPos, attackerPos; //The position of the pinned piece, position of attacker
	uint64_t pinBB; //BB of pin direction

	Pinned(int pieceIn, int pinnnedIn, int attackerIn, uint64_t pinBBIn) {
		pieceType = (Piece)pieceIn;
		pinnedPos = pinnnedIn;
		attackerPos = attackerIn;
		pinBB = pinBBIn;
	}
};




class MoveCreator {
public: Board board;
	  vector<Move> nonCap, capture; //Creates 2 vectors for storing moves so they dont have to be seperate after generation
	  BitOperations bitOp;
	  MoveBBCreator bbCreator;
	  vector<Pinned> pinnedPieces;
	  int checkers = 0; //How many pieces are checking the king
	  uint64_t checkingBB = 0; //Where they are checking the king from
	  uint64_t pinnedBB = 0; //Where pieces are pinned
	  uint64_t ownColourBB = 0, oppColourBB = 0, allBB = 0; //Useful bitboards
	  bool capturesOnly; //If we are only search for captures (during quiescence search)
	  bool debug;


public: MoveCreator(Board boardIn) {
	board = boardIn; //Takes board on creation and stores it
}

private: void setup() { //Sets up variables at the start of every generation
	nonCap.clear(); //Clears vectors
	capture.clear();
	pinnedPieces.clear();
	checkers = 0; //Resets vars
	checkingBB = 0;
	pinnedBB = 0;
	allBB = board.get_all_BB();
	ownColourBB = board.bitboards[board.toMove];
	oppColourBB = board.bitboards[board.toNotMove];
	setup_check_pinned(board.toMove); //Finds pinned pieces
}

public: vector<Move> get_q_moves() { //Just gets captures for q search
	setup();
	capturesOnly = true;
	return get_moves();
}

public: vector<Move> get_all_moves() { //Gets all moves
	return get_all_moves(false);
}

public: vector<Move> get_all_moves(bool debugIn) {
	setup(); //Sets up
	capturesOnly = false;
	debug = debugIn;
	if (debug) { //Used for debug purposes
		cout << "checkers: " << checkers << "\n";
		cout << "checking: " << checkingBB << "\n";
		cout << "wKing: " << board.get_piece_BB(white, king) << "\n";
		cout << "knight: " << board.bitboards[3] << "\n";
		cout << "bishop: " << board.bitboards[4] << "\n";
		cout << "move1" << "\n";
		cout << "pawnsB: " << board.get_piece_BB(black, pawn) << "\n";
	}
	return get_moves();
}

private: vector<Move> get_moves() {
	if (debug) { //Debug purposed
		cout << capturesOnly << "\n";
	}
	if (checkers == 2) { //MAYBE CHANGE to >= 2 //If there are 2 or more checkers then only the king can move
		get_king_moves();
	}
	else {
		get_pinned_moves(); //Get moves for pinned pieces
		get_non_pinned_moves(); //Get moves for non-pinned pieces
	}
	vector<Move> winning, equal, losing; //Sorts captures into winning or losing capture
	for (Move& move : capture) { //NEED to change as losing move may not be neccasirily losing
		if (move.pieceType < move.pieceCapture) {
			winning.push_back(move);
		}
		else if (move.pieceType == move.pieceCapture) {
			equal.push_back(move);
		}
		else {
			losing.push_back(move);
		}
	}
	nonCap.insert(nonCap.begin(), losing.begin(), losing.end());
	nonCap.insert(nonCap.begin(), equal.begin(), equal.end());
	nonCap.insert(nonCap.begin(), winning.begin(), winning.end());
	return nonCap; //Compiles all moves into 1 vector with the best captures first
}

	   /*private: bool capture_winning(Move& move) {
		   if (move.pieceType >= move.pieceCapture) {
			   return true;
		   }
		   else {
			   int value = 0;
			   pair<Piece, int> attacker = get_smallest_attacker(move.startPos, board.toMove);
		   }
	   }
	   private: pair<Piece, int> get_smallest_attacker(int pos, int colour) {
		   Piece oppColour = (Piece)(board.toMove ^ 1);
		   uint64_t allBBNoPos = allBB & ~ 1ULL << pos;
		   uint64_t pawnAttackers = bbCreator.get_king_pawn_attack_BB(pos, board.toMove) & board.get_piece_BB(oppColour, pawn);
		   if (pawnAttackers != 0) { // pawn attacks
			   return make_pair(pawn, bitOp.lsb_bitscan(pawnAttackers));
		   }
		   uint64_t knightAttackers = bbCreator.get_knight_BB_empty(pos) & board.get_piece_BB(oppColour, knight);
		   if (knightAttackers != 0) { //knight attacks
			   return make_pair(knight, bitOp.lsb_bitscan(knightAttackers));
		   }
		   int bishopAttacker = bbCreator.attacker_pos(new int[4] { 0,2,4,6 }, board.get_piece_BB(oppColour, bishop), pos, allBBNoPos);
		   if (bishopAttacker < 64) {
			   return make_pair(bishop, bitOp.lsb_bitscan(bishopAttacker));
		   }
		   int rookAttacker = bbCreator.attacker_pos(new int[4]{ 1,3,5,7 }, board.get_piece_BB(oppColour, rook), pos, allBBNoPos);
		   if (bishopAttacker < 64) {
			   return make_pair(rook, bitOp.lsb_bitscan(rookAttacker));
		   }
		   int queenAttacker = bbCreator.attacker_pos(new int[8]{ 0,1,2,3,4,5,6,7 }, board.get_piece_BB(oppColour, queen), pos, allBBNoPos);
		   if (bishopAttacker < 64) {
			   return make_pair(queen, bitOp.lsb_bitscan(queenAttacker));
		   }
		   uint64_t kingAttacker = bbCreator.get_king_BB(pos, 0) & board.get_piece_BB(oppColour, king);
		   if (kingAttacker != 0) {
			   return make_pair(king, bitOp.lsb_bitscan(kingAttacker));
		   }
		   return make_pair(white, 0);

		   return false;
	   }*/




private: void get_pinned_moves() {
	for (Pinned& pinnedP : pinnedPieces) {
		pinnedBB |= (1ULL << pinnedP.pinnedPos);
		if (pinnedP.pieceType == pawn) {
			get_pinned_pawn(pinnedP);
		}
		else {
			get_pinned_other(pinnedP);
		}
	}
}



private: void get_pinned_other(Pinned& pinnedP) {
	function get_BB = &MoveBBCreator::get_knight_BB; // just placeholder value
	switch (pinnedP.pieceType) {
	case knight:
		get_BB = &MoveBBCreator::get_knight_BB;
		break;
	case bishop:
		get_BB = &MoveBBCreator::get_bishop_BB;
		break;
	case rook:
		get_BB = &MoveBBCreator::get_rook_BB;
		break;
	case queen:
		get_BB = &MoveBBCreator::get_queen_BB;
		break;
	}
	Piece pieceCaptured;
	uint64_t movesBB = ((bbCreator.*get_BB)(pinnedP.pinnedPos, allBB, ownColourBB) & checkingBB) & pinnedP.pinBB;
	int endPos;
	pair<int*, int> fullScan = bitOp.full_bitscan(bbCreator.get_attacks(movesBB, oppColourBB));
	for (int i = 0; i < fullScan.second; i++) {
		endPos = fullScan.first[i];
		pieceCaptured = board.get_piece_from_pos(endPos);
		capture.emplace_back(Move(pinnedP.pieceType, pinnedP.pinnedPos, endPos, pieceCaptured));
	}
	if (!capturesOnly) {
		fullScan = bitOp.full_bitscan(bbCreator.get_quiet(movesBB, oppColourBB));
		for (int i = 0; i < fullScan.second; i++) {
			nonCap.emplace_back(Move(pinnedP.pieceType, pinnedP.pinnedPos, fullScan.first[i]));
		}
	}

}



private: void get_non_pinned_moves() {
	get_unpinned_pawn_moves();
	get_other_piece_moves(knight, &MoveBBCreator::get_knight_BB);
	get_other_piece_moves(bishop, &MoveBBCreator::get_bishop_BB);
	get_other_piece_moves(rook, &MoveBBCreator::get_rook_BB);
	get_other_piece_moves(queen, &MoveBBCreator::get_queen_BB);
	get_king_moves();
}

private: void setup_check_pinned(int colour) { //return attacking rays, return rays that are pinning pieces alongside pieces
	int kingPos = bitOp.lsb_bitscan(board.get_piece_BB((Piece)colour, king));
	Piece oppColour = (Piece)(colour ^ 1);
	checkingBB |= (bbCreator.get_knight_BB_empty(kingPos) & board.get_piece_BB(oppColour, knight)); //knight attacks
	if (checkingBB != 0) { // not sure if IF needed
		checkers += bitOp.pop_count(checkingBB);
	}
	int moves[4] = { 0,2,4,6 }; // diagonals
	uint64_t diagAttackers = board.get_piece_BB(oppColour, queen) | board.get_piece_BB(oppColour, bishop); //diagonal attacks
	get_all_attackers_pinned(moves, diagAttackers, kingPos, colour);
	int moves2[4] = { 1,3,5,7 }; //straights
	uint64_t straightAttackers = board.get_piece_BB(oppColour, queen) | board.get_piece_BB(oppColour, rook);
	get_all_attackers_pinned(moves2, straightAttackers, kingPos, colour);
	uint64_t pawnAttacks = bbCreator.get_king_pawn_attack_BB(kingPos, colour) & board.get_piece_BB(oppColour, pawn);
	checkingBB |= pawnAttacks;
	checkers += bitOp.pop_count(pawnAttacks);
	if (checkers == 0) {
		checkingBB = allSet;
	}
}

	   //can be improved, made branchless
private: void get_all_attackers_pinned(int* moves, uint64_t attackers, int kingPos, int colour) {//returns a bitboard of rays attacking king and list of pinned pieces and direction they are pinned
	uint64_t blockersBB;
	uint64_t rayBB;
	uint64_t pinnedBB;
	int blockerPos;
	int blockerPos2;
	for (int i = 0; i < 4; i++) {
		rayBB = bbCreator.lookup.slideMoves[kingPos][moves[i]];
		blockersBB = rayBB & allBB;
		if (blockersBB != 0) {
			blockerPos = bbCreator.get_blocker_pos(blockersBB, moves[i]);
			if (((board.bitboards[colour]) & (1ULL << blockerPos)) != 0) {
				blockersBB -= 1ULL << blockerPos;
				if (blockersBB != 0) {
					blockerPos2 = bbCreator.get_blocker_pos(blockersBB, moves[i]);
					if (((1ULL << blockerPos2) & attackers) != 0) {
						pinnedBB = (bbCreator.lookup.slideMoves[blockerPos2][moves[i]] ^ bbCreator.lookup.slideMoves[kingPos][moves[i]]); //line can be done better using the direction value to get the pos for the ray to xor
						pinnedPieces.emplace_back(Pinned(board.get_piece_from_pos(blockerPos), blockerPos, blockerPos2, pinnedBB));
					}
				}
			}
			else if (((1ULL << blockerPos) & attackers) != 0) {
				checkers++;
				checkingBB |= (rayBB ^ bbCreator.lookup.slideMoves[blockerPos][moves[i]]);
			}
		}
	}
}

private: bool square_threatened_any(int pos) {
	Piece oppColour = (Piece)(board.toMove ^ 1);
	uint64_t allBBNoKing = allBB & ~board.get_piece_BB(board.toMove, king);
	if ((bbCreator.get_knight_BB_empty(pos) & board.get_piece_BB(oppColour, knight)) != 0) { //knight attacks
		return true;
	}
	if ((bbCreator.get_king_BB(pos, 0) & board.get_piece_BB(oppColour, king)) != 0) {
		return true;
	}
	int moves[4] = { 0,2,4,6 }; // diagonals
	uint64_t diagAttackers = board.get_piece_BB(oppColour, queen) | board.get_piece_BB(oppColour, bishop);
	if (bbCreator.any_attackers(moves, diagAttackers, pos, allBBNoKing)) {
		return true;
	}
	int moves2[4] = { 1,3,5,7 }; //straights
	uint64_t straightAttackers = board.get_piece_BB(oppColour, queen) | board.get_piece_BB(oppColour, rook);
	if (bbCreator.any_attackers(moves2, straightAttackers, pos, allBBNoKing)) {
		return true;
	}
	if ((bbCreator.get_king_pawn_attack_BB(pos, board.toMove) & board.get_piece_BB(oppColour, pawn)) != 0) { // pawn attacks
		return true;
	}
	return false;
}



private: void get_king_moves() {
	uint64_t kingBB = board.get_piece_BB((Piece)board.toMove, king);
	int pos = bitOp.lsb_bitscan(kingBB);
	uint64_t allMovesBB = bbCreator.get_king_BB(pos, ownColourBB);
	Piece pieceCaptured;
	int castling = board.castling[board.toMove];
	int endPos;
	pair<int*, int> fullScan = bitOp.full_bitscan(bbCreator.get_attacks(allMovesBB, oppColourBB));
	for (int i = 0; i < fullScan.second; i++) {
		endPos = fullScan.first[i];
		if (!square_threatened_any(endPos)) {
			pieceCaptured = board.get_piece_from_pos(endPos);
			capture.emplace_back(Move(king, pos, endPos, pieceCaptured));
		}
	}
	if (!capturesOnly) {
		fullScan = bitOp.full_bitscan(bbCreator.get_quiet(allMovesBB, oppColourBB));
		for (int i = 0; i < fullScan.second; i++) {
			endPos = fullScan.first[i];
			if (!square_threatened_any(endPos)) {
				nonCap.emplace_back(Move(king, pos, endPos));
			}
		}

		if (checkers == 0) {
			if (can_queenside_castle(castling)) { //queenside
				nonCap.emplace_back(Move(king, pos, pos - 2, white, 1));
			}
			if (can_kingside_castle(castling)) { //queenside
				capture.emplace_back(Move(king, pos, pos + 2, white, 2));
			}
		}
	}
}





private: bool can_queenside_castle(int castling) {
	int rowPos = 56 * board.toMove;
	if ((castling & 1UL) == 1) {
		if (bbCreator.sqr_empty(allBB, rowPos + 1) && bbCreator.sqr_empty(allBB, rowPos + 2) && bbCreator.sqr_empty(allBB, rowPos + 3)) {
			if (!square_threatened_any(rowPos + 2) && !square_threatened_any(rowPos + 3)) {
				return true;
			}
		}
	}
	return false;
}

private: bool can_kingside_castle(int castling) {
	int rowPos = 56 * board.toMove;
	if (((castling >> 1) & 1UL) == 1) {
		if (bbCreator.sqr_empty(allBB, rowPos + 5) && bbCreator.sqr_empty(allBB, rowPos + 6)) {
			if (!square_threatened_any(rowPos + 5) && !square_threatened_any(rowPos + 6)) {
				return true;
			}
		}
	}
	return false;
}

private: void get_other_piece_moves(Piece piece, function get_BB) {
	uint64_t pieceBB = board.get_piece_BB((Piece)board.toMove, piece) & ~pinnedBB;
	uint64_t allMovesBB;
	Piece pieceCaptured;
	pair<int*, int> fullScan1 = bitOp.full_bitscan(pieceBB);
	int pos;
	int endPos;
	for (int i = 0; i < fullScan1.second; i++) {
		pos = fullScan1.first[i];
		allMovesBB = (bbCreator.*get_BB)(pos, allBB, ownColourBB) & checkingBB;
		pair<int*, int> fullScan2 = bitOp.full_bitscan(bbCreator.get_attacks(allMovesBB, oppColourBB));
		for (int j = 0; j < fullScan2.second; j++) {
			endPos = fullScan2.first[j];
			pieceCaptured = board.get_piece_from_pos(endPos);
			capture.emplace_back(Move(piece, pos, endPos, pieceCaptured));
		}
		if (!capturesOnly) {
			fullScan2 = bitOp.full_bitscan(bbCreator.get_quiet(allMovesBB, oppColourBB));
			for (int j = 0; j < fullScan2.second; j++) {
				nonCap.emplace_back(Move(piece, pos, fullScan2.first[j]));
			}
		}
	}
}

private: void get_pinned_pawn(Pinned pinnedPawn) {
	uint64_t attackerBB = (1ULL << pinnedPawn.attackerPos);
	get_pawn_moves((1ULL << pinnedPawn.pinnedPos), pinnedPawn.pinBB ^ attackerBB, attackerBB);
}


private: void pawn_quietBB_to_moves(uint64_t moveBB, int diff) {//do promo in here
	moveBB &= checkingBB;

	pair<uint64_t, uint64_t> promos = bbCreator.get_promo_BB(moveBB);
	pair<int*, int> fullScan = bitOp.full_bitscan(promos.first);
	if (debug) {
		cout << "pawnsProm: " << promos.first << "\n";
		cout << "checking: " << checkingBB << "\n";
		cout << "pawnCount: " << fullScan.second << "\n";
	}
	int pos;
	for (int i = 0; i < fullScan.second; i++) {
		pos = fullScan.first[i];
		nonCap.emplace_back(Move(pawn, pos + diff, pos));
	}
	fullScan = bitOp.full_bitscan(promos.second);
	for (int i = 0; i < fullScan.second; i++) {
		pos = fullScan.first[i];
		for (int j = 3; j < 7; j++) {
			nonCap.emplace_back(Move(pawn, pos + diff, pos, white, 0, (Piece)j));
		}
	}
}

private: void pawn_captureBB_to_moves(uint64_t moveBB, int diff) {//do promo in here
	moveBB &= checkingBB;
	pair<uint64_t, uint64_t> promos = bbCreator.get_promo_BB(moveBB);
	pair<int*, int> fullScan = bitOp.full_bitscan(promos.first);
	int pos;
	for (int i = 0; i < fullScan.second; i++) {
		pos = fullScan.first[i];
		capture.emplace_back(Move(pawn, pos + diff, pos, board.get_piece_from_pos(pos)));
	}
	fullScan = bitOp.full_bitscan(promos.second);
	for (int i = 0; i < fullScan.second; i++) {
		pos = fullScan.first[i];
		for (int j = 3; j < 7; j++) {
			capture.emplace_back(Move(pawn, pos + diff, pos, board.get_piece_from_pos(pos), 0, (Piece)j));
		}
	}
}

private: void pawn_enPassantBB_to_moves(uint64_t moveBB, int diff) {//do promo in here
	pair<int*, int> fullScan = bitOp.full_bitscan(moveBB);
	int pos;
	for (int i = 0; i < fullScan.second; i++) {
		pos = fullScan.first[i];
		int startPos = pos + diff;
		if (!(check_after_enpassant(startPos))) {
			capture.emplace_back(Move(pawn, pos + diff, pos, pawn, 0, white, board.enPassant));
		}
	}
}

private: void get_pawn_moves(uint64_t pawnBB, uint64_t emptyBB, uint64_t oppBB) { //rename
	if (board.toMove == 0) {
		pair<uint64_t, uint64_t> captureBBs = bbCreator.get_white_pawn_attack(pawnBB, oppBB);
		pawn_captureBB_to_moves(captureBBs.first, -7);
		pawn_captureBB_to_moves(captureBBs.second, -9);
		if (!capturesOnly) {
			pair<uint64_t, uint64_t> quietBBs = bbCreator.get_white_pawn_quiet(pawnBB, emptyBB);
			pawn_quietBB_to_moves(quietBBs.first, -8);
			pawn_quietBB_to_moves(quietBBs.second, -16);
			if (board.enPassant != 0) {
				pair<uint64_t, uint64_t> enPassant = bbCreator.get_white_pawn_attack(pawnBB, (1ULL << board.enPassant));
				pawn_enPassantBB_to_moves(enPassant.first & emptyBB, -7);
				pawn_enPassantBB_to_moves(enPassant.second & emptyBB, -9);
			}
		}
	}
	else
	{
		pair<uint64_t, uint64_t> captureBBs = bbCreator.get_black_pawn_attack(pawnBB, oppBB);
		pawn_captureBB_to_moves(captureBBs.first, 9);
		pawn_captureBB_to_moves(captureBBs.second, 7);
		if (!capturesOnly) {
			pair<uint64_t, uint64_t> quietBBs = bbCreator.get_black_pawn_quiet(pawnBB, emptyBB);
			if (debug) {
				cout << "movesBB: " << (quietBBs.first | quietBBs.second) << "\n";
			}
			pawn_quietBB_to_moves(quietBBs.first, 8);
			pawn_quietBB_to_moves(quietBBs.second, 16);
			if (board.enPassant != 0) {
				pair<uint64_t, uint64_t> enPassant = bbCreator.get_black_pawn_attack(pawnBB, (1ULL << board.enPassant));
				pawn_enPassantBB_to_moves(enPassant.first & emptyBB, 9);
				pawn_enPassantBB_to_moves(enPassant.second & emptyBB, 7);
			}
		}
	}
}


private: void get_unpinned_pawn_moves() {
	get_pawn_moves(board.get_piece_BB(board.toMove, pawn) & ~pinnedBB, ~allBB, oppColourBB);
}





private: bool check_after_enpassant(int startPos) {
	int enPassantC;
	if (board.toMove == 0) {
		enPassantC = board.enPassant - 8;
	}
	else {
		enPassantC = board.enPassant + 8;
	}
	uint64_t newBB = (allBB | 1ULL << board.enPassant) & ~((1ULL << enPassantC) | (1ULL << startPos));
	int kingPos = bitOp.lsb_bitscan(board.get_piece_BB((Piece)board.toMove, king));
	int moves[4] = { 0,2,4,6 }; // diagonals
	uint64_t diagAttackers = board.get_piece_BB(board.toNotMove, queen) | board.get_piece_BB(board.toNotMove, bishop);
	if (bbCreator.any_attackers(moves, diagAttackers, kingPos, newBB)) {
		return true;
	}
	int moves2[4] = { 1,3,5,7 }; //straights
	uint64_t straightAttackers = board.get_piece_BB(board.toNotMove, queen) | board.get_piece_BB(board.toNotMove, rook);
	if (bbCreator.any_attackers(moves2, straightAttackers, kingPos, newBB)) {
		return true;
	}
	return false;
}

};