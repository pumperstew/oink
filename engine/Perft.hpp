#ifndef PERFT_HPP
#define PERFT_HPP

#include "BasicTypes.hpp"

namespace chess
{
    class Position;

    struct DetailedPerftResults
    {
        uint64_t total_leaves;
        uint64_t capture_count;
        uint64_t castle_count;
        uint64_t prom_count;
        uint64_t ep_count;
        uint64_t check_count;
        uint64_t mate_count;
    };

    DetailedPerftResults perft_correctness(int depth, Position &pos, Side side);
    uint64_t perft_nodesonly(int depth, Position &pos, Side side);
}

#endif // PERFT_HPP
