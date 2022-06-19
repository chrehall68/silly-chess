#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <vector>

#include "chess_board.h"
#include "chess_pieces.h"
#include "chess_player.h"

using namespace std;

#include <fstream>
ofstream out("out.txt");

void play_chess_one_turn(Board &board, Player &player) {
    out << board << endl;
    out << player.name() << "'s turn." << endl;
    vector<Move> moves = board.get_moves();
    Move move;
    while (true) {
        move = player.get_move(board, moves);
        if (find(moves.begin(), moves.end(), move) != moves.end()) {
            break;
        }
    }
    out << player.name() << " chose to move " << board[move.from]
        << " from " << move.from << " to " << move.to << " ("
        << board[move.to] << ")\n\n";
    board.make_move(move);
}

Team play_one_chess_game(Player &white_player, Player &black_player) {
    Board board;
    vector<Move> moves;
    while (true) {
        play_chess_one_turn(board, white_player);
        if (board.winner() != NONE) {
            break;
        }
        play_chess_one_turn(board, black_player);
        if (board.winner() != NONE) {
            break;
        }
    }
    Team winner = board.winner();
    out << team_name(winner) << " won!\n";
    return winner;
}

int main(int argc, const char *argv[]) {
    // HumanPlayer white_player(WHITE);
    // CapturePlayer white_player(WHITE);
    // CheckMateCapturePlayer black_player(BLACK);

    // map<int, long> win_counts = {{0, 0}, {1, 0}, {2, 0}};
    // for (int i = 0; i < 100000; i++) {
    //     ++win_counts[play_one_chess_game(white_player, black_player)];
    // }
    // out << "NONE won " << win_counts[0] << endl;
    // out << "BLACK won " << win_counts[1] << endl;
    // out << "WHITE won " << win_counts[2] << endl;

    // Board b(10, 10);
    // CapturePlayer white_player(WHITE);
    // CapturePlayer black_player(BLACK);

    // play_chess_one_turn(b, white_player);

    // ofstream out("./out.txt");
    // out << b;

    // Board c(4, 99);
    // play_chess_one_turn(c, white_player);
    // out << c;
    // out.close();

    // ifstream in("./out.txt");
    // Board b1;
    // in >> b1;
    // out << b1 << endl;

    // Board b2;
    // in >> b2;
    // out << b2 << endl;

    map<int, int> win_counts = {{1, 0},
                                {2, 0}};
    AIPlayer white_player(WHITE);
    CheckMateCapturePlayer black_player(BLACK);
    for (int i = 0; i < 1000; ++i) {
        ++win_counts[play_one_chess_game(white_player, black_player)];

        out << "\n\n----------Next Game----------\n\n";

        if (i % 100 == 0 && i!= 0) {
            cout << "out of 100, WHITE WON " << win_counts[WHITE] << " and BLACK WON " << win_counts[BLACK] << endl;
            win_counts[WHITE] = 0;
            win_counts[BLACK] = 0;
        }
    }

    return 0;
}
