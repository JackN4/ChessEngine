#pragma once
#include "Board.h"
#include "MoveCreator.h"
#include "Evaluator.h"
#include "SearchTable.h"
#include <limits>
#include <stdint.h>
using std::cout ;


 

class Search
{
	int depthStart = 7;
	int max = 100000;
	Evaluator evaluator;

public: Move negamax_iter(Board& board) {
	SearchTable table;
	Move bestMove;
	MoveCreator moveGen = MoveCreator(board);
	for (int i = 1; i <= depthStart; i++) {
		negamax(moveGen, i, -max, max, table);
		cout << i << "\n";
		print_moves(board, table, 0);
	}
	bestMove = moveGen.board.get_move_from_hash(table.get_entry(moveGen.board.zobristKey).second.bestMove);
	table.delete_table();
	return bestMove;
}

private: void print_moves(Board& board, SearchTable& table, int depth) {
	pair<bool, EntrySearch> entry = table.get_entry(board.zobristKey);
	if (entry.first) {
		if (entry.second.bestMove.start != entry.second.bestMove.end) {
			Move move = board.get_move_from_hash(entry.second.bestMove);
			cout << move.move_to_lerf() << " ";
			board.make_move(move);
			print_moves(board, table, depth + 1);
			board.unmake_move(move);
			return;
		}
	}
	cout << "  depth:" << depth << "\n";
}

public: Move negamax_start(MoveCreator& moveGen, SearchTable& table, int depth) {
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
}

private: int negamax(MoveCreator &moveGen, int depth, int alpha, int beta, SearchTable &table) { 
	pair<bool, EntrySearch> entry = table.get_entry(moveGen.board.zobristKey);
	if (entry.first) {
		if (entry.second.depth == depth) { // can maybe do more here
			if (entry.second.node == 1) {
				return entry.second.value;
			}
			else if (entry.second.node == 2) {
				if (entry.second.value > beta) {
					return beta;
				}
			}
			else {
				if (entry.second.value < alpha) {
					return alpha;
				}
			}
		}
	}
	int score;
	if (depth == 0) {
		score = q_search(moveGen, alpha, beta, table);
		table.add(EntrySearch(moveGen.board.zobristKey, 0, score, 1));
		return score;
	}
	Move bestMove;
	int bestScore = -max;
	if (entry.first) {
		if (entry.second.bestMove.start != entry.second.bestMove.end) {
			Move move = moveGen.board.get_move_from_hash(entry.second.bestMove);
			moveGen.board.make_move(move);
			score = -(negamax(moveGen, depth - 1, -beta, -alpha, table));
			moveGen.board.unmake_move(move);
			if (score >= beta) {
				table.add(EntrySearch(moveGen.board.zobristKey, depth, score, 2, bestMove));
				return beta;
			}
			if (score > bestScore) {
				bestMove = move;
				bestScore = score;
				if (score > alpha) {
					alpha = score;
				}
			}
		}
	}
	vector<Move> allMoves = moveGen.get_all_moves();
	if (allMoves.size() == 0) {
		if (moveGen.checkers != 0) {;
			score = -max;
		}
		else {
			score = 0;
		}
		table.add(EntrySearch(moveGen.board.zobristKey, depth, score, 1));
		return score;
	}
	
	for (Move& move : allMoves) {
		moveGen.board.make_move(move);
		score = -(negamax(moveGen, depth - 1, -beta, -alpha, table));
		moveGen.board.unmake_move(move);
		if (score >= beta) {
			table.add(EntrySearch(moveGen.board.zobristKey, depth, score, 2, bestMove));
			return beta;
		}
		if (score > bestScore) {
			bestMove = move;
			bestScore = score;
			if (score > alpha) {
				alpha = score;
			}
		}
	}
	if (bestScore == alpha) {
		table.add(EntrySearch(moveGen.board.zobristKey, depth, bestScore, 1, bestMove));
	}
	else {
		table.add(EntrySearch(moveGen.board.zobristKey, depth, bestScore, 3, bestMove));
	}
	return alpha;
}

private: int q_search(MoveCreator& moveGen, int alpha, int beta, SearchTable& table) { //add check+checkmate - search all moves if in check
	int currentEval;
	currentEval = evaluator.eval(moveGen.board); //we take the current evaluation as a lower bound
	if (currentEval >= beta) { //if eval is higher than beta we can return beta as we know the move is too good to ever be searched
		return beta;
	}
	if (currentEval > alpha) {
		alpha = currentEval; // we set alpha to be the current lower bound if it is higher than alpha
	}
	int score;
	int bestScore = -max;
	Move bestMove;
	vector<Move> q_moves = moveGen.get_q_moves();
	for (Move& move : q_moves) {
		if (evaluator.vals[move.pieceCapture - 2] + currentEval > alpha - 200) { // delta pruning - check if taking pieces + 200 centipawns is bigger than alpha to see if it is worth searching
			moveGen.board.make_move(move);
			score = -q_search(moveGen, -beta, -alpha, table);
			moveGen.board.unmake_move(move);
			if (score >= beta) {
				return beta; //score is high enough to prune as we know this position is too good to ever be chosen
			}
			if (score > bestScore) {
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


