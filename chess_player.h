#ifndef _CHESS_PLAYER_H_
#define _CHESS_PLAYER_H_

#include <random>
#include <vector>

#include "chess_board.h"

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

#endif  // _CHESS_PLAYER_H_
