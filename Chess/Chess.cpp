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

using namespace N;
using namespace std;

string engineName = "Jack's engine";
string author = "Jack";


int main()
{
    Lookups lookups;
    Board board; //Create Board
    BoardDisplay display;
    string input;
    while (true) {
        getline(cin, input);
        if (input == "uci") {
            cout << "id name" << engineName << "\n";
            cout << "id author" << author << "\n";
            cout << "uciok" << "\n";
        }
        else if (input == "ucinewgame") {
            cout << "newGame" << "\n";
            //engine setup message
        }
        else if (input.rfind("setoption", 0) == 0) {
            cout << "setOptions" << "\n";
            //setup for new game
        }
        else if (input == "isready") {
            cout << "readyok" << "\n";
        }
        else if (input.rfind("position", 0) == 0) {
            string FEN;
            int movesPos = input.find("moves");
            if (input.find("startpos") != string::npos) {
                FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
            }
            else {
                if (movesPos != string::npos) {
                    FEN = input.substr(9);
                }
                else {
                    FEN = input.substr(9, movesPos - 11);
                }
            }
            board.create_from_FEN(FEN);
            if (movesPos != string::npos) {
                std::istringstream iss(input.substr(movesPos+5));
                string move;
                while (std::getline(iss, move, ' ')) {
                    if (move.length() == 4 || move.length() == 5) {
                        board.make_move_notation(move);
                    }
                }
            }
            display.display_board(board);
        }
        else if (input == "moves debug") {
            MoveCreator moveGen = MoveCreator(board);
            list<Move> moves = moveGen.get_all_moves();
            display.display_all_moves(moves);
        }
        else if (input.rfind("perft", 0) == 0) {
            Perft perft;
            string depthStr = input.substr(6);
            depthStr.erase(remove_if(depthStr.begin(), depthStr.end(), isspace), depthStr.end());
            int depth = stoi(depthStr);
            perft.calculate_perft(board, depth);
        }
    }
}



