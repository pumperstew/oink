#ifndef MOVEGENERATOR_HPP
#define MOVEGENERATOR_HPP

#include "Position.hpp"
#include "Move.hpp"

namespace chess
{
	class MoveGenerator
	{
	public:
		MoveVector GeneratePawnMoves(const Position &position,   Side side);
		MoveVector GenerateKingMoves(const Position &position,   Side side);
		MoveVector GenerateRookMoves(const Position &position,   Side side);
        MoveVector GenerateKnightMoves(const Position &position, Side side);
        MoveVector GenerateBishopMoves(const Position &position, Side side);
		MoveVector GenerateQueenMoves(const Position &position,  Side side);
		MoveVector GenerateAllMoves(const Position &position,	 Side side);
	};
}

#endif
