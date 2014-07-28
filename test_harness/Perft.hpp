#ifndef PERFT_HPP
#define PERFT_HPP

#include <engine/Position.hpp>

namespace chess
{
    const int MAX_PERFT_RESULTS_DEPTH = 6 + 1;

    struct DetailedPerftResults
    {
        const char* fen;
        const uint64_t PERFT_NODES_EXPECTED[MAX_PERFT_RESULTS_DEPTH];
        const uint64_t PERFT_CAPTURES_EXPECTED[MAX_PERFT_RESULTS_DEPTH];
        const uint64_t PERFT_EPS_EXPECTED[MAX_PERFT_RESULTS_DEPTH];
        const uint64_t PERFT_CASTLES_EXPECTED[MAX_PERFT_RESULTS_DEPTH];
        const uint64_t PERFT_PROMS_EXPECTED[MAX_PERFT_RESULTS_DEPTH];
        const uint64_t PERFT_CHECKS_EXPECTED[MAX_PERFT_RESULTS_DEPTH];
        const uint64_t PERFT_MATES_EXPECTED[MAX_PERFT_RESULTS_DEPTH];
    };

    extern const DetailedPerftResults starting_position_perft_expectations;
    extern const DetailedPerftResults kiwipete_perft_expectations;

    void perft_driver_correctness(Position pos, const int depth, const DetailedPerftResults &expected_results);

    void perft_driver_bench(Position pos, const int depth, const DetailedPerftResults &expected_results);
}

#endif // PERFT_HPP
