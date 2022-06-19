#ifndef _CHESS_PLAYER_H_
#define _CHESS_PLAYER_H_

#include <random>
#include <vector>

#include "chess_board.h"
#include "chess_pieces.h"

using std::vector;

class Player {
   public:
    const Team team;

    Player(Team team) : team(team) {}

    virtual Move get_move(const Board &board, const vector<Move> &moves) const = 0;
    virtual const char *name() const;
};

class RandomPlayer : public Player {
    mutable std::default_random_engine random_number_generator;

   public:
    RandomPlayer(Team team);

    Move get_move(const Board &board, const vector<Move> &moves) const override;
};

class HumanPlayer : public Player {
   public:
    HumanPlayer(Team team);
    Move get_move(const Board &board, const vector<Move> &moves) const override;
};

// CapturePlayer plays a random move that captures an opponents piece.
// If there is no such move, then it plays a random move.
class CapturePlayer : public Player {
    mutable std::default_random_engine random_number_generator;

   public:
    CapturePlayer(Team team);
    Move get_move(const Board &board, const vector<Move> &moves) const override;
};

class CheckMateCapturePlayer : public Player {
    mutable std::default_random_engine random_number_generator;

   public:
    CheckMateCapturePlayer(Team team);
    Move get_move(const Board &board, const vector<Move> &moves) const override;
};

class AIPlayer : public Player {
   public:
    AIPlayer(Team team);
    Move get_move(const Board &board, const vector<Move> &moves) const override;

   private:
    vector<int> minimax(const Board &board, const vector<Move> &moves, int depth, Team cur_team) const;
    const int king_weight = 100;
    const int custom_weight = 5;
    const map<const ChessPiece *, int> weights = {
        {&WHITE_PAWN, 1},
        {&BLACK_PAWN, 1},
        {&WHITE_KNIGHT, 3},
        {&BLACK_KNIGHT, 3},
        {&WHITE_BISHOP, 3},
        {&BLACK_BISHOP, 3},
        {&WHITE_ROOK, 5},
        {&BLACK_ROOK, 5},
        {&WHITE_QUEEN, 9},
        {&BLACK_QUEEN, 9},
        {&WHITE_KING, king_weight},
        {&BLACK_KING, king_weight}};
};

#endif  // _CHESS_PLAYER_H_
