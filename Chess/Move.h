#pragma once
#include <list>
#include <stdint.h>
#include <string>
#include "Constants.h"
using namespace N;
using std::list;
using std::string;


struct Move
{
int startPos, endPos, castling, castlingBefore;
// int enPassantBefore;
Piece pieceType, pieceCapture, promoPiece;
//public: list<Pinned> addPinned[2], removedPinned[2];
bool enPassant;

public: Move(Piece pieceIn, int startPosin, int endPosIn, Piece pieceCIn=white, int castlingIn = 0, Piece promoPieceIn = white, int enPassantIn = 0) {
	pieceType = pieceIn;
	pieceCapture = (Piece)pieceCIn;
	castling = castlingIn; //kq does it castle
	castlingBefore = 0; //castling rights before move
	promoPiece = (Piece)promoPieceIn;
	startPos = startPosin;
	endPos = endPosIn;
	enPassant = enPassantIn;//the piece that is remove
}

public: string move_to_lerf() {
	string result;
	result += sqr_to_lerf(startPos);
	result += sqr_to_lerf(endPos);
	return result;
}

private: string sqr_to_lerf(int sqr) {
	char rank = (sqr >> 3) + '1';
	char file = (sqr & 7) + 'a';
	string result;
	result += file;
	result += rank;
	return result;
}
};

