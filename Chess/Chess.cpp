#include <stdint.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include <string>
#include "BoardDisplay.h"
#include "Lookups.h"
#include "MoveCreator.h"
#include "Perft.h"
#include "Evaluator.h"
#include "SearchDifficulty.h"

using namespace N;
using namespace std;

string engineName = "Jack's engine";
string author = "Jack";

//This is the file where the program starts and interacts with command line

int main()
{
    Lookups lookups; //Creates lookup tables
    Board board; //Create Board
    BoardDisplay display; //Creates object to display board
    string input; //Creates string to get input
    bool debug = false; //changes debug mode to show more move
    while (true) { //Runs until program ends
        getline(cin, input); //Get user input
        if (input == "uci") { //Prints engine info
            cout << "id name" << engineName << "\n";
            cout << "id author" << author << "\n";
            cout << "uciok" << "\n";
        }
        else if (input == "ucinewgame") { //Sets up for new game
            cout << "newGame" << "\n";
        }
        else if (input.rfind("setoption", 0) == 0) { //Sets options
            cout << "setOptions" << "\n";
            //setup for new game
        }
        else if (input == "isready") { //Says its ready for new command
            cout << "readyok" << "\n";
        }
        else if (input.rfind("position", 0) == 0) { //Takes in new board position
            string FEN;
            int movesPos = input.find("moves");
            if (input.find("startpos") != string::npos) { //Sets the FEN string to starting position
                FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
            }
            else if (input.find("fen") != string::npos) { //Gets FEN string if its not start position
                if (movesPos == string::npos) {
                    FEN = input.substr(13);
                }
                else {
                    FEN = input.substr(13, movesPos - 14);
                }
            }
            board.create_from_FEN(FEN);
            if (movesPos != string::npos) { //Finds the strings for the moves which have been played and makes the moves on the board
                std::istringstream iss(input.substr(movesPos + 5));
                string move;
                while (std::getline(iss, move, ' ')) {
                    if (move.length() == 4 || move.length() == 5) {
                        board.make_move_notation(move); //Makes move on board using move string
                    }
                }
            }
            if (debug) {
                display.display_board(board);
            }
        }
        else if (input == "moves debug") { //Gets list of current moves for debugging purposes
            MoveCreator moveGen = MoveCreator(board);
            vector<Move> moves = moveGen.get_all_moves();
            display.display_all_moves(moves);
        }
        else if (input == "display") { //Displays boards
            display.display_board(board);
        }
        else if (input.rfind("perftNB", 0) == 0) { //Performs perft without batch counting
            Perft perft;
            string depthStr = input.substr(8); //Gets depth to seach to
            depthStr.erase(remove_if(depthStr.begin(), depthStr.end(), isspace), depthStr.end());
            int depth = stoi(depthStr);
            perft.calculate_perft(board, depth, debug);
        }
        else if (input.rfind("perftTT", 0) == 0) { //Performs perft using the transposition table
            Perft perft;
            string depthStr = input.substr(8);//Gets depth to seach to
            depthStr.erase(remove_if(depthStr.begin(), depthStr.end(), isspace), depthStr.end());
            int depth = stoi(depthStr);
            perft.calculate_perft_TT(board, depth, debug);
        }
        else if (input.rfind("perft", 0) == 0) { //Performs perft with batch testing
            Perft perft;
            string depthStr = input.substr(6);
            depthStr.erase(remove_if(depthStr.begin(), depthStr.end(), isspace), depthStr.end());
            int depth = stoi(depthStr);
            perft.calculate_perft_bulk(board, depth, debug);
        }
        else if (input == "debug") { //Toggles debug mode
            if (debug) {
                cout << "debug mode is now off\n";
                debug = false;
            }
            else {
                cout << "debug mode is now on\n";
                debug = true;
            }
        }
        else if (input == "eval") { //Prints current static evaluation of board
            Evaluator evaluator;
            int score = evaluator.eval(board);
            cout << "eval: " << score << "\n";
        }
        else if (input.rfind("go", 0) == 0) { //Finds best move
            Move bestMove;
            if (input.find("diff") != string::npos) { // If difficulty is specified
                string depthStr = input.substr(8);
                int depth = stoi(depthStr);
                SearchDifficulty searchDiff;
                bestMove = searchDiff.search_diff(board, depth);
            }
            else { //Otherwise full depth search
                Search search;
                bestMove = search.negamax_iter(board); //Searchs for best  move
            }
            cout << "bestmove " << bestMove.move_to_lerf() << "\n"; //Outputs best move
        }
        else if (input == "zobrist") { //Outputs current zobrist key
            cout << (board.zobristKey) << "\n";
        }
        else {
            cout << "that command is not recognised\n";
        }
    }
}



