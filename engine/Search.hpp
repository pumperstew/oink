#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "BasicTypes.hpp"
#include "ChessConstants.hpp"
#include "Move.hpp"

namespace chess
{
    class Position;

    struct MoveAndEval
    {
        PosEvaluation best_eval;
        Move          best_move;
    };

    MoveAndEval minimax(Side side_moving, const Position &pos, int depth);
    MoveAndEval alpha_beta(Side side_moving, const Position &pos, int depth, int alpha, int beta);
}

#endif // SEARCH_HPP