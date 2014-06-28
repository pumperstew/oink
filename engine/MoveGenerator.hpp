#ifndef MOVEGENERATOR_HPP
#define MOVEGENERATOR_HPP

#include "Move.hpp"

namespace chess
{
    const size_t OINK_MOVE_VECTOR_INITIAL_SIZE = 30;

    class Position;

	void generate_pawn_moves(MoveVector &moves, const Position &position,   Side side);
	void generate_king_moves(MoveVector &moves, const Position &position,   Side side);
	void generate_rook_moves(MoveVector &moves, const Position &position,   Side side);
    void generate_knight_moves(MoveVector &moves, const Position &position, Side side);
    void generate_bishop_moves(MoveVector &moves, const Position &position, Side side);
	void generate_queen_moves(MoveVector &moves, const Position &position,  Side side);
	MoveVector generate_all_moves(const Position &position,	Side side);
}

#endif
