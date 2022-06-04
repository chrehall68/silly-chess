#include "chess_board.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "chess_pieces.h"
#include "utf8_codepoint.h"

using std::cerr;
using std::cout;
using std::endl;
using std::istream;
using std::map;
using std::ostream;
using std::out_of_range;
using std::setw;
using std::stringstream;
using std::vector;

// https://en.cppreference.com/w/cpp/error/assert
#define assertm(condition, message)                                 \
    {                                                               \
        if (!static_cast<bool>(condition)) cerr << message << endl; \
        assert(condition);                                          \
    }

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

Board::Board(size_t width, size_t height) : width(width), height(height) {
    assertm(width >= 2 && width <= 26, "width must be between 2 and 26 (inclusive)");
    assertm(height >= 2 && height <= 99, "height must be between 2 and 99 (inclusive)");
    reset_board();
}

void Board::resize_board() {
    board.clear();

    for (size_t y = 0; y < height; ++y) {
        board.push_back({});  // push an empty vector
        for (size_t x = 0; x < width; ++x) {
            board[y].push_back(&EMPTY_SPACE);
        }
    }
}

const ChessPiece &Board::operator[](Cell cell) const {
    return *board[cell.y][cell.x];
}

void Board::reset_board() {
    resize_board();

    if (board.size() >= 4) {
        for (size_t x = 0; x < width; ++x) {
            board[1][x] = &WHITE_PAWN;
            board[board.size() - 2][x] = &BLACK_PAWN;
        }
    }

    // important pieces first
    board[0][board[0].size() / 2] = &WHITE_KING;
    board[board.size() - 1][board[0].size() / 2] = &BLACK_KING;
    board[0][board[0].size() / 2 - 1] = &WHITE_QUEEN;
    board[board.size() - 1][board[0].size() / 2 - 1] = &BLACK_QUEEN;

    // other pieces, if have space
    vector<vector<const ChessPiece *>> importances = {{&WHITE_BISHOP, &BLACK_BISHOP}, {&WHITE_KNIGHT, &BLACK_KNIGHT}, {&WHITE_ROOK, &BLACK_ROOK}};
    for (size_t i = 0; i < importances.size(); ++i) {
        size_t xpos1 = board[0].size() / 2 + i + 1;
        if (xpos1 >= board[0].size()) {
            break;
        }
        board[0][xpos1] = importances[i][0];
        board[board.size() - 1][xpos1] = importances[i][1];

        int xpos2 = board[0].size() / 2 - 1 - i - 1;
        if (xpos2 < 0) {
            break;
        }
        board[0][xpos2] = importances[i][0];
        board[board.size() - 1][xpos2] = importances[i][1];
    }

    current_teams_turn = WHITE;
}

vector<Move> Board::get_moves() const {
    vector<Move> moves;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
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
    return cell.x >= 0 && cell.x < static_cast<int>(width) && cell.y >= 0 && cell.y < static_cast<int>(height);
}

Team Board::winner() const {
    bool found_white_king = false, found_black_king = false;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
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
    os << "   " << ((board.height + 1) / 10 > 0 ? " " : "");
    for (size_t i = 0; i < board.width; ++i) {
        os << static_cast<char>('a' + i);
    }
    os << "\n";

    for (int y = board.height - 1; y >= 0; --y) {
        os << ' ' << ((y + 1) / 10 > 0 ? "" : " ") << (y + 1) << ' ';
        for (int x = 0; x < static_cast<int>(board.width); ++x) {
            os << board[Cell(x, y)];
        }
        os << ' ' << (y + 1) << endl;
    }
    os << "   " << ((board.height + 1) / 10 > 0 ? " " : "");
    for (size_t i = 0; i < board.width; ++i) {
        os << static_cast<char>('a' + i);
    }
    os << "\n";
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
    is.get(c);

    // set the board's stuff
    board.width = num_cols;
    board.height = num_rows;
    board.resize_board();

    // read the pieces
    size_t row = 0;
    while (row < num_rows) {
        for (size_t col = 0; col < num_cols; ++col) {
            UTF8CodePoint temp;
            is >> temp;
            board.board[num_rows - row - 1][col] = ALL_CHESS_PIECES.at(temp);
        }
        // get the whitespace, row number, newline, whitespace, row number, whitespace
        for (int i = 0; i < 2; ++i) {
            if (!(row==num_rows-1 && i==1)) {
                int temp;
                is.get(c);
                is >> temp;
                is.get(c);
            }
        }

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

const size_t Board::get_height() const {
    return height;
}

const size_t Board::get_width() const {
    return width;
}