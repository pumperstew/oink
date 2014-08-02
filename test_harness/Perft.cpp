#include "Perft.hpp"

#include <engine/MoveGenerator.hpp>
#include <engine/Evaluator.hpp>

#include <chrono>
#include <iostream>

using namespace std;
using namespace chess;

static int perft_capture_count;
static int perft_castle_count;
static int perft_prom_count;
static int perft_ep_count;
static int perft_check_count;
static int perft_mate_count;

namespace chess
{
    const DetailedPerftResults starting_position_perft_expectations = 
    {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        6,
        sides::white,
        { 1, 20, 400, 8902, 197281, 4865609, 119060324 },
        { 0, 0, 0, 34, 1576, 82719, 2812008 },
        { 0, 0, 0, 0, 0, 258, 5248 },
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 12, 469, 27351, 809099 },
        { 0, 0, 0, 0,  8,   347,   10828 },
    };

    const DetailedPerftResults kiwipete_perft_expectations = 
    {
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq",
        5,
        sides::white,
        { 1, 48, 2039, 97862, 4085603, 193690690, 0 },
        { 0, 8,  351,  17102, 757163,  35043416,  0 },
        { 0, 0,  1,    45,    1929,    73365,     0 },
        { 0, 2,  91,   3162,  128013,  4993637,   0 },
        { 0, 0,  0,    0,     15172,   8392,      0 },
        { 0, 0,  3,    993,   25523,   3309887,   0 },
        { 0, 0,  0,    1,     43,      30171,     0 },
    };
}

static const char* perft_checker(int depth, uint64_t got, const uint64_t expected_array[7])
{
    return got == expected_array[depth] ? "        OK" : " ===============> FAIL";
}

class StopWatch
{
    chrono::high_resolution_clock::time_point start;
public:
    StopWatch()
    {
        start = chrono::high_resolution_clock::now();
    }
    
    chrono::milliseconds::rep elapsed_ms() const
    {
        auto elapsed = chrono::high_resolution_clock::now() - start;
        return chrono::duration_cast<chrono::milliseconds>(elapsed).count();
    }
};

static uint64_t perft_nodesonly(int depth, Position &pos, Side side)
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

static uint64_t perft_correctness(int depth, Position &pos, Side side)
{
    uint64_t leaves = 0;
 
    if (depth == 0)
        return 1;
 
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
                if (moves[i].get_captured_piece() != pieces::NONE)  ++perft_capture_count;
                if (moves[i].get_castling() != pieces::NONE)        ++perft_castle_count;
                if (moves[i].get_promotion_piece() != pieces::NONE) ++perft_prom_count;
                if (moves[i].get_en_passant() != pieces::NONE)      ++perft_ep_count;
                if (pos_type == util::CHECK)                        ++perft_check_count;
                if (pos_type == util::MATE)
                {
                    ++perft_mate_count;
                    ++perft_check_count;
                }
            }

            leaves += perft_correctness(depth - 1, pos, swap_side(side));
        }
        pos = backup; // undo move
    }

    return leaves;
}

namespace chess
{
    void perft_driver_correctness(Position pos, const int depth, const DetailedPerftResults &expected_results)
    {
        perft_capture_count = 0;
        perft_castle_count  = 0;
        perft_prom_count    = 0;
        perft_ep_count      = 0;
        perft_check_count   = 0;
        perft_mate_count    = 0;

        uint64_t node_count = perft_correctness(depth, pos, expected_results.side_to_move);

        cout.imbue(std::locale(""));
        cout << "\nperft("     << depth << ")"
             << "\nNodes: "    << node_count            << perft_checker(depth, node_count, expected_results.nodes_expected)
             << "\nCaptures: " << perft_capture_count   << perft_checker(depth, perft_capture_count, expected_results.captures_expected)
             << "\nEPs: "      << perft_ep_count        << perft_checker(depth, perft_ep_count, expected_results.eps_expected)
             << "\nCastles: "  << perft_castle_count    << perft_checker(depth, perft_castle_count, expected_results.castles_expected)
             << "\nProms: "    << perft_prom_count      << perft_checker(depth, perft_prom_count, expected_results.proms_expected)
             << "\nChecks: "   << perft_check_count     << perft_checker(depth, perft_check_count, expected_results.checks_expected)
             << "\nMates: "    << perft_mate_count      << perft_checker(depth, perft_mate_count, expected_results.mates_expected)
             << endl;
    }

    bool perft_driver_nodesonly(Position pos, const int depth, Side side, uint64_t nodes_expected, bool quiet)
    {
        StopWatch watch;
        uint64_t node_count = perft_nodesonly(depth, pos, side);

        int64_t elapsed_ms = watch.elapsed_ms();
        uint64_t nps = elapsed_ms ? (uint64_t)(1000 * node_count / elapsed_ms) : 0;

        if (!quiet)
        {
            cout.imbue(std::locale(""));
            cout << "\nperft("        << depth << ")"
                 << "\nNodes: "       << node_count << (node_count == nodes_expected ? "        OK" : " ===============> FAIL")
                 << "\nElapsed: "     << elapsed_ms/1000. << "s"
                 << "\nNodes/second " << nps
                 << endl;
        }
        return node_count == nodes_expected;
    }
}
