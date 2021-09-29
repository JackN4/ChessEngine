#pragma once
#include <stdint.h>
#include <string>
#include <stack>
#include "Move.h"
#include "BitOperations.h"
#include "Zobrist.h"
#include "SearchTable.h"

using std::string;
using std::pair;
using namespace N;




class Board
{

public: uint64_t bitboards[8] = {0}; //All bitboards needed to store bitboards
//public: stack<Move> moves;
	int toMove= 0;//Which colour is moving
	int toNotMove = 0; //Which colour isnt moving
	int castling[2] = { 0,0 }; //KQkq
public: uint64_t zobristKey = 0; //The current zobrist key for board
	Zobrist zobrist = Zobrist(); //Gets zobrist info
	int enPassant; //Information about en passant for next move


	private: void change_zobrist_piece(int colour, Piece piece, int sqr) {
		zobristKey ^= zobrist.pieces[colour][piece - 2][sqr]; //Add or remove piece to zobrist key
	}

	private: void change_zobrist_castle(int colour, int side) {
		zobristKey ^= zobrist.castling[colour][side]; //Change castling rights in zobrist key
	}

	private: void change_zobrist_multiple_castle(int colour, int changes) { //colour to change and bits to change (01, 10, 11)
		int lsb; //Changes multiple castling rights at once
		if (changes & 1) { // odd
			zobristKey ^= zobrist.castling[colour][0];
		}
		if (changes & 2) {
			zobristKey ^= zobrist.castling[colour][1];
		}
	}

	private: void change_zobrist_enPassant(int sqr) { //Change en passant rights for zobrist key
		if (sqr != 0) {
			zobristKey ^= zobrist.enPassant[sqr % 8];
		}
	}


private: void check_for_castle(Move& move) { //Checks if changes are needed to castling rights or perform some castling
	int castlingRemove[2] = { 0,0 };
	move.castlingBefore[0] = castling[0];
	move.castlingBefore[1] = castling[1];
	if (move.pieceType == king) { //If king is moving then no castling can occur after
		castlingRemove[toMove] = 3;
		if (move.castling != 0) {
			int row = 56 * toMove;
			if (move.castling == 1) { //queenside
				move_piece(rook, 0 + row, 3 + row, toMove);
			}
			else { //kingside
				move_piece(rook, 7 + row, 5 + row, toMove);
			}
		}
	}
	else if (move.pieceType == rook) { // rook move so removes castling rights on that side
		if (move.startPos == 0 + 56 * toMove) { //queenside
			castlingRemove[toMove] |= 1U;
		}
		if (move.startPos == 7 + 56 * toMove) { //kingside
			castlingRemove[toMove] |= 1U << 1;
		}
	}
	if (move.pieceCapture == rook) { //rook capture so can no longer castle whenever
		if (move.endPos == 0 + 56 * toNotMove) { //queenside
			castlingRemove[toNotMove] |= 1U;
		}
		if (move.endPos == 7 + 56 * toNotMove) { //kingside
			castlingRemove[toNotMove] |= 1U << 1;
		}
	}
	change_zobrist_multiple_castle(0, castling[0] & castlingRemove[0]); //These lines change castling rights
	change_zobrist_multiple_castle(1, castling[1] & castlingRemove[1]); 
	castling[0] &= ~(castlingRemove[0]); 
	castling[1] &= ~(castlingRemove[1]);
}

	private: void change_turn() { //Swaps which colur is playing
		toNotMove = toMove;
		toMove ^= 1;
		zobristKey ^= zobrist.turn;
	}

	private: void check_for_en_passant(Move &move) { //Check if pawn moves affects en passants or performs some en passant 
		move.enPassantBefore = enPassant;
		if (move.enPassant) { //If move is en passant it removes the taken piece
			if (toMove == 0) {
				remove_piece(pawn, enPassant - 8, toNotMove);
			}
			else {
				remove_piece(pawn, enPassant + 8, toNotMove);
			}
			change_zobrist_enPassant(enPassant);
			enPassant = 0;
			return;
		}
		else if (move.pieceType == pawn && abs(move.startPos - move.endPos) == 16) { //Makes en passant possible for next move
			int sqr = move.endPos + (move.startPos - move.endPos) / 2;
			change_zobrist_enPassant(enPassant);
			enPassant = sqr;
			change_zobrist_enPassant(enPassant);
		}
		else {
			change_zobrist_enPassant(enPassant);
			enPassant = 0;
		}
	}

	public : void make_move(Move &move) { //Performs moves
		check_for_castle(move);
		check_for_en_passant(move);
		if (move.pieceCapture != 0 && !(move.enPassant)) { //If there was a capture then it removes the piece
			remove_piece(move.pieceCapture, move.endPos, toNotMove);
		}
		if (move.promoPiece == 0) { //Moves the piece if it is not promotion
			move_piece(move.pieceType, move.startPos, move.endPos, toMove);
		}
		else {	//If there is promotion then removes old piece and adds new one
			remove_piece(move.pieceType, move.startPos, toMove);
			add_piece(move.promoPiece, move.endPos, toMove);
		}
		change_turn();
	}

	private: void move_piece(Piece piece, int startPos, int endPos, int colour) { //Moves piece from one square to another
		remove_piece(piece, startPos, colour);
		add_piece(piece, endPos, colour);
	}

public: void unmake_move(Move& move) { //undoes move
	change_turn();
	unmake_castling(move);
	unmake_enPassant(move);
	if (move.promoPiece == 0) {//If no promotion then piece is moved back
		move_piece(move.pieceType, move.endPos, move.startPos, toMove);
	}
	else {
		add_piece(move.pieceType, move.startPos, toMove); //Adds promoted piece
		remove_piece(move.promoPiece, move.endPos, toMove); //Removes old piece
	}
	if (move.pieceCapture != 0 && !(move.enPassant)) {
		add_piece(move.pieceCapture, move.endPos, toNotMove); //Re adds piece that was captured
	}
}

private: void unmake_enPassant(Move& move) { //Undoes enpassant
	change_zobrist_enPassant(enPassant); //Removes enpassant rights to zobrist
	change_zobrist_enPassant(move.enPassantBefore); //Adds old enpassant rights to zobrist
	enPassant = move.enPassantBefore; //Undoes changes to enpassant rights
	if (move.enPassant) { //If piece was taken by enpassant it is readded
		if (toMove == 0) {
			add_piece(pawn, move.endPos - 8, toNotMove);
		}
		else {
			add_piece(pawn, move.endPos + 8, toNotMove);
		}
	}
}

private: void unmake_castling(Move& move) { //Undoes castling
	change_zobrist_multiple_castle(0, castling[0] ^ move.castlingBefore[0]); //If there was changes to castling rights they are changed in the zobrist key
	change_zobrist_multiple_castle(1, castling[1] ^ move.castlingBefore[1]);
	castling[0] = move.castlingBefore[0]; //Changes castling rights to old castling rights
	castling[1] = move.castlingBefore[1];
	if (move.castling != 0) { //If move was castle perform
		int row = 56 * toMove;
		if (move.castling == 1) {
			move_piece(rook, 3 + row, 0 + row, toMove);
		}
		else {
			move_piece(rook,  5 + row, 7 + row, toMove);
		}
	}
}

public: void make_move_notation(string moveStr) { //Make a move given the standard notation for the move
	Move move = get_move_notation(moveStr);
	make_move(move);
}

private: Move get_move_notation(string moveStr) { //assume move is legal - maybe check against list of legal moves
	int startPos = LERF_to_sqr(moveStr.substr(0, 2)); //Find the squares of the moves
	int endPos = LERF_to_sqr(moveStr.substr(2, 2));
	Piece pieceType = get_piece_from_pos(startPos); //Gets piece from square
	Piece captured;
	if (((get_all_BB() >> endPos) & 1ULL) == 1) {//If there  was a piece on the end square it 
		captured = get_piece_from_pos(endPos);
	}
	else {//If not capture
		captured = (Piece)0;
	}
	if (pieceType == pawn) {
		if (enPassant != 0 && endPos == enPassant) { //If move was enPassant
			return Move(pawn, startPos, endPos, pawn, 0, white, enPassant);
		}
		else if ((endPos >> 3) == 0 || (endPos >> 3) == 7) { //If the move is a promotion
			char promo = moveStr.at(4);
			Piece promoPiece = letter_to_piece(promo).second;
			return Move(pawn, startPos, endPos, captured, 0, promoPiece, 0);
		}
	}
	else if (pieceType == king) {
		int colourRank = toMove * 56;
		if (startPos == 4 + colourRank) { //Checks if move is castle
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


public: Piece get_piece_from_pos(int pos) { //Find what piece is at a specific pos
	for (int i = 2; i < 8; i++) {
		if (((bitboards[i] >> pos) & 1ULL) == 1) {
			return (Piece)i;
		}
	}
	return (Piece)0;
}


private: void remove_piece(Piece piece, int pos, int colour) {
	bitboards[colour] &= ~(1ULL << pos); //removes from specific colour bitboard
	bitboards[piece] &= ~(1ULL << pos); //removes from piece bitboard
	change_zobrist_piece(colour, piece, pos); //changes zobrist key
	}

private: void add_piece(Piece piece, int pos, int colour) {
	bitboards[colour] |= 1ULL << pos; //adds to colour bitboard
	bitboards[piece] |= 1ULL << pos; //adds to piece bitboard
	change_zobrist_piece(colour, piece, pos); //changes zobrist
}

	public: Move get_move_from_hash(MoveStore hash) {
		Piece piece = get_piece_from_pos(hash.start);
		Piece pieceCapture = get_piece_from_pos(hash.end);
		if (hash.special != 0) {
			if (piece == king) {
				Move move = Move(king, hash.start, hash.end, white, hash.special);
				return move;
			}
			else if (piece == pawn) {
				if (hash.special < 7) {
					Move move = Move(pawn, hash.start, hash.end, pieceCapture, 0, (Piece)hash.special);
					return move;
				}
				else {
					Move move = Move(pawn, hash.start, hash.end, pieceCapture, 0, white, hash.special);
					return move;
				}
			}
			else {
				cout << "ERROR" << "\n";
			}
		}
		else {
			return Move(piece, hash.start, hash.end, pieceCapture);
		}
	}

	public : 
		uint64_t get_piece_BB(Piece colour, Piece piece) { //Finds bitboard for specific piece of 1 colour
			return bitboards[colour] & bitboards[piece]; 
		}

	public:
		uint64_t get_piece_BB(int colour, Piece piece) { //Finds bitboard for specific piece of 1 colour
			return get_piece_BB((Piece)colour, piece);
		}

	public:
		uint64_t get_all_BB() { //Get biboard of all pieces
			return bitboards[white] | bitboards[black];
		}

	public:
		void create_from_FEN(string fen) { //Creates a board from a FEN string
			zobristKey = 0; //sets zobrist key to 0
			for (int i = 0; i < 8; i++) {//sets all boards to 0
				bitboards[i] = 0;
			}
			int64_t sqr = 56; //starts on last square
			int i = 0;
			int section = 0; //represents which section of fen string it is looking at
			while(section < 4) {
				char consd = fen[i]; //The character currently being considered 
				if (consd == ' ') { //Move to next section
					section += 1;
				}
				else if (section == 0) {//Section represeting where pieces are
					if (consd == '/') { // new row
						sqr -= 16;
					}
					else if (consd > '0' && consd < '9') { //if consd is a number it represents empty space
						sqr += int64_t(consd) - '0';
					}
					else { //It is a piece
						pair<Piece, Piece> pieceInfo = letter_to_piece(consd); //Checks what piece the letter represents and what colour
						bitboards[pieceInfo.first] |= (1ULL << sqr); //Adds to colour bitboard
						bitboards[pieceInfo.second] |= (1ULL << sqr); //Adds to piece bitboard
						change_zobrist_piece(pieceInfo.first, pieceInfo.second, sqr); //Adds piece to zobrist key
						sqr += 1;
					}
				}
				else if (section == 1) { //Section which states which player's turn it is
					switch (consd) {
						case 'w': //Whites move
							toMove = white;
							toNotMove = black;
							break;
						case 'b': //Blacks move
							toMove = black;
							toNotMove = white;
							zobristKey ^= zobrist.turn;
							break;
					}
				}
				else if (section == 2) {//Castling section
					if (consd != '-') { // '-' would mean no castling rights
						char pos;
						int colour;
						switch (isupper(consd)) { 
						case true:
							colour = 0; //white
							break;
						case false:
							colour = 1; //black
							break;
						}
						switch (tolower(consd)) {
						case 'k':
							pos = 1; //kingside
							break;
						case 'q':
							pos = 0; //queenside
							break;
						}
						castling[colour] |= 1UL << pos; //Changes castling rights
						change_zobrist_castle(colour, pos); //Changes castling rights in zobrist
					}
				}
				else if (section == 3) {//enPassant section
					if (consd != '-') { //If there is enPassant
						string enPassantStr = fen.substr(i, 2); //Gets whole enPassant string
						if (enPassantStr[0] >= 'a' && enPassantStr[0] <= 'h' && enPassantStr[1] > '0' && enPassantStr[1] < '9') {
							int sqr = LERF_to_sqr(fen.substr(i, 2)); //Gets enPassant square
							enPassant = sqr; //Changes enPassant
							change_zobrist_enPassant(sqr); //Changes enPassant to zobrist key
						}

					}
					else {
						enPassant = 0;
						section += 1;//MIGHT ADD HALF MOVE DRAW
					}
				}
				i++;
			}
		}

	private:
		pair<Piece, Piece> letter_to_piece(char letter) {//Finds colour and type of piece from letter
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


