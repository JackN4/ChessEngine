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


private: int negamax(MoveCreator &moveGen, int depth, int alpha, int beta, SearchTable &table) { 
	if (depth == 7) {
		cout << "test" << "\n";
	}
	pair<bool, EntrySearch> entry = table.get_entry(moveGen.board.zobristKey);
	if (entry.first) {
		if (entry.second.depth >= depth) { // can maybe do more here
			if (depth == 7) {
				cout <<"ERROR"<< "\n";
			}
			if (entry.second.node == 1) {
				return entry.second.value;
			}
			else if (entry.second.node == 2) {
				if (entry.second.value > beta) {
					return beta;
				}
			}
			else if(entry.second.node == 3){
				if (entry.second.value < alpha) {
					return alpha;
				}
			}
		}
	}
	int score;
	if (depth == 0) {
		score = q_search(moveGen, alpha, beta, table);
		//score = evaluator.eval(moveGen.board);
		table.add(EntrySearch(moveGen.board.zobristKey, 0, score, 1));
		return score;
	} 
	Move bestMove;
	int bestScore = -max;
	if (entry.first) {
		if (entry.second.bestMove.start != entry.second.bestMove.end) {
			if (depth == 7) {
				cout << "test2" << "\n";
			}
			Move move = moveGen.board.get_move_from_hash(entry.second.bestMove);
			moveGen.board.make_move(move);
			score = -(negamax(moveGen, depth - 1, -beta, -alpha, table));
			moveGen.board.unmake_move(move);
			if ((((1ULL << 8) & moveGen.board.bitboards[1]) != 0) && depth == 7) {
				cout << "ERROR2" << "\n";
			}
			if (score >= beta) {
				table.add(EntrySearch(moveGen.board.zobristKey, depth, score, 2, move));
				return beta;
			}
			if (score > bestScore) {
				bestMove = move;
				bestScore = score;
				if (score > alpha) {
					alpha = score;
				}
			}
			if (depth == 7) {
				cout << "test3" << "\n";
			}
		}
	}
	vector<Move> allMoves;
	if (depth == 7) {
		allMoves = moveGen.get_all_moves(true);
	}
	else {
		allMoves = moveGen.get_all_moves(false);
	}
	if (allMoves.size() == 0) {
		if (depth == 7) {
			cout << "testERROR" << "\n";
		}
		if (moveGen.checkers != 0) {;
			score = -max;
		}
		else {
			score = 0;
		}
		table.add(EntrySearch(moveGen.board.zobristKey, depth, score, 1));
		return score;
	}
	if (depth == 7) {
		cout << "test4" << "\n";
	}
	for (Move& move : allMoves) {
		moveGen.board.make_move(move);
		score = -(negamax(moveGen, depth - 1, -beta, -alpha, table));
		moveGen.board.unmake_move(move);
		if ((((1ULL << 8) & moveGen.board.bitboards[1]) != 0) && depth == 7) {
			cout << "ERROR3" << "\n";
		}
		if (score >= beta) {
			table.add(EntrySearch(moveGen.board.zobristKey, depth, score, 2, move));
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
	if (depth == 7) {
		cout << "test5" << "\n";
	}
	if (bestScore == alpha) {
		if (depth == 7) {
			cout << "start: " << bestMove.startPos << "\n";
			cout << "end: " << bestMove.endPos << "\n";
		}
		table.add(EntrySearch(moveGen.board.zobristKey, depth, bestScore, 1, bestMove));
	}
	else {
		table.add(EntrySearch(moveGen.board.zobristKey, depth, bestScore, 3, bestMove));
	}
	return alpha;
}

private: int q_search(MoveCreator& moveGen, int alpha, int beta, SearchTable& table) { //add check+checkmate - search all moves if in check
	pair<bool, EntrySearch> entry = table.get_entry(moveGen.board.zobristKey);
	if (entry.first) {
		if (entry.second.node == 4) {
			return entry.second.value;
		}
		else if (entry.second.node == 5) {
			if (entry.second.value > beta) {
				return beta;
			}
		}
		else if (entry.second.node == 6) {
			if (entry.second.value < alpha) {
				return alpha;
			}
		}
	}
	int currentEval;
	currentEval = evaluator.eval(moveGen.board); //we take the current evaluation as a lower bound
	if (currentEval >= beta) { //if eval is higher than beta we can return beta as we know the move is too good to ever be searched
		table.add(EntrySearch(moveGen.board.zobristKey, 0, currentEval, 5));
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
				table.add(EntrySearch(moveGen.board.zobristKey, 0, score, 5, move));
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
	if (bestScore == alpha) {
		table.add(EntrySearch(moveGen.board.zobristKey, 0, bestScore, 4, bestMove));
	}
	else {
		table.add(EntrySearch(moveGen.board.zobristKey, 0, bestScore, 6, bestMove));
	}
	return alpha;
}
};

