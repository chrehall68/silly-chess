#ifndef _CHESS_BOARD_H_
#define _CHESS_BOARD_H_

#include <iostream>
#include <map>
#include <vector>

#include "utf8_codepoint.h"

using std::istream;
using std::map;
using std::ostream;
using std::vector;

class ChessPiece;

enum Team {
    NONE,
    BLACK,
    WHITE
};

const char* team_name(Team team);

// A place on the board
struct Cell {
    int x;  // file -  1  (so we start at 0 instead of 1)
    int y;  // rank - 'a' (so we start at 0 instead of 'a')

    Cell() = default;
    Cell(int x, int y) : x(x), y(y) {}
    bool operator==(Cell other) const;
    bool operator!=(Cell other) const;
};

ostream& operator<<(ostream& os, const Cell& cell);
istream& operator>>(istream& is, Cell& cell);

struct Move {
    Cell from, to;

    Move() = default;
    Move(Cell from, Cell to) : from(from), to(to) {}
    bool operator==(Move other) const;
    bool operator!=(Move other) const;
};

ostream& operator<<(ostream& os, const Move& move);
istream& operator>>(istream& is, Move& move);

class Board {
    const ChessPiece* board[8][8];
    Team current_teams_turn;

   public:
    Board();
    const ChessPiece& operator[](Cell cell) const;
    // Reset all the pieces on the board (as if you're starting a new game).
    void reset_board();
    vector<Move> get_moves() const;
    // This function represents how most classical chess pieces would move.
    // This also allows us to add support for more complex "moves", like a pawn
    // getting to the end of the board and turning into a queen or some other type
    // of piece.
    // If we allow the chess piece that's moving to define the move, then we can
    // add really interesting custom pieces that are nothing like normal pieces!
    void make_classical_chess_move(Move move);
    // Makes a move on the board by calling make_move on the piece at move.from.
    void make_move(Move move);
    // Returns true if cell is on the board
    bool contains(Cell cell) const;
    // Returns the winner or NONE if there is no winner (yet).
    Team winner() const;

    friend ostream& operator<<(ostream& os, const Board& board);
};

#endif  // _CHESS_BOARD_H_
