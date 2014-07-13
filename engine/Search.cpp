#include "Search.hpp"
#include "Position.hpp"
#include "MoveGenerator.hpp"
#include "BasicOperations.hpp"
#include "Evaluator.hpp"

//#define OINK_SEARCH_DIAGNOSTICS

#ifndef OINK_SEARCH_DIAGNOSTICS
#include <display/console/ConsoleDisplay.hpp>
#endif

using namespace chess::util;

namespace chess
{
    MoveAndEval minimax(Side side_moving, const Position &pos, int depth)
    {
        MoveAndEval result;
        // If this doesn't get bettered, then we have no legal moves -- the previous move up mated us.
        result.best_eval = -evals::MATE_SCORE;

        MoveVector moves = generate_all_moves(pos, side_moving);
        if (moves.empty())
        {
            printf("\n****** ERROR: MOVES EMPTY ****** : should have been mate or stalemate last move!\n");
            assert(false);
            exit(1);
        }

        bool any_legal = false;

        const int num_moves = (int)moves.size();
        for (int i = 0; i < num_moves; ++i)
        {
            Position test = pos;

            if (test.make_move(moves[i]))
            {
                PosEvaluation leaf_eval;
                // Do the leaf eval here, rather than make the extra recursive call.
                if (depth == 1)
                {
                    leaf_eval = eval_position(swap_side(side_moving), test, depth);
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
                    MoveAndEval rec_eval = minimax(swap_side(side_moving), test, depth - 1);
                    leaf_eval = rec_eval.best_eval;
                }

                leaf_eval = -leaf_eval;

                // If there are any legal moves, we must set result.
                if (leaf_eval > result.best_eval || !any_legal)
                {
                    result.best_eval = leaf_eval;
                    result.best_move = moves[i];
                }

                any_legal = true;
            }  
        }

        // None of the moves were legal. This means we're either in mate, or stalemate (but we're not at the leaf depth)
        // So we need to do static evaluation for this node.
        if (!any_legal)
        {
            result.best_eval = eval_position(side_moving, pos, depth);
            //print_position(pos);
            //printf("side: %d, best = %d depth = %d\n", side_moving, result.best_eval, depth);
        }

        return result;
    }

    MoveAndEval alpha_beta(Side side_moving, const Position &pos, int depth, int alpha, int beta)
    {
        MoveAndEval result;
        // If this doesn't get bettered, then we have no legal moves -- the previous move up mated us.
        result.best_eval = alpha;

        MoveVector moves = generate_all_moves(pos, side_moving);
        if (moves.empty())
        {
            printf("\n****** ERROR: MOVES EMPTY ****** : should have been mate or stalemate last move!\n");
            assert(false);
            exit(1);
        }

        bool any_legal = false;

        const int num_moves = (int)moves.size();
        for (int i = 0; i < num_moves; ++i)
        {
            Position test = pos;

            if (test.make_move(moves[i]))
            {
                PosEvaluation leaf_eval;
                if (depth == 1)
                {
                    leaf_eval = eval_position(swap_side(side_moving), test, depth);
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
                    MoveAndEval rec_eval = alpha_beta(swap_side(side_moving), test, depth - 1, -beta, -result.best_eval);
                    leaf_eval = rec_eval.best_eval;
                }

                leaf_eval = -leaf_eval;

                // Disable beta-cutoff for mate scores, so we find the shortest path.
                if (leaf_eval >= beta && leaf_eval < evals::MATE_SCORE)
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

        // None of the moves were legal. This means we're either in mate, or stalemate (but we're not at the leaf depth)
        // So we need to do static evaluation for this node.
        if (!any_legal)
        {
            result.best_eval = eval_position(side_moving, pos, depth);
            //print_position(pos);
            //printf("side: %d, best = %d depth = %d\n", side_moving, result.best_eval, depth);
        }

        return result;
    }
}