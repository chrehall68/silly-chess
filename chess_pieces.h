#ifndef _CHESS_PIECES_H_
#define _CHESS_PIECES_H_

#include <iostream>
#include <map>
#include <vector>

#include "chess_board.h"
#include "utf8_codepoint.h"

using std::istream;
using std::map;
using std::ostream;
using std::vector;

class ChessPiece {
   public:
    const UTF8CodePoint utf8_codepoint;
    const Team team;

    ChessPiece(UTF8CodePoint cp, Team team) : utf8_codepoint(cp), team(team) {}

    virtual ~ChessPiece() {}

    virtual void get_moves(const Board &board, Cell from, vector<Move> &moves) const = 0;
    virtual void make_move(Board &board, Move move) const = 0;

    bool is_opposite_team(const ChessPiece &other) const;

    bool operator==(const ChessPiece &other) const;
    bool operator!=(const ChessPiece &other) const;

    friend ostream &operator<<(ostream &os, const ChessPiece &p);
};

ostream &operator<<(ostream &os, const ChessPiece &p);

class EmptySpace : public ChessPiece {
   public:
    EmptySpace() : ChessPiece('.', NONE) {}
    void get_moves(const Board &board, Cell from, vector<Move> &moves) const override {}
    void make_move(Board &board, Move move) const override {}
};

class SimpleChessPiece : public ChessPiece {
   public:
    SimpleChessPiece(UTF8CodePoint cp, Team team) : ChessPiece(cp, team) {}
    void make_move(Board &board, Move move) const;
};

class King : public SimpleChessPiece {
   public:
    King(UTF8CodePoint cp, Team team) : SimpleChessPiece(cp, team) {}
    void get_moves(const Board &board, Cell from, vector<Move> &moves) const override;
};

class Queen : public SimpleChessPiece {
   public:
    Queen(UTF8CodePoint cp, Team team) : SimpleChessPiece(cp, team) {}
    void get_moves(const Board &board, Cell from, vector<Move> &moves) const override;
};

class Bishop : public SimpleChessPiece {
   public:
    Bishop(UTF8CodePoint cp, Team team) : SimpleChessPiece(cp, team) {}
    void get_moves(const Board &board, Cell from, vector<Move> &moves) const override;
};

class Knight : public SimpleChessPiece {
   public:
    Knight(UTF8CodePoint cp, Team team) : SimpleChessPiece(cp, team) {}
    void get_moves(const Board &board, Cell from, vector<Move> &moves) const override;
};

class Rook : public SimpleChessPiece {
   public:
    Rook(UTF8CodePoint cp, Team team) : SimpleChessPiece(cp, team) {}
    void get_moves(const Board &board, Cell from, vector<Move> &moves) const override;
};

class Pawn : public SimpleChessPiece {
    int y_move_steps;

   public:
    Pawn(UTF8CodePoint cp, Team team, int y_move_steps)
        : SimpleChessPiece(cp, team), y_move_steps(y_move_steps) {}
    void get_moves(const Board &board, Cell from, vector<Move> &moves) const override;
};

// `extern` is used to declare the variables here, without defining them
// The actual variables/objects are defined in the corresponding .cpp file.
extern const EmptySpace EMPTY_SPACE;
extern const King WHITE_KING;
extern const King BLACK_KING;
extern const Queen WHITE_QUEEN;
extern const Queen BLACK_QUEEN;
extern const Bishop WHITE_BISHOP;
extern const Bishop BLACK_BISHOP;
extern const Knight WHITE_KNIGHT;
extern const Knight BLACK_KNIGHT;
extern const Rook WHITE_ROOK;
extern const Rook BLACK_ROOK;
extern const Pawn WHITE_PAWN;
extern const Pawn BLACK_PAWN;
extern const map<UTF8CodePoint, const ChessPiece *> ALL_CHESS_PIECES;

#endif  // _CHESS_PIECES_H_
