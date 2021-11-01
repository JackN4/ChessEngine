#pragma once
#include "Board.h"
#include "MoveCreator.h"
#include "Evaluator.h"
#include "SearchTable.h"
#include <limits>
#include <stdint.h>
using std::cout ;

//This is the file that searches for the best move
 

class Search
{
	int depthStart = 7; //The specified depth to search too
	int max = 100000; //Used as infinity
	Evaluator evaluator; 

public: Move negamax_iter(Board& board) { //Performs an iterative negamax search
	SearchTable table;
	Move bestMove;
	MoveCreator moveGen = MoveCreator(board);
	for (int i = 1; i <= depthStart; i++) { //Iterates 1 to depth start
		negamax(moveGen, i, -max, max, table); //Performs negamax
		cout << i << "\n"; //TODO:Remove this line
		print_moves(board, table, 0); //Prints current best line of moves found
	}
	bestMove = moveGen.board.get_move_from_hash(table.get_entry(moveGen.board.zobristKey).second.bestMove); //Finds best move from hash table
	table.delete_table(); //Deletes table
	return bestMove;
}

private: void print_moves(Board& board, SearchTable& table, int depth) { //Prints moves from Transposition table
	pair<bool, EntrySearch> entry = table.get_entry(board.zobristKey); //Gets current entry in transposition table
	if (entry.first) { //If entry exists
		if (entry.second.bestMove.start != entry.second.bestMove.end) { //If there is a valid move in entry
			Move move = board.get_move_from_hash(entry.second.bestMove); //Gets best move from entry
			cout << move.move_to_lerf() << " "; //Prints move
			board.make_move(move); //Makes move
			print_moves(board, table, depth + 1); //Calls function again
			board.unmake_move(move); // Unmakes move
			return;
		}
	}
	cout << "  depth:" << depth << "\n"; //TODO: Remove line
}

	//TODO: remove this
/*public: Move negamax_start(MoveCreator& moveGen, SearchTable& table, int depth) {
	Move bestMove;
	int bestScore = std::numeric_limits<int>::min();
	int score;
	pair<bool, EntrySearch> entry = table.get_entry(moveGen.board.zobristKey);
	for (Move& move : moveGen.get_all_moves()) {
		moveGen.board.make_move(move);
		score = -(negamax(moveGen, depth, -max, max, table));
		moveGen.board.unmake_move(move);
		if(score > bestScore){
			bestScore = score;
			bestMove = move;
		}
	}
	table.add(EntrySearch(moveGen.board.zobristKey, depth, bestScore, 1, bestMove));
	return bestMove;
}*/

private: int negamax(MoveCreator &moveGen, int depth, int alpha, int beta, SearchTable &table) { //Performs negamax search //TODO: allow returning scores above beta or below alpha (fail-soft)
	int origAlpha = alpha;
	pair<bool, EntrySearch> entry = table.get_entry(moveGen.board.zobristKey); //Gets entry in transposition table from key
	if (entry.first && entry.second.depth >= depth) {  //If entry depth is current depth or more
		if (entry.second.node == 1) { //Node was fully searched
			return entry.second.value;
		}
		else if (entry.second.node == 2) { //value is lower bound
			if(entry.second.value > alpha){
				alpha = entry.second.value;
			}
		}
		else if(entry.second.node == 3){ //value is upper bound
			if(entry.second.value < beta){
				beta = entry.second.value;	
			}
		}
		if(alpha >= beta){
			return entry.second.value;
		}
	}
	int score;
	if (depth == 0) { //If depth is 0 an q search must be perform to make the board stable befor evaluation takes place
		score = q_search(moveGen, alpha, beta, table);
		table.add(EntrySearch(moveGen.board.zobristKey, 0, score, 1)); //Results added to trans table
		return score;
	}
	Move bestMove;
	int bestScore = -max; //Best score starts at -inf so it can only be improved upon
	if (entry.first) { //If entry in table exist
		if (entry.second.bestMove.start != entry.second.bestMove.end) { //If it has a valid best move
			Move move = moveGen.board.get_move_from_hash(entry.second.bestMove); //Gets move from entry 
			moveGen.board.make_move(move); //Make move
			score = -(negamax(moveGen, depth - 1, -beta, -alpha, table)); //Performs negamax search
			moveGen.board.unmake_move(move); //Unmakes move
			if (score >= beta) { //Fail high
				table.add(EntrySearch(moveGen.board.zobristKey, depth, score, 2, bestMove));
				return beta;
			}
			if (score > bestScore) { //Sets bestscore and alpha
				bestMove = move;
				bestScore = score;
				if (score > alpha) {
					alpha = score;
				}
			}
		}
	}
	vector<Move> allMoves = moveGen.get_all_moves(); //Generates all moves
	if (allMoves.size() == 0) { //If there are no moves
		if (moveGen.checkers != 0) {; //If the king is in check
			score = -max; //Checkmate
		}
		else { //If not it is stalemate
			score = 0;
		}
		table.add(EntrySearch(moveGen.board.zobristKey, depth, score, 1)); //Adds result to transposition table
		return score;
	}
	
	for (Move& move : allMoves) { //Iterates through moves
		moveGen.board.make_move(move); //Makes move
		score = -(negamax(moveGen, depth - 1, -beta, -alpha, table)); //Recursively calls function to get score
		moveGen.board.unmake_move(move); //Unmakes move
		if (score >= beta) { //Fail high
			table.add(EntrySearch(moveGen.board.zobristKey, depth, score, 2, bestMove)); //Adds result to transposition table
			return beta; 
		}
		if (score > bestScore) { //Sets best score and alpha
			bestMove = move;
			bestScore = score;
			if (score > alpha) {
				alpha = score;
			}
		}
	}
	int nodeType;
	if (bestScore <= origAlpha) { //Adds results to transposition table //TODO: mightve broken
		nodeType = 3;
	}
	else{
		nodeType = 1;
	}
	table.add(EntrySearch(moveGen.board.zobristKey, depth, bestScore, nodeType, bestMove));
	return alpha; //TODO: Try to change to bestScore
}

private: int q_search(MoveCreator& moveGen, int alpha, int beta, SearchTable& table) { //TODO: add check+checkmate - search all moves if in check //This search is done after the normal search to create a stable situation on the board by just searching captures
	int currentEval;
	currentEval = evaluator.eval(moveGen.board); //We take the current evaluation as a lower bound
	if (currentEval >= beta) { //If eval is higher than beta we can return beta as we know the move is too good to ever be searched
		return beta;
	}
	if (currentEval > alpha) {
		alpha = currentEval; // We set alpha to be the current lower bound if it is higher than alpha
	}
	int score;
	int bestScore = -max;
	Move bestMove;
	vector<Move> q_moves = moveGen.get_q_moves(); //Gets just captures
	for (Move& move : q_moves) { //Iterates through moves
		if (evaluator.vals[move.pieceCapture - 2] + currentEval > alpha - 200) { // delta pruning - check if taking pieces + 200 centipawns is bigger than alpha to see if it is worth searching
			moveGen.board.make_move(move); 
			score = -q_search(moveGen, -beta, -alpha, table); //Recursively searchs deeper
			moveGen.board.unmake_move(move);
			if (score >= beta) {
				return beta; //score is high enough to prune as we know this position is too good to ever be chosen
			}
			if (score > bestScore) { //Sets best score and alpha
				bestMove = move;
				bestScore = score;
				if (score > alpha) {
					alpha = score;
				}
			}
		}
	}
	return alpha;
}
};


