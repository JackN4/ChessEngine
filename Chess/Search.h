#pragma once
#include <random>
#include <functional>
#include "Board.h"
#include "MoveCreator.h"
#include "Evaluator.h"
#include "SearchTable.h"
#include <limits>
#include <stdint.h>
using std::cout;
//This is the file that searches for the best move
 

class Search
{
	int depthStart = 7; //The specified depth to search too
	int max = 100000; //Used as infinity
	Evaluator evaluator; 
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution; //TODO: make more random
	enum difficulty { easy, medium, hard , impossible}; //difficulties
	double randomness[3] = { 0.4, 0.2, 0.1 }; //How much randomness is added
	int depths[3] = { 3,4,5 }; //The depth searched to
	difficulty diff = hard; //default difficulty

public: Move start_search(Board& board, int diffIn = 3) {
	diff = (difficulty)diffIn; //Gets difficulty as enum
	int score;
	if (diff == impossible) { //Searches to maximum depth
		return negamax_iter(board).first;
	}
	else { //Limits search to a difficulty level
		generator.seed(time(NULL)); //Check if works
		SearchTable table; //Creates table
		MoveCreator moveGen = MoveCreator(board); //Makes move generator
		negamax_diff(moveGen, depths[diff], -max, max, table); //Searches with specified
		print_moves(board, table, 0);  
		Move bestMove = moveGen.board.get_move_from_hash(table.get_entry(moveGen.board.zobristKey).second.bestMove); //Finds best move from hash table
		table.delete_table(); //Deletes table
		return bestMove;
	}
	
}

public: void eval_search(Board& board) {
	MoveCreator moveGen = MoveCreator(board);
	vector<Move> allMoves = moveGen.get_all_moves(); //Generates all moves
	pair<Move, int> result;
	for (Move& move : allMoves) { //Iterates through moves
		moveGen.board.make_move(move); //Makes move
		result = negamax_iter(moveGen.board, 5, false); //Recursively calls function to get score
		cout << "move " << move.move_to_lerf() << " " << result.second << "\n";
		moveGen.board.unmake_move(move); //Unmakes move
	}
	cout << "done" << "\n";
}

public: pair<Move, int> negamax_iter(Board& board, int depth = 7, bool print = true) { //Performs an iterative negamax search
	SearchTable table;
	Move bestMove;
	MoveCreator moveGen = MoveCreator(board);
	for (int i = 1; i <= depth; i++) { //Iterates 1 to depth start
		negamax(moveGen, i, -max, max, table); //Performs negamax
		if (print) {
			cout << "Depth:" << i << "\n";
			print_moves(board, table, 0); //Prints current best line of moves found
		}
	}
	bestMove = moveGen.board.get_move_from_hash(table.get_entry(moveGen.board.zobristKey).second.bestMove); //Finds best move from hash table
	int eval = table.get_entry(moveGen.board.zobristKey).second.value;
	table.delete_table(); //Deletes table
	return std::make_pair(bestMove, eval);
}

public: void print_moves(Board& board, SearchTable& table, int depth) { //Prints moves from Transposition table
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
	cout << "\n";
}

int random_score(int score) {
	double multi = 2*randomness[diff]*(distribution(generator)-0.5);
	int output = (score * (1 + multi));
	return (score * (1+multi));
}

private: int negamax(MoveCreator &moveGen, int depth, int alpha, int beta, SearchTable &table) { //Performs negamax search 
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
	int bestScore = -(max + 1); //Best score starts at -inf so it can only be improved upon
	if (entry.first) { //If entry in table exist
		if (entry.second.bestMove.start != entry.second.bestMove.end) { //If it has a valid best move
			Move move = moveGen.board.get_move_from_hash(entry.second.bestMove); //Gets move from entry 
			moveGen.board.make_move(move); //Make move
			score = -negamax(moveGen, depth - 1, -beta, -alpha, table); //Performs negamax search
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
		score = -negamax(moveGen, depth - 1, -beta, -alpha, table); //Recursively calls function to get score
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
	if (bestScore <= origAlpha) { //Adds results to transposition table
		nodeType = 3;
	}
	else{
		nodeType = 1;
	}
	table.add(EntrySearch(moveGen.board.zobristKey, depth, bestScore, nodeType, bestMove));
	return alpha;
}

private: int negamax_diff(MoveCreator& moveGen, int depth, int alpha, int beta, SearchTable& table) { //Performs negamax search to specified difficulty
	int origAlpha = alpha;
	pair<bool, EntrySearch> entry = table.get_entry(moveGen.board.zobristKey); //Gets entry in transposition table from key
	if (entry.first && entry.second.depth >= depth) {  //If entry depth is current depth or more
			if (entry.second.node == 1) { //Node was fully searched
				return entry.second.value;
			}
			else if (entry.second.node == 2) { //value is lower bound
				if (entry.second.value > alpha) {
					alpha = entry.second.value;
				}
			}
			else if (entry.second.node == 3) { //value is upper bound
				if (entry.second.value < beta) {
					beta = entry.second.value;
				}
			}
			if (alpha >= beta) {
				return entry.second.value;
			}
		}
	int score;
	if (depth == 0) { //If depth is 0 an q search must be perform to make the board stable befor evaluation takes place
		score = random_score(q_search(moveGen, alpha, beta, table)); //Adds randomness to the returned score
		table.add(EntrySearch(moveGen.board.zobristKey, 0, score, 1)); //Results added to trans table
		return score;
	}
	Move bestMove;
	int bestScore = -(max + 1); //Best score starts at -inf so it can only be improved upon
	if (entry.first) { //If entry in table exist
		if (entry.second.bestMove.start != entry.second.bestMove.end) { //If it has a valid best move
			Move move = moveGen.board.get_move_from_hash(entry.second.bestMove); //Gets move from entry 
			moveGen.board.make_move(move); //Make move
			score = random_score(-(negamax(moveGen, depth - 1, -beta, -alpha, table))); //Adds randomness to the returned score
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
		if (moveGen.checkers != 0) {
			; //If the king is in check
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
		score = random_score(-(negamax(moveGen, depth - 1, -beta, -alpha, table))); //Adds randomness to the returned score
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
	if (bestScore <= origAlpha) { //Adds results to transposition table
		nodeType = 3;
	}
	else {
		nodeType = 1;
	}
	table.add(EntrySearch(moveGen.board.zobristKey, depth, bestScore, nodeType, bestMove));
	return alpha; 
}

private: int q_search(MoveCreator& moveGen, int alpha, int beta, SearchTable& table) {//This search is done after the normal search to create a stable situation on the board by just searching captures
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


