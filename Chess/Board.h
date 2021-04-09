#pragma once
#include <stdint.h>
#include <string>
#include <stack>
#include "Move.h"
#include "BitOperations.h"

using std::string;
using std::pair;
using namespace N;




class Board
{

public: uint64_t bitboards[8] = {0};
//public: stack<Move> moves;
	int toMove= 0;
	int toNotMove = 0;
	int castling[2] = { 0,0 }; //KQkq
	int enPassant;

	private: void check_for_castle(Move &move) {
		move.castlingBefore[0] = castling[0];
		move.castlingBefore[1] = castling[1];
		if (move.pieceType == king) {
			castling[toMove] = 0;
			if (move.castling != 0) {
				int row = 56 * toMove;
				if (move.castling == 1) {
					move_piece(rook, 0 + row, 3 + row, toMove);
				}
				else {
					move_piece(rook, 7 + row, 5 + row, toMove);
				}
			}
		}
		else if (move.pieceType == rook) {
			if (move.startPos == 0 + 56 * toMove) {
				castling[toMove] &= ~(1UL);
			}
			if (move.startPos == 7 + 56 * toMove) {
				castling[toMove] &= ~(1UL<<1);
			}
		}
		if (move.pieceCapture == rook) {
			if (move.endPos == 0 + 56 * toNotMove) {
				castling[toNotMove] &= ~(1UL);
			}
			if (move.endPos == 7 + 56 * toNotMove) {
				castling[toNotMove] &= ~(1UL << 1);
			}
		}
	}

	private: void change_turn() {
		toNotMove = toMove;
		toMove ^= 1;
	}

	private: void check_for_en_passant(Move &move) {
		move.enPassantBefore = enPassant;
		if (move.enPassant) {
			if (toMove == 0) {
				remove_piece(pawn, enPassant - 8, toNotMove);
			}
			else {
				remove_piece(pawn, enPassant + 8, toNotMove);
			}
			enPassant = 0;
			return;
		}
		else if (move.pieceType == pawn && abs(move.startPos - move.endPos) == 16) {
			enPassant = move.endPos + (move.startPos - move.endPos )/2;
		}
		else{
			enPassant = 0;
		}
		
	}

	public : void make_move(Move &move) {
		check_for_castle(move);
		check_for_en_passant(move);
		if (move.pieceCapture != 0) {
			remove_piece(move.pieceCapture, move.endPos, toNotMove);
		}
		if (move.promoPiece == 0) {
			move_piece(move.pieceType, move.startPos, move.endPos, toMove);
		}
		else {
			remove_piece(move.pieceType, move.startPos, toMove);
			add_piece(move.promoPiece, move.endPos, toMove);
		}
		change_turn();
	}

	private: void move_piece(Piece piece, int startPos, int endPos, int colour) {
		remove_piece(piece, startPos, colour);
		add_piece(piece, endPos, colour);
	}

public: void unmake_move(Move& move) {
	change_turn();
	unmake_castling(move);
	unmake_enPassant(move);
	if (move.promoPiece == 0) {
		move_piece(move.pieceType, move.endPos, move.startPos, toMove);
	}
	else {
		add_piece(move.pieceType, move.startPos, toMove);
		remove_piece(move.promoPiece, move.endPos, toMove);
	}
	if (move.pieceCapture != 0 && !(move.enPassant)) {
		add_piece(move.pieceCapture, move.endPos, toNotMove);
	}
	}

private: void unmake_castling(Move& move) {
	castling[0] = move.castlingBefore[0];
	castling[1] = move.castlingBefore[1];
	if (move.castling != 0) {
		int row = 56 * toMove;
		if (move.castling == 1) {
			move_piece(rook, 3 + row, 0 + row, toMove);
		}
		else {
			move_piece(rook,  5 + row, 7 + row, toMove);
		}
	}
}

public: void make_move_notation(string moveStr) {
	Move move = get_move_notation(moveStr);
	make_move(move);
}

private: Move get_move_notation(string moveStr) { //assume move is legal
	int startPos = LERF_to_sqr(moveStr.substr(0, 2));
	int endPos = LERF_to_sqr(moveStr.substr(2, 2));
	Piece pieceType = get_piece_from_pos(startPos);
	Piece captured;
	if (((get_all_BB() >> endPos) & 1ULL) == 1) {
		captured = get_piece_from_pos(endPos);
	}
	else {
		captured = (Piece)0;
	}
	if (pieceType == pawn) {
		if (enPassant != 0 && endPos == enPassant) {
			return Move(pawn, startPos, endPos, pawn, 0, white, enPassant);
		}
		else if ((endPos >> 3) == 0 || (endPos >> 3) == 7) {
			char promo = moveStr.at(5);
			Piece promoPiece = letter_to_piece(promo).second;
			return Move(pawn, startPos, endPos, captured, 0, promoPiece, 0);
		}
	}
	else if (pieceType == king) {
		int colourRank = toMove * 56;
		if (startPos == 4 + colourRank) { //on king's startpos
			if (endPos == 6 + colourRank) {
				return Move(king, startPos, endPos, white, 2);
			}
			else if (endPos == 2 + colourRank) {
				return Move(king, startPos, endPos, white, 1);
			}
		}
	}
	return Move(pieceType, startPos, endPos, captured);
}


public: Piece get_piece_from_pos(int pos) {
	for (int i = 2; i < 8; i++) {
		if (((bitboards[i] >> pos) & 1ULL) == 1) {
			return (Piece)i;
		}
	}
}

private: void unmake_enPassant(Move& move) {
	enPassant = move.enPassantBefore;
	if (move.enPassant) {
		if (toMove == 0) {
			add_piece(pawn, move.endPos - 8, toNotMove);
		}
		else {
			add_piece(pawn, move.endPos + 8, toNotMove);
		}
	}
}

private: void remove_piece(Piece piece, int pos, int colour) {
	bitboards[colour] &= ~(1ULL << pos);
	bitboards[piece] &= ~(1ULL << pos);
	}

private: void add_piece(Piece piece, int pos, int colour) {
	bitboards[colour] |= 1ULL << pos;
	bitboards[piece] |= 1ULL << pos;
}



	public : 
		uint64_t get_piece_BB(Piece colour, Piece piece) {
			return bitboards[colour] & bitboards[piece];
		}

public:
	uint64_t get_piece_BB(int colour, Piece piece) {
		return get_piece_BB((Piece)colour, piece);
	}

	public:
		uint64_t get_all_BB() {
			return bitboards[white] | bitboards[black];
		}

	public:
		void create_from_FEN(string fen) {
			//fill boards in from FEN
			for (int i = 0; i < 8; i++) {//sets all boards to 0
				bitboards[i] = 0;
			}
			int64_t sqr = 56;
			int i = 0;
			int section = 0;
			while(section < 4) {
				char consd = fen[i];
				if (consd == ' ') {
					section += 1;
				}
				else if (section == 0) {//board section
					if (consd == '/') { // new row
						sqr -= 16;
					}
					else if (consd > '0' && consd < '9') { //consd if digit denoting empty space
						sqr += int64_t(consd) - '0';
					}
					else {
						pair<Piece, Piece> pieceInfo = letter_to_piece(consd);
						bitboards[pieceInfo.first] |= (1ULL << sqr);
						bitboards[pieceInfo.second] |= (1ULL << sqr);
						sqr += 1;
					}
				}
				else if (section == 1) { //turn
					switch (consd) {
						case 'w': 
							toMove = white;
							toNotMove = black;
							break;
						case 'b':
							toMove = black;
							toNotMove = white;
							break;
					}
				}
				else if (section == 2) {//castling
					if (consd != '-') {
						char pos;
						int colour;
						switch (isupper(consd)) {
						case true:
							colour = 0;
							break;
						case false:
							colour = 1;
							break;
						}
						switch (tolower(consd)) {
						case 'k':
							pos = 1;
							break;
						case 'q':
							pos = 0;
							break;
						}
						castling[colour] |= 1UL << pos;
					}
				}
				else if (section == 3) {//enPassant
					string enPassantStr = fen.substr(i, 2);
					if (enPassantStr[0] >= 'a' && enPassantStr[0] <= 'h' && enPassantStr[1] > '0' && enPassantStr[1] < '9') {
						enPassant = LERF_to_sqr(fen.substr(i, 2));
					}
					else {
						enPassant = 0;
					}
				}
				i++;
			}
		}
	private:
		pair<Piece, Piece> letter_to_piece(char letter) {
			pair<Piece, Piece> pieceInfo;
			if (isupper(letter)) {
				pieceInfo.first = white;
			}
			else {
				pieceInfo.first = black;
			}
			switch (tolower(letter)) {
			case 'p':
				pieceInfo.second = pawn;
				break;
			case 'n':
				pieceInfo.second = knight;
				break;
			case 'b':
				pieceInfo.second = bishop;
				break;
			case 'r':
				pieceInfo.second = rook;
				break;
			case 'q':
				pieceInfo.second = queen;
				break;
			case 'k':
				pieceInfo.second = king;
				break;
		}
			return pieceInfo;
		}

	private: char LERF_to_sqr(string lerf) { //converts a1 to 0, c8 to 58 etc
		return ((lerf[1] - '0')-1) * 8 + (lerf[0] - 'a');
	}

	
	
};


