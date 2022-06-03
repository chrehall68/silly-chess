#include "chess_board.h"

#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "chess_pieces.h"
#include "utf8_codepoint.h"

using std::cout;
using std::endl;
using std::istream;
using std::map;
using std::ostream;
using std::out_of_range;
using std::stringstream;
using std::vector;

const char *team_name(Team team) {
    switch (team) {
        case WHITE:
            return "White";
        case BLACK:
            return "Black";
        case NONE:
            return "None";
    }
    return "UNKNOWN";
}

bool Cell::operator==(Cell other) const {
    return x == other.x && y == other.y;
}

bool Cell::operator!=(Cell other) const {
    return !(*this == other);
}

bool Move::operator==(Move other) const {
    return to == other.to && from == other.from;
}

bool Move::operator!=(Move other) const {
    return !(*this == other);
}

ostream &operator<<(ostream &os, const Cell &cell) {
    return os << static_cast<char>(cell.x + 'a') << cell.y + 1;
}
istream &operator>>(istream &is, Cell &cell) {
    char x_plus_a;
    int y_plus_1;
    is >> x_plus_a >> y_plus_1;
    cell.x = x_plus_a - 'a';
    cell.y = y_plus_1 - 1;
    return is;
}

ostream &operator<<(ostream &os, const Move &move) {
    return os << move.from << move.to;
}
istream &operator>>(istream &is, Move &move) {
    return is >> move.from >> move.to;
}

Board::Board() {
    reset_board();
}

const ChessPiece &Board::operator[](Cell cell) const {
    return *board[cell.y][cell.x];
}

void Board::reset_board() {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            board[y][x] = &EMPTY_SPACE;
        }
    }

    for (int x = 0; x < 8; ++x) {
        board[1][x] = &WHITE_PAWN;
        board[6][x] = &BLACK_PAWN;
    }

    board[0][0] = &WHITE_BOMBTOWER;
    board[0][1] = &WHITE_KNIGHT;
    board[0][2] = &WHITE_BISHOP;
    board[0][3] = &WHITE_QUEEN;
    board[0][4] = &WHITE_KING;
    board[0][5] = &WHITE_BISHOP;
    board[0][6] = &WHITE_KNIGHT;
    board[0][7] = &WHITE_BOMBTOWER;

    board[7][0] = &BLACK_ROOK;
    board[7][1] = &BLACK_KNIGHT;
    board[7][2] = &BLACK_BISHOP;
    board[7][3] = &BLACK_QUEEN;
    board[7][4] = &BLACK_KING;
    board[7][5] = &BLACK_BISHOP;
    board[7][6] = &BLACK_KNIGHT;
    board[7][7] = &BLACK_ROOK;

    current_teams_turn = WHITE;
}

vector<Move> Board::get_moves() const {
    vector<Move> moves;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (board[y][x]->team == current_teams_turn) {
                board[y][x]->get_moves(*this, Cell(x, y), moves);
            }
        }
    }
    for (Move move : moves) {
        if (!contains(move.to) || !contains(move.from)) {
            stringstream err_msg;
            err_msg << "Board::get_moves got a move that moves to or from a cell that is not on the board: " << move;
            throw out_of_range(err_msg.str());
        }
    }
    return moves;
}

// This function represents how most classical chess ALL_CHESS_PIECES would move.
// This also allows us to add support for more complex "moves", like a pawn
// getting to the end of the board and turning into a queen or some other type
// of piece.
// If we allow the chess piece that's moving to define the move, then we can
// add really interesting custom ALL_CHESS_PIECES that are nothing like normal ALL_CHESS_PIECES!
void Board::make_classical_chess_move(Move move) {
    board[move.to.y][move.to.x] = board[move.from.y][move.from.x];
    board[move.from.y][move.from.x] = &EMPTY_SPACE;
    current_teams_turn = current_teams_turn == WHITE ? BLACK : WHITE;
}

void Board::make_move(Move move) {
    if (!contains(move.to) || !contains(move.from)) {
        stringstream err_msg;
        err_msg << "Board::make_move called with a move that moves to or from a cell that is not on the board: " << move;
        throw out_of_range(err_msg.str());
    }
    board[move.from.y][move.from.x]->make_move(*this, move);
}

bool Board::contains(Cell cell) const {
    return cell.x >= 0 && cell.x < 8 && cell.y >= 0 && cell.y < 8;
}

Team Board::winner() const {
    bool found_white_king = false, found_black_king = false;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (board[y][x] == &WHITE_KING) {
                found_white_king = true;
            } else if (board[y][x] == &BLACK_KING) {
                found_black_king = true;
            }
        }
    }
    if (!found_white_king) {
        return BLACK;
    }
    if (!found_black_king) {
        return WHITE;
    }
    return NONE;
}

ostream &operator<<(ostream &os, const Board &board) {
    os << "   abcdefgh\n";
    for (int y = 7; y >= 0; --y) {
        os << ' ' << (y + 1) << ' ';
        for (int x = 0; x < 8; ++x) {
            os << board[Cell(x, y)];
        }
        os << ' ' << (y + 1) << endl;
    }
    os << "   abcdefgh\n";
    return os;
}

istream &operator>>(istream &is, Board &board) {
    size_t num_rows = 0;
    size_t num_cols = 0;

    // get num cols
    char c;
    while (is.get(c)) {
        if (c == '\n') {
            break;
        }
        if (c != ' ') {
            ++num_cols;
        }
    }

    // get num cols
    is.get(c);  // get the whitespace
    is >> num_rows;
    is.get(c);  // get the wrapping whitespace

    // read the pieces
    size_t row = 0;
    while (row < num_rows) {
        for (size_t col = 0; col < num_cols; ++col) {
            UTF8CodePoint temp;
            is >> temp;
            board.board[num_rows - row - 1][col] = ALL_CHESS_PIECES.at(temp);
        }
        // get the whitespace, row number, newline, whitespace, row number, whitespace
        for (int i = 0; i < 6; ++i) is.get(c);
        ++row;
    }

    // read the rest of the board (the bottom stuff)
    while (is.get(c)) {
        if (c == '\n') {
            break;
        }
    }

    return is;
}