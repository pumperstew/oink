#ifndef PERFT_HPP
#define PERFT_HPP

#include <engine/Position.hpp>

namespace chess
{
    const int MAX_PERFT_RESULTS_DEPTH = 6;
    const int MAX_PERFT_RESULTS_ARR_SIZE = MAX_PERFT_RESULTS_DEPTH + 1;

    struct DetailedPerftResults
    {
        const char* fen;
        int         depth_supported;
        Side        side_to_move;
        const uint64_t nodes_expected[MAX_PERFT_RESULTS_ARR_SIZE];
        const uint64_t captures_expected[MAX_PERFT_RESULTS_ARR_SIZE];
        const uint64_t eps_expected[MAX_PERFT_RESULTS_ARR_SIZE];
        const uint64_t castles_expected[MAX_PERFT_RESULTS_ARR_SIZE];
        const uint64_t proms_expected[MAX_PERFT_RESULTS_ARR_SIZE];
        const uint64_t checks_expected[MAX_PERFT_RESULTS_ARR_SIZE];
        const uint64_t mates_expected[MAX_PERFT_RESULTS_ARR_SIZE];
    };

    extern const DetailedPerftResults starting_position_perft_expectations;
    extern const DetailedPerftResults kiwipete_perft_expectations;

    void perft_driver_correctness(Position pos, const int depth, const DetailedPerftResults &expected_results);

    bool perft_driver_nodesonly(Position pos, const int depth, Side side, uint64_t nodes_expected, bool quiet = false);
}

#endif // PERFT_HPP
