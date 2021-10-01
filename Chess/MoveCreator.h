#pragma once
#include <vector>
#include "MoveBBCreator.h"
#include "Board.h"

using std::pair;
using std::vector;

//This file generates moves given a board

using function = uint64_t(MoveBBCreator::*)(int, uint64_t, uint64_t); //Allows different functions to take the same variables


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
	  uint64_t checkingBB = 0; //Shows where pieces can move when the king is in check
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




private: void get_pinned_moves() { //Gets all pinned moves
	for (Pinned& pinnedP : pinnedPieces) {
		pinnedBB |= (1ULL << pinnedP.pinnedPos); //Adds to pinned piece to pinned BB
		if (pinnedP.pieceType == pawn) { //Deals with pawns and other pieces seperately
			get_pinned_pawn(pinnedP);
		}
		else {
			get_pinned_other(pinnedP);
		}
	}
}



private: void get_pinned_other(Pinned& pinnedP) { //Gets pinned moves for pinned pieces which aren't pawns
	function get_BB = &MoveBBCreator::get_knight_BB; // just placeholder value
	switch (pinnedP.pieceType) { //Choose the correct function for the piece
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
	uint64_t movesBB = ((bbCreator.*get_BB)(pinnedP.pinnedPos, allBB, ownColourBB) & checkingBB) & pinnedP.pinBB; //This finds moves that pinned pieces can make
	int endPos;
	pair<int*, int> fullScan = bitOp.full_bitscan(bbCreator.get_attacks(movesBB, oppColourBB));//splits the BB in seperate moves
	for (int i = 0; i < fullScan.second; i++) { //Adds capture moves to list
		endPos = fullScan.first[i];
		pieceCaptured = board.get_piece_from_pos(endPos);
		capture.emplace_back(Move(pinnedP.pieceType, pinnedP.pinnedPos, endPos, pieceCaptured));
	}
	if (!capturesOnly) { //Adds non capture moves to list
		fullScan = bitOp.full_bitscan(bbCreator.get_quiet(movesBB, oppColourBB));
		for (int i = 0; i < fullScan.second; i++) {
			nonCap.emplace_back(Move(pinnedP.pieceType, pinnedP.pinnedPos, fullScan.first[i]));
		}
	}

}

private: void setup_check_pinned(int colour) { //Finds if kings is in check and if there are pinned pieces
	int kingPos = bitOp.lsb_bitscan(board.get_piece_BB((Piece)colour, king)); //Get position of king
	Piece oppColour = (Piece)(colour ^ 1);
	checkingBB |= (bbCreator.get_knight_BB_empty(kingPos) & board.get_piece_BB(oppColour, knight)); //Finds if any knights attacking king
	if (checkingBB != 0) { //Adds number of knights attacking king checkers
		checkers += bitOp.pop_count(checkingBB);
	}
	int moves[4] = { 0,2,4,6 }; // diagonals
	uint64_t diagAttackers = board.get_piece_BB(oppColour, queen) | board.get_piece_BB(oppColour, bishop); //Gets BB of diagonal attackers
	get_sliding_attackers_pinned(moves, diagAttackers, kingPos, colour); //Checks if any pieces are diagonally attacking king
	int moves2[4] = { 1,3,5,7 }; //Orthogonal
	uint64_t straightAttackers = board.get_piece_BB(oppColour, queen) | board.get_piece_BB(oppColour, rook); //Gets BB of orthogonal attackers
	get_sliding_attackers_pinned(moves2, straightAttackers, kingPos, colour); //Checks if any pieces are orthogonally attacking king
	uint64_t pawnAttacks = bbCreator.get_king_pawn_attack_BB(kingPos, colour) & board.get_piece_BB(oppColour, pawn); //Gets if any pawns are attacking king
	checkingBB |= pawnAttacks;
	checkers += bitOp.pop_count(pawnAttacks); //Adds number of attacking pawns to checkers
	if (checkers == 0) {
		checkingBB = allSet; //If there are no pieces attacking the king all pieces can move somewhere
	}
}

private: void get_non_pinned_moves() { //Gets all moves for non-pinned pieces
	get_unpinned_pawn_moves();
	get_other_piece_moves(knight, &MoveBBCreator::get_knight_BB);
	get_other_piece_moves(bishop, &MoveBBCreator::get_bishop_BB);
	get_other_piece_moves(rook, &MoveBBCreator::get_rook_BB);
	get_other_piece_moves(queen, &MoveBBCreator::get_queen_BB);
	get_king_moves(); //
}

	
private: void get_sliding_attackers_pinned(int* moves, uint64_t attackers, int kingPos, int colour) {//Finds if king is in check from sliding pieces or if there are pinned pieces
	uint64_t blockersBB;
	uint64_t rayBB;
	uint64_t pinnedBB;
	int blockerPos;
	int blockerPos2;
	for (int i = 0; i < 4; i++) {
		rayBB = bbCreator.lookup.slideMoves[kingPos][moves[i]]; //Gets ray from king position in all directions give in moves
		blockersBB = rayBB & allBB; //Finds pieces along the ray
		if (blockersBB != 0) { //If there are blockers
			blockerPos = bbCreator.get_blocker_pos(blockersBB, moves[i]); //Finds the position of the closest blocker
			if (((board.bitboards[colour]) & (1ULL << blockerPos)) != 0) { //If blocker is of current turns colour (so it could be pinned)
				blockersBB -= 1ULL << blockerPos; //Removes the first blocker
				if (blockersBB != 0) { //If there is another blocker (piece along the ray)
					blockerPos2 = bbCreator.get_blocker_pos(blockersBB, moves[i]); //Gets position of second blocker
					if (((1ULL << blockerPos2) & attackers) != 0) { //If 2nd blocker is an attacker, the 1st blocker will be pinned
						//This puts all the info about the pinned piece in an object then adds to vector of pinned pieces 
						pinnedBB = (bbCreator.lookup.slideMoves[blockerPos2][moves[i]] ^ bbCreator.lookup.slideMoves[kingPos][moves[i]]); 
						pinnedPieces.emplace_back(Pinned(board.get_piece_from_pos(blockerPos), blockerPos, blockerPos2, pinnedBB));
					}
				}
			}
			else if (((1ULL << blockerPos) & attackers) != 0) { //If the first blocker is any attacker then the king is in check
				checkers++; //Increment number of checkers
				checkingBB |= (rayBB ^ bbCreator.lookup.slideMoves[blockerPos][moves[i]]); //Adds ray to checking BB as pieces can move onto this and block check
			}
		}
	}
}

private: bool square_threatened_any(int pos) { //Checks if the square at pos is under attack
	Piece oppColour = (Piece)(board.toMove ^ 1); //Gets BB of other players pieces
	uint64_t allBBNoKing = allBB & ~board.get_piece_BB(board.toMove, king);
	if ((bbCreator.get_knight_BB_empty(pos) & board.get_piece_BB(oppColour, knight)) != 0) { //Checks for knight attacks
		return true;
	}
	if ((bbCreator.get_king_BB(pos, 0) & board.get_piece_BB(oppColour, king)) != 0) { //Checks if king is attacking square
		return true;
	}
	int moves[4] = { 0,2,4,6 }; // diagonals
	uint64_t diagAttackers = board.get_piece_BB(oppColour, queen) | board.get_piece_BB(oppColour, bishop); //Checks if there is a diagonal attack on square
	if (bbCreator.any_attackers(moves, diagAttackers, pos, allBBNoKing)) {
		return true;
	}
	int moves2[4] = { 1,3,5,7 }; //straights
	uint64_t straightAttackers = board.get_piece_BB(oppColour, queen) | board.get_piece_BB(oppColour, rook); //Checks if there is an orthogonal attack on square
	if (bbCreator.any_attackers(moves2, straightAttackers, pos, allBBNoKing)) {
		return true;
	}
	if ((bbCreator.get_king_pawn_attack_BB(pos, board.toMove) & board.get_piece_BB(oppColour, pawn)) != 0) { //Checks if a pawn is attacking square
		return true;
	}
	return false;
}



private: void get_king_moves() { //Gets kings moves
	uint64_t kingBB = board.get_piece_BB((Piece)board.toMove, king); //Gets BB with just king
	int pos = bitOp.lsb_bitscan(kingBB); //Gets position of king
	uint64_t allMovesBB = bbCreator.get_king_BB(pos, ownColourBB); //Gets all sqrs king can move to normally
	Piece pieceCaptured;
	int castling = board.castling[board.toMove]; //Gets castling rights
	int endPos;
	pair<int*, int> fullScan = bitOp.full_bitscan(bbCreator.get_attacks(allMovesBB, oppColourBB)); 
	for (int i = 0; i < fullScan.second; i++) { //Iterates through all squares king can attack which are capture
		endPos = fullScan.first[i];
		if (!square_threatened_any(endPos)) { //Checks if the king would end up in check
			pieceCaptured = board.get_piece_from_pos(endPos); 
			capture.emplace_back(Move(king, pos, endPos, pieceCaptured)); //Adds to list
		}
	}
	if (!capturesOnly) {
		fullScan = bitOp.full_bitscan(bbCreator.get_quiet(allMovesBB, oppColourBB)); //Same as above with non captures
		for (int i = 0; i < fullScan.second; i++) {
			endPos = fullScan.first[i];
			if (!square_threatened_any(endPos)) {
				nonCap.emplace_back(Move(king, pos, endPos));
			}
		}

		if (checkers == 0) { //If not in check we check for castling
			if (can_queenside_castle(castling)) { //If can queenside castle it is added to list
				nonCap.emplace_back(Move(king, pos, pos - 2, white, 1));
			}
			if (can_kingside_castle(castling)) { //If can kingside castle it is added to list
				capture.emplace_back(Move(king, pos, pos + 2, white, 2));
			}
		}
	}
}





private: bool can_queenside_castle(int castling) { //Checks if king can queenside castle
	int rowPos = 56 * board.toMove; //Finds row for castling
	if ((castling & 1UL) == 1) { //Checks castling rights
		if (bbCreator.sqr_empty(allBB, rowPos + 1) && bbCreator.sqr_empty(allBB, rowPos + 2) && bbCreator.sqr_empty(allBB, rowPos + 3)) { //Checks if all squares between king and rook and empty
			if (!square_threatened_any(rowPos + 2) && !square_threatened_any(rowPos + 3)) { //Checks the squares king moves across arent threatened
				return true; //Can castle
			}
		}
	}
	return false; //Can't castle
}

private: bool can_kingside_castle(int castling) { //Same as above but for kingside
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

private: void get_other_piece_moves(Piece piece, function get_BB) { //Gets moves for non-king and non-pawn pieces
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
