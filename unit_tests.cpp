#include <algorithm>
#include <cassert>
#include <sstream>
#include <vector>

#include "chess_board.h"
#include "chess_pieces.h"
#include "chess_player.h"
#include "utf8_codepoint.h"
using namespace std;

// https://en.cppreference.com/w/cpp/error/assert
#define assertm(condition, message)                                 \
    {                                                               \
        if (!static_cast<bool>(condition)) cerr << message << endl; \
        assert(condition);                                          \
    }

// make sure that board.contains really works since we will be using it a lot
void test_contains(const Board& board) {
    for (int y = -10; y < 10; ++y) {
        for (int x = -10; x < 10; ++x) {
            bool expected = x >= 0 && x < 8 && y >= 0 && y < 8;
            try {
                ostringstream temp;
                temp << "expected the board to contain " << Cell(x, y) << " but it didn't contain it";
                assertm(expected == board.contains(Cell(x, y)), temp.str());
            } catch (out_of_range e) {
                ostringstream temp;
                temp << "expected the board to not contain cell (" << x << ", " << y << ") but it did contain it";
                assertm(!expected, temp.str());
            }
        }
    }
}

// makes sure that regular moving (just swapping) works
// tests occasions that will never happen as well
void test_make_classical_chess_move(Board& board) {
    board.reset_board();
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const ChessPiece* there = &board[Cell(x, y)];
            board.make_classical_chess_move(Move(Cell(x, y), Cell(4, 4)));  // (4, 4) is an arbitrary number
            assert(board[Cell(x, y)] == EMPTY_SPACE);
            assert(board[Cell(4, 4)] == *there);
            board.reset_board();
        }
    }
}

void test_make_moves(Board& board) {
    // test when it is too small
    for (int i = 1; i < 10; ++i) {
        // to tests
        bool threw_to_error = false;
        int to_x = -(rand() % i + 1);
        int to_y = -(rand() % i + 1);
        try {
            board.make_move(Move(Cell(0, 0), Cell(to_x, to_y)));  // this should throw an out of range error
        } catch (exception e) {
            threw_to_error = true;
        }

        ostringstream temp;
        temp << "expected board.make_move to throw an error when trying to move to (" << to_x << ", " << to_y << "), but board.make_move didn't";
        assertm(threw_to_error, temp.str());

        // from tests
        bool threw_from_error = false;
        int from_x = -(rand() % i + 1);
        int from_y = -(rand() % i + 1);

        try {
            board.make_move(Move(Cell(from_x, from_y), Cell(4, 4)));  // this should throw an out of range error
        } catch (exception e) {
            threw_from_error = true;
        }

        temp.str("");
        temp.clear();
        temp << "expected board.make_move to throw an error when trying to move from (" << from_x << ", " << from_y << "), but board.make_move didn't";
        assertm(threw_from_error, temp.str());
    }

    // test when it is too large
    for (int i = 1; i < 10; ++i) {
        // to tests
        bool threw_to_error = false;
        int to_x = 9 + rand() % i;
        int to_y = 9 + rand() % i;
        try {
            board.make_move(Move(Cell(0, 0), Cell(to_x, to_y)));  // this should throw an out of range error
        } catch (out_of_range e) {
            threw_to_error = true;
        }

        ostringstream temp;
        temp << "expected board.make_move to throw an error when trying to move to (" << to_x << ", " << to_y << "), but board.make_move didn't";
        assertm(threw_to_error, temp.str());

        // from tests
        bool threw_from_error = false;
        int from_x = 9 + rand() % i;
        int from_y = 9 + rand() % i;

        try {
            board.make_move(Move(Cell(from_x, from_y), Cell(4, 4)));  // this should throw an out of range error
        } catch (out_of_range e) {
            threw_from_error = true;
        }

        temp.str("");
        temp.clear();
        temp << "expected board.make_move to throw an error when trying to move from (" << from_x << ", " << from_y << "), but board.make_move didn't";
        assertm(threw_from_error, temp.str());
    }
}

Cell operator+(const Cell& c1, const Cell& c2) {
    return Cell(c1.x + c2.x, c1.y + c2.y);
}
template <typename T>
ostream& operator<<(ostream& os, const vector<T> v) {
    os << "{";
    for (size_t i = 0; i < v.size(); ++i) {
        os << v[i];
        if (i != v.size() - 1) {
            os << ", ";
        }
    }
    return os << "}";
}
// make sure that all the moves returned follow the displacements set by 'directions'
void test_get_moves(const ChessPiece& piece, const Board& board, const Cell& cur_cell, const vector<Cell>& directions) {
    vector<Move> moves;
    piece.get_moves(board, cur_cell, moves);
    vector<Cell> destinations;  // faster search times

    // fill destinations
    for (const Move& m : moves) {
        destinations.push_back(m.to);
        assert(board[m.to].team != piece.team);
        assert(board.contains(m.to));
    }

    // make sure that the destinations are correct moves
    for (const Cell& c : directions) {
        Cell temp(c + cur_cell);
        if (board.contains(temp) && board[temp].team != piece.team) {
            ostringstream err_msg;
            err_msg << "tried to move " << c.x << " to the right and " << c.y << " up from " << cur_cell << " to end up at " << temp << " but this wasn't found in the outputted destinations."
                    << "Outputted destinations (by piece.get_moves) are: " << destinations;
            assertm(std::find(destinations.begin(), destinations.end(), temp) != destinations.end(), err_msg.str());
        }
    }
}

void test_has_team_name(const ChessPiece& piece) {
    string temp = string(team_name(piece.team));
    assert(temp != "UNKNOWN" && (piece == EMPTY_SPACE || temp != "None"));  // if it is unnamed (unless it's empty space), something is wrong
}

void test_winner(Board& board) {
    for (int i = 0; i < 3; ++i) {
        board.reset_board();

        Team actual_winner = NONE;
        bool found_white_king = false, found_black_king = false;
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                if (&board[Cell(x, y)] == &WHITE_KING) {
                    if (i != 1) {
                        found_white_king = true;
                    } else {
                        board.make_classical_chess_move(Move(Cell(4, 4), Cell(x, y)));  // kill the white king with an empty space
                    }
                } else if (&board[Cell(x, y)] == &BLACK_KING) {
                    if (i != 2) {
                        found_black_king = true;
                    } else {
                        board.make_classical_chess_move(Move(Cell(4, 4), Cell(x, y)));  // kill the black king with an empty space
                    }
                }
            }
        }
        if (!found_white_king) {
            actual_winner = BLACK;
        }
        if (!found_black_king) {
            actual_winner = WHITE;
        }

        Team winner = board.winner();

        ostringstream temp;
        temp << "Expected the winner to be " << actual_winner << " but got " << winner;
        assertm(actual_winner == winner, temp.str());
    }
}

int main() {
    Board m;
    vector<Cell> pawn_moves = {Cell(0, 1)};
    test_get_moves(m[Cell(0, 1)], m, Cell(0, 1), pawn_moves);

    test_contains(m);
    test_make_classical_chess_move(m);
    test_make_moves(m);

    test_winner(m);

    cout << "all tests passed" << endl;
}