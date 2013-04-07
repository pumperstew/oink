#ifndef MOVEGENERATOR_HPP
#define MOVEGENERATOR_HPP

#include "Position.hpp"
#include "Move.hpp"

namespace chess
{
	class MoveGenerator
	{
	public:
		MoveVector GeneratePawnMoves(const Position &position,   int side);
		MoveVector GenerateKingMoves(const Position &position,   int side);
		MoveVector GenerateRookMoves(const Position &position,   int side);
        MoveVector GenerateKnightMoves(const Position &position, int side);
        MoveVector GenerateBishopMoves(const Position &position, int side);
		MoveVector GenerateAllMoves(const Position &position,	 int side);
	};
}

#endif
