#include "Perft.hpp"
#include "Position.hpp"
#include "MoveGenerator.hpp"
#include "Evaluator.hpp"

namespace chess
{
    uint64_t perft_nodesonly(int depth, Position &pos, Side side)
    {
        if (depth == 0)
            return 1;

        MoveVector moves;
        generate_all_moves(moves, pos, side);

        uint64_t leaves = 0;
        Position backup(pos);

        for (uint32_t i = 0; i < moves.size; ++i)
        {
            if (pos.make_move(moves[i]))
            {
                leaves += perft_nodesonly(depth - 1, pos, swap_side(side));
            }
            pos = backup; // undo move
        }
        return leaves;
    }

    static void perft_correctness_inner(int depth, Position &pos, Side side, DetailedPerftResults &results)
    {
        uint64_t leaves = 0;
 
        if (depth == 0)
        {
            results.total_leaves += 1;
            return;
        }

        MoveVector moves;
        generate_all_moves(moves, pos, side);
        bool any = false;
        Position backup(pos);

        for (uint32_t i = 0; i < moves.size; ++i)
        {
            if (pos.make_move(moves[i]))
            {
                any = true;

                if (depth == 1)
                {
                    auto pos_type = test_position_type(pos, swap_side(side));
                    if (moves[i].get_captured_piece() != pieces::NONE)  ++results.capture_count;
                    if (moves[i].get_castling() != pieces::NONE)        ++results.castle_count;
                    if (moves[i].get_promotion_piece() != pieces::NONE) ++results.prom_count;
                    if (moves[i].get_en_passant() != pieces::NONE)      ++results.ep_count;
                    if (pos_type == util::CHECK)                        ++results.check_count;
                    if (pos_type == util::MATE)
                    {
                        ++results.mate_count;
                        ++results.check_count;
                    }
                }

                perft_correctness_inner(depth - 1, pos, swap_side(side), results);
            }
            pos = backup; // undo move
        }
    }

    DetailedPerftResults perft_correctness(int depth, Position &pos, Side side)
    {
        DetailedPerftResults results;
        memset(&results, 0, sizeof(results));
        perft_correctness_inner(depth, pos, side, results);
        return results;
    }
}
