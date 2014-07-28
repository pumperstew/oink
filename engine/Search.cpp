#include "Search.hpp"
#include "Position.hpp"
#include "MoveGenerator.hpp"
#include "BasicOperations.hpp"
#include "Evaluator.hpp"

//#define OINK_SEARCH_DIAGNOSTICS

//#ifndef OINK_SEARCH_DIAGNOSTICS
//#include <display/ConsoleDisplay.hpp>
//#endif

#include <algorithm>

using namespace chess::util;

namespace chess
{
    MoveAndEval minimax(Side side_moving, const Position &pos, int depth)
    {
        MoveAndEval result;
        // If this doesn't get bettered, then we have no legal moves.
        result.best_eval = evals::INITIAL_SEARCH_VALUE;

        const MoveVector moves = generate_all_moves(pos, side_moving);
        //std::sort(moves.begin(), moves.end(), [](Move a, Move b) { return a.get_captured_piece() > b.get_captured_piece(); });
        for (size_t i = 0, num_moves = moves.size(); i < num_moves; ++i)
        {
            Position test = pos;

            if (test.make_move(moves[i]))
            {
                PosEvaluation leaf_eval;
                // Do the leaf eval here, rather than make the extra recursive call.
                if (depth == 1)
                    leaf_eval = -eval_position(swap_side(side_moving), test, depth);
                else
                    leaf_eval = -minimax(swap_side(side_moving), test, depth - 1).best_eval;

                if (leaf_eval > result.best_eval)
                {
                    result.best_eval = leaf_eval;
                    result.best_move = moves[i];
                }
            }  
        }

        // There were no legal moves. 
        // This means we're either in mate, or stalemate (but we're not at the desired search depth)
        // So we need to do static evaluation for this node.
        if (result.best_eval == evals::INITIAL_SEARCH_VALUE)
        {
            result.best_eval = eval_position(side_moving, pos, depth);
            if (result.best_eval != evals::DRAW_SCORE && result.best_eval > -evals::MATE_SCORE)
                 printf("\n****** ERROR: unexpected eval : %d\n", result.best_eval);
        }

        return result;
    }

    MoveAndEval alpha_beta(Side side_moving, const Position &pos, int depth, int alpha, int beta)
    {
        MoveAndEval result;
        // best_eval takes place of alpha. Since best_eval doesn't start at -infinity (cf. minimax),
        // use an auxiliary variable to keep track of whether we had any legal moves.
        result.best_eval = alpha;
        bool any_legal = false;

        const MoveVector moves = generate_all_moves(pos, side_moving);
        //std::sort(moves.begin(), moves.end(), [](Move a, Move b) { return a.get_captured_piece() > b.get_captured_piece(); });
        for (size_t i = 0, num_moves = moves.size(); i < num_moves; ++i)
        {
            Position test = pos;

            if (test.make_move(moves[i]))
            {
                PosEvaluation leaf_eval;
                if (depth == 1)
                {
                    leaf_eval = -eval_position(swap_side(side_moving), test, depth);
#ifdef OINK_SEARCH_DIAGNOSTICS
                    printf("LEAF:\n");
                    print_move(moves[i], -1, side_moving, util::NORMAL, leaf_eval);
                    print_position(test);
#endif
                }
                else
                {
#ifdef OINK_SEARCH_DIAGNOSTICS
                    printf("NON-LEAF:\n");
                    print_move(moves[i], -1, side_moving, util::NORMAL, 0);
                    print_position(test);
#endif
                    leaf_eval = -alpha_beta(swap_side(side_moving), test, depth - 1, -beta, -result.best_eval).best_eval;
                }

                if (leaf_eval >= beta)
                {
                    result.best_eval = beta;
                    result.best_move = moves[i];
                    return result;
                }

                if (leaf_eval > result.best_eval || !any_legal)
                {
                    result.best_eval = leaf_eval;
                    result.best_move = moves[i];
                }

                any_legal = true;
            }  
        }   

        // There were no legal moves. 
        // This means we're either in mate, or stalemate (but we're not at the desired search depth)
        // So we need to do static evaluation for this node.
        if (!any_legal)
        {
            result.best_eval = eval_position(side_moving, pos, depth);
            if (result.best_eval != evals::DRAW_SCORE && result.best_eval > -evals::MATE_SCORE)
                 printf("\n****** ERROR: unexpected eval : %d\n", result.best_eval);
        }

        return result;
    }
}