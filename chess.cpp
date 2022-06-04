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

void play_chess_one_turn(Board &board, Player &player) {
    cout << board << endl;
    cout << player.name() << "'s turn." << endl;
    vector<Move> moves = board.get_moves();
    Move move;
    while (true) {
        move = player.get_move(board, moves);
        if (find(moves.begin(), moves.end(), move) != moves.end()) {
            break;
        }
    }
    cout << player.name() << " chose to move " << board[move.from]
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
    cout << team_name(winner) << " won!\n";
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
    // cout << "NONE won " << win_counts[0] << endl;
    // cout << "BLACK won " << win_counts[1] << endl;
    // cout << "WHITE won " << win_counts[2] << endl;

    Board b(10, 10);
    CapturePlayer white_player(WHITE);
    CapturePlayer black_player(BLACK);

    play_chess_one_turn(b, white_player);

    ofstream out("./out.txt");
    out << b;

    Board c(4, 99);
    play_chess_one_turn(c, white_player);
    out << c;
    out.close();

    ifstream in("./out.txt");
    Board b1;
    in >> b1;
    cout << b1 << endl;

    Board b2;
    in >> b2;
    cout << b2 << endl;

    // HumanPlayer white_player(WHITE);
    // CapturePlayer black_player(BLACK);
    // play_one_chess_game(white_player, black_player);

    return 0;
}
