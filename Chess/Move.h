#pragma once
#include <list>
#include <stdint.h>
#include <string>
#include "Constants.h"
using namespace N;
using std::list;
using std::string;

//This defines the structure Move which is used to store all needed info about a move


struct Move
{
//Position before move, position after move, if move is a castle, the castling rights before move, en passant rights before move
int startPos, endPos, castling, castlingBefore[2], enPassantBefore; 

//Type of piece in move, piece that was captured, if there is a promotion what piece to promote to
Piece pieceType, pieceCapture, promoPiece; 

//If move is en passant
bool enPassant;

public: Move(Piece pieceIn, int startPosin, int endPosIn, Piece pieceCIn=white, int castlingIn = 0, Piece promoPieceIn = white, int enPassantIn = 0) { //Sets all variables to given arguments
	pieceType = pieceIn;
	pieceCapture = (Piece)pieceCIn;
	castling = castlingIn; //kq does it castle
	promoPiece = (Piece)promoPieceIn;
	startPos = startPosin;
	endPos = endPosIn;
	enPassant = enPassantIn;//the piece that is remove
}

public: Move() { //Creates empty move
	startPos = 0;
	endPos = 0;
}



public: string move_to_lerf() { //Turns move into standard notation
	string result;
	result += sqr_to_lerf(startPos);
	result += sqr_to_lerf(endPos);
	switch (promoPiece) { 
	case white:
		break;
	case knight:
		result += 'n';
		break;
	case bishop:
		result += 'b';
		break;
	case rook:
		result += 'r';
		break;
	case queen:
		result += 'q';
		break;
	}
	return result;
}

private: string sqr_to_lerf(int sqr) { //Gets the rank and file of a square at turns it into standard notation
	char rank = (sqr >> 3) + '1';
	char file = (sqr & 7) + 'a';
	string result;
	result += file;
	result += rank;
	return result;
}
};

