#include "Evaluator.hpp"
#include "Position.hpp"
#include "MoveGenerator.hpp"
#include "BasicOperations.hpp"

#include <display/console/ConsoleDisplay.hpp>

using namespace chess::util;

namespace chess
{
    PositionType test_position_type(const Position &pos, Side king_side)
    {
        if (!(pos.whole_board & ~pos.kings[sides::white] & ~pos.kings[sides::black]))
            return INSUFFICIENT_MATERIAL;

        bool in_check = pos.detect_check(king_side);

        MoveVector moves = generate_all_moves(pos, king_side);
        size_t count = moves.size();

        Position test(pos);
        bool any_legal = false;

        for (size_t i = 0; i < count; ++i)
        {
            any_legal = test.make_move(moves[i]);
            if (any_legal)
                break;
            test = pos;
        }

        if (in_check && !any_legal)
            return MATE;
        else if (!in_check && !any_legal)
            return STALEMATE;
        else if (in_check)
            return CHECK;
        else
            return NORMAL;
    }

    // From POV of side to move.
    PosEvaluation eval_position(Side side_to_move, const Position &pos, int depth)
    {
        PositionType pos_type = test_position_type(pos, side_to_move);
        if (pos_type == MATE)
        {
            return -(evals::MATE_SCORE + depth);
        }
        else if (pos_type == STALEMATE || pos_type == INSUFFICIENT_MATERIAL)
        {
            return evals::DRAW_SCORE;
        }

        PosEvaluation eval = 0;

        // Negative material is good for black
        int material_sign = side_to_move == sides::black ? -1 : +1;
        int material_eval = material_sign * pos.material;

        eval += material_eval;

        // Generally worse to be in check
        //if (pos_type == CHECK)
        //    eval -= evals::CHECK_BIAS;        
        return eval;
    }
}