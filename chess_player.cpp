#include "chess_player.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

#include "chess_board.h"
#include "chess_pieces.h"

using std::cin;
using std::cout;
using std::endl;
using std::shuffle;
using std::vector;

const char *Player::name() const {
    return team_name(team);
}

RandomPlayer::RandomPlayer(Team team) : Player(team) {
    // Initialize the pseudo-random number generator based on the current time,
    // so it chooses different numbers when you run the code at different times.
    random_number_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

Move RandomPlayer::get_move(const Board &board, const vector<Move> &moves) const {
    return moves[random_number_generator() % moves.size()];
}

HumanPlayer::HumanPlayer(Team team) : Player(team) {}

Move HumanPlayer::get_move(const Board &board, const vector<Move> &moves) const {
    Move move;
    while (true) {
        cout << "What's your move?: ";
        cin >> move;
        cout << endl;
        if (find(moves.begin(), moves.end(), move) != moves.end()) {
            break;
        }
        cout << move << " is not a valid move! Please choose one of the following moves: \n";
        for (Move valid_move : moves) {
            cout << valid_move << ' ';
        }
        cout << endl;
    }
    return move;
}

CapturePlayer::CapturePlayer(Team team) : Player(team) {
    // Initialize the pseudo-random number generator based on the current time,
    // so it chooses different numbers when you run the code at different times.
    random_number_generator.seed(
        std::chrono::system_clock::now().time_since_epoch().count());
}

Move CapturePlayer::get_move(const Board &board, const vector<Move> &moves) const {
    vector<Move> shuffled_moves = moves;
    shuffle(shuffled_moves.begin(), shuffled_moves.end(), random_number_generator);
    for (Move move : shuffled_moves) {
        if (board[move.from].is_opposite_team(board[move.to])) {
            return move;
        }
    }
    return shuffled_moves[0];
}

CheckMateCapturePlayer::CheckMateCapturePlayer(Team team) : Player(team) {
    // Initialize the pseudo-random number generator based on the current time,
    // so it chooses different numbers when you run the code at different times.
    random_number_generator.seed(
        std::chrono::system_clock::now().time_since_epoch().count());
}

Move CheckMateCapturePlayer::get_move(const Board &board, const vector<Move> &moves) const {
    vector<Move> shuffled_moves = moves;
    shuffle(shuffled_moves.begin(), shuffled_moves.end(), random_number_generator);
    for (Move move : shuffled_moves) {
        if (board[move.from].is_opposite_team(board[move.to]) && (board[move.to] == WHITE_KING || board[move.to] == BLACK_KING)) {
            return move;
        }
    }
    for (Move move : shuffled_moves) {
        if (board[move.from].is_opposite_team(board[move.to])) {
            return move;
        }
    }
    return shuffled_moves[0];
}
