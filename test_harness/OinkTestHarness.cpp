#include "OinkTestHarness.hpp"

#include <engine/Position.hpp>
#include <engine/BasicOperations.hpp>
#include <engine/MoveGenerator.hpp>
#include <engine/Evaluator.hpp>
#include <engine/Search.hpp>

#define OINK_MOVEGEN_DIAGNOSTICS

#ifdef OINK_MOVEGEN_DIAGNOSTICS
    #include <display/console/ConsoleDisplay.hpp>
#endif

#include <thread>
#include <chrono>
#include <random>
#include <iostream>
#include <fstream>

using namespace chess;
using namespace std;

int perft_capture_count;
int perft_castle_count;
int perft_prom_count;
int perft_ep_count;
int perft_check_count;
int perft_mate_count;

const uint64_t PERFT_NODES_EXPECTED[]    = { 1, 20, 400, 8902, 197281, 4865609, 119060324 };
const uint64_t PERFT_CAPTURES_EXPECTED[] = { 0, 0, 0, 34, 1576, 82719, 2812008 };
const uint64_t PERFT_EPS_EXPECTED[]      = { 0, 0, 0, 0, 0, 258, 5248 };
const uint64_t PERFT_CASTLES_EXPECTED[]  = { 0, 0, 0, 0, 0, 0, 0 };
const uint64_t PERFT_PROMS_EXPECTED[]    = { 0, 0, 0, 0, 0, 0, 0 };
const uint64_t PERFT_CHECKS_EXPECTED[]   = { 0, 0, 0, 12, 469, 27351, 809099 };

class StopWatch
{
    chrono::high_resolution_clock::time_point start;
public:
    StopWatch()
    {
        start = chrono::high_resolution_clock::now();
    }
    
    chrono::milliseconds::rep elapsed() const
    {
        auto elapsed = chrono::high_resolution_clock::now() - start;
        return chrono::duration_cast<chrono::milliseconds>(elapsed).count();
    }
};

static uint64_t perft_bench(int depth, Position &pos, Side side)
{
    if (depth == 0)
        return 1;

    MoveVector moves = generate_all_moves(pos, side);

    uint64_t leaves = 0;
    size_t num_moves = moves.size();
    Position backup(pos);

    for (size_t i = 0; i < num_moves; ++i)
    {
        if (pos.make_move(moves[i]))
        {
            leaves += perft_bench(depth - 1, pos, swap_side(side));
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
 
    MoveVector moves = generate_all_moves(pos, side);
    bool any = false;
    size_t num_moves = moves.size();
    Position backup(pos);

    for (size_t i = 0; i < num_moves; ++i)
    {
        if (pos.make_move(moves[i]))
        {
            any = true;

            if (depth == 1)
            {
                if (moves[i].get_captured_piece() != pieces::NONE)  ++perft_capture_count;
                if (moves[i].get_castling() != pieces::NONE)        ++perft_castle_count;
                if (moves[i].get_promotion_piece() != pieces::NONE) ++perft_prom_count;
                if (moves[i].get_en_passant() != pieces::NONE)      ++perft_ep_count;
                if (pos.detect_check(swap_side(side)))              ++perft_check_count;
            }

            leaves += perft_correctness(depth - 1, pos, swap_side(side));
        }
        pos = backup; // undo move
    }

    // OINK_TODO: we detect this too late, so the results are for perft(n+1)
    if (depth == 1 && !any)
    {
        ++perft_mate_count;
    }

    return leaves;
}

static void perft_driver_correctness(const int depth)
{
    Position pos;
    pos.setup_starting_position();
    
    perft_capture_count = 0;
    perft_castle_count  = 0;
    perft_prom_count    = 0;
    perft_ep_count      = 0;
    perft_check_count   = 0;
    perft_mate_count    = 0;

    StopWatch watch;
    uint64_t node_count = perft_correctness(depth, pos, sides::white);

    int64_t elapsed = watch.elapsed();
    uint64_t nps = elapsed ? (uint64_t)(1000 * node_count / elapsed) : 0;

    auto check_print = [depth](uint64_t got, const uint64_t* expected_array) {
        return got == expected_array[depth] ? "        OK" : " ===============> FAIL";
    };

    cout.imbue(std::locale(""));
    cout << "\nperft("     << depth << ")"
         << "\nNodes: "    << node_count            << check_print(node_count, PERFT_NODES_EXPECTED)
         << "\nCaptures: " << perft_capture_count   << check_print(perft_capture_count, PERFT_CAPTURES_EXPECTED)
         << "\nEPs: "      << perft_ep_count        << check_print(perft_ep_count, PERFT_EPS_EXPECTED)
         << "\nCastles: "  << perft_castle_count    << check_print(perft_castle_count, PERFT_CASTLES_EXPECTED)
         << "\nProms: "    << perft_prom_count      << check_print(perft_prom_count, PERFT_PROMS_EXPECTED)
         << "\nChecks: "   << perft_check_count     << check_print(perft_check_count, PERFT_CHECKS_EXPECTED)
         << "\nMates: "    << perft_mate_count
         << "\nNodes/second " << nps
         << "\n";
    cout.flush();

    /* printf("\npertf(%d):\nnodes: %I64u\ncaptures: %d\ncastles: %d\nproms: %d\neps: %d\nchecks: %d\nmates: %d\nelapsed: %f\nnps = %I64u\n", 
        depth, count, perft_capture_count, perft_castle_count, perft_prom_count, perft_ep_count, perft_check_count, perft_mate_count, 
        elapsed / 1000., );*/
}

static void perft_driver_bench(const int depth)
{
    Position pos;
    pos.setup_starting_position();

    StopWatch watch;
    uint64_t node_count = perft_bench(depth, pos, sides::white);

    int64_t elapsed = watch.elapsed();
    uint64_t nps = elapsed ? (uint64_t)(1000 * node_count / elapsed) : 0;

    auto check_print = [depth](uint64_t got, const uint64_t* expected_array) {
        return got == expected_array[depth] ? "        OK" : " ===============> FAIL";
    };

    cout.imbue(std::locale(""));
    cout << "\nperft(" << depth << ")"
         << "\nNodes: "       << node_count << check_print(node_count, PERFT_NODES_EXPECTED)
         << "\nNodes/second " << nps
         << "\n";
    cout.flush();
}

static void play_random()
{
    FILE *pgn_file = fopen("foo.pgn", "w");

    Position pos;
    pos.setup_starting_position();
    Side side = sides::white;
    
    std::random_device rand_dev;
    std::mt19937 rand_engine(10);//rand_dev());
    
    int move_num = 1;

    auto moves = generate_all_moves(pos, side);
    int opener = std::uniform_int_distribution<int>(0, (int)moves.size() - 1)(rand_engine);
    pos.make_move(moves[opener]);
    print_move(moves[opener], move_num, side, util::NORMAL, 0);
    pgn_out_move(pgn_file, moves[opener], move_num, side, util::NORMAL);
    print_position(pos);
    side = sides::black;

    while (1)
    {
        MoveAndEval result = alpha_beta(side, pos, 3, -2*evals::MATE_SCORE, 2*evals::MATE_SCORE);
        MoveAndEval result2 = minimax(side, pos, 3);

        if (result.best_eval      != result2.best_eval)
            printf("eval mismatch\n");
        if (result.best_move.data != result2.best_move.data)
            printf("move mismatch\n");

        if (!result.best_move.data)
        {
            printf("\n****** ERROR: NO LEGAL MOVES ****** : should have been mate or stalemate last move!\n");
            assert(false);
            break;
        }

        pos.make_move(result.best_move);

        auto pos_type = test_position_type(pos, swap_side(side));
        print_move(result.best_move, move_num, side, pos_type, (side == sides::black ? -1 : 1) * result.best_eval / 100.0);
        pgn_out_move(pgn_file, result.best_move, move_num, side, pos_type);
        print_position(pos);

        if (pos_type == util::MATE || pos_type == util::STALEMATE || pos_type == util::INSUFFICIENT_MATERIAL)
        {
            break;
        }
        else if (pos.fifty_move_count >= 100)
        {
            printf("\n1/2 - 1/2 (Fifty-move rule)\n");
            break;
        }
        else
        {
            printf("\nMaterial: %+d\n", pos.material / 100);
        }

        assert(pos.kings[0]);
        assert(pos.kings[1]);

        side = swap_side(side);

        if (side == sides::white)
        {
            move_num++;
        }

        fflush(pgn_file);
        //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
}

int main(int argc, char **argv)
{
    constants_initialize();

    play_random();
    return 0;

    perft_driver_bench(6);
    perft_driver_bench(6);
    return 0;

    for (int depth = 1; depth < 7; ++depth)
    {
        perft_driver_correctness(depth);
    }
    return 0;
}
