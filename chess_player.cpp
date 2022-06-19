#include "chess_player.h"

#include <algorithm>
#include <chrono>
#include <climits>
#include <iostream>
#include <random>

#include "chess_board.h"
#include "chess_pieces.h"

using std::cin;
using std::cout;
using std::endl;
using std::numeric_limits;
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

AIPlayer::AIPlayer(Team team) : Player(team) {}
Move AIPlayer::get_move(const Board &board, const vector<Move> &moves) const {
    Board copy = Board(board);
    vector<int> vals = minimax(copy, moves, 3, team);
    return moves[vals[1]];
}

vector<int> AIPlayer::minimax(const Board &board, const vector<Move> &moves, int depth, Team cur_team) const {
    if (depth == 0 || board.winner() != NONE) {
        // count black, white pieces existing
        // note that the 8's should be replace with board.get_width() or board.get_height()
        // if the board supports that
        int black_count = 0, white_count = 0;
        for (size_t y = 0; y < 8; ++y) {
            for (size_t x = 0; x < 8; ++x) {
                const ChessPiece *cur_piece = &board[Cell(x, y)];
                Team cur_piece_team = cur_piece->team;
                if (cur_piece_team == BLACK) {
                    if (weights.find(cur_piece) != weights.end()) {
                        black_count += weights.at(cur_piece);
                    } else {
                        // assume all custom pieces are worth x amount
                        black_count += custom_weight;
                    }
                }
                if (cur_piece_team == WHITE) {
                    if (weights.find(cur_piece) != weights.end()) {
                        white_count += weights.at(cur_piece);
                    } else {
                        // assume all custom pieces are worth x amount
                        white_count += custom_weight;
                    }
                }
            }
        }

        return {(team == WHITE ? 1 : -1) * (white_count - black_count), 0};
    }

    if (cur_team == team) {
        int max_value = numeric_limits<int>::min();
        int best_idx = 0;
        for (size_t i = 0; i < moves.size(); ++i) {
            Board temp(board);
            temp.make_move(moves[i]);
            vector<int> vals = minimax(temp, temp.get_moves(), depth - 1, Team(static_cast<int>(!(team - 1)) + 1));
            if (vals[0] > max_value) {
                max_value = vals[0];
                best_idx = i;
            }
        }

        return {max_value, best_idx};
    } else {
        int min_value = numeric_limits<int>::max();
        int best_idx = 0;

        // first, find if there are any moves that result in capture
        vector<Move> choices;
        for (Move move : moves) {
            if (board[move.from].is_opposite_team(board[move.to])) {
                choices.push_back(move);
            }
        }

        // then iterate through all the valid moves (either all the moves if none result in capture
        // or just the ones that result in capture)
        const vector<Move> actual_choices = (choices.size() == 0 ? moves : choices);
        for (size_t i = 0; i < actual_choices.size(); ++i) {
            Board temp(board);
            temp.make_move(actual_choices[i]);
            vector<int> vals = minimax(temp, temp.get_moves(), depth - 1, Team(static_cast<int>(!(team - 1)) + 1));
            if (vals[0] < min_value) {
                min_value = vals[0];
                best_idx = i;
            }
        }
        return {min_value, best_idx};
    }
}