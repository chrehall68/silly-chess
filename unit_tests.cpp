#include <cassert>
#include <set>
#include <vector>

#include "chess_board.h"
#include "chess_pieces.h"
#include "chess_player.h"
#include "utf8_codepoint.h"
using namespace std;

// make sure that board.contains really works since we will be using it a lot
void test_contains(const Board& board) {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            bool expected = x >= 0 && x < 8 && y >= 0 && y < 8;
            assert(expected == board.contains(Cell(x, y)));
        }
    }
}

// makes sure that regular moving (just swapping) works
// tests occasions that will never happen as well
void test_make_classical_move(Board& board) {
    board.reset_board();
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            auto there = board[Cell(x, y)];
            board.make_classical_chess_move(Move(Cell(x, y), Cell(4, 4)));  // (4, 4) is an arbitrary number
            assert(board[Cell(x, y)] == EMPTY_SPACE);
            assert(board[Cell(4, 4)] == there);
            board.reset_board();
        }
    }
}

// makes sure that all the moves returned are on the board
void test_valid_get_moves(const ChessPiece& piece, const Board& board, const Cell& cur_cell) {
    vector<Move> moves;
    piece.get_moves(board, cur_cell, moves);
    for (Move& m : moves) {
        assert(board.contains(m.to));
    }
}

Cell operator+(const Cell& c1, const Cell& c2) {
    return Cell(c1.x + c2.x, c1.y + c2.y);
}
// make sure that all the moves returned follow the displacements set by 'directions'
void test_correct_get_moves(const ChessPiece& piece, const Board& board, const Cell& cur_cell, const vector<Cell>& directions) {
    vector<Move> moves;
    piece.get_moves(board, cur_cell, moves);
    set<Cell> destinations;  // faster search times

    // fill destinations
    for (const Move& m : moves) {
        destinations.insert(m.to);
    }

    // make sure that the destinations are correct moves
    for (const Cell& c : directions) {
        Cell temp(c + cur_cell);
        if (board.contains(temp)) {
            assert(destinations.find(temp) != destinations.end());
        }
    }
}

void test_has_team_name(const ChessPiece& piece) {
    string temp = string(team_name(piece.team));
    assert(temp != "UNKNOWN" && (piece == EMPTY_SPACE || temp != "None"));  // if it is unnamed (unless it's empty space), something is wrong
}