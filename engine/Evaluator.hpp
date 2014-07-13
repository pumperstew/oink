#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "BasicTypes.hpp"
#include "ChessConstants.hpp"

namespace chess
{
    class Position;

    util::PositionType test_position_type(const Position &pos, Side king_side);

    PosEvaluation eval_position(Side side_to_move, const Position &pos, int depth);
}

#endif // EVALUATOR_HPP