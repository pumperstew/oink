#include "OinkTestHarness.hpp"

#include <engine/Position.hpp>
#include <engine/BasicOperations.hpp>
#include <engine/MoveGenerator.hpp>
#include <engine/Evaluator.hpp>
#include <engine/Search.hpp>
#include <engine/Perft.hpp>
#include <fen_parser/FenParser.hpp>
#include <display/PgnWriter.hpp>
#include <display/ConsoleDisplay.hpp>

#include <random>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <chrono>

#define LOG_ERROR(message, ...) fprintf(stderr, "\n***ERROR*** " message "\n", ##__VA_ARGS__)

using namespace chess;
using namespace std;

static void play_self(const string &fen)
{
    std::ofstream pgn_file("test_game.pgn", std::ios::out);
    if (!pgn_file)
    {
        LOG_ERROR("Failed to open PGN");
        return;
    }

    Position pos;
    Side side = sides::white;
    int move_num = 1;

    if (fen.empty())
    {
        pos.setup_starting_position();
        // Opening move
        std::random_device rand_dev;
        std::mt19937 rand_engine(10); //rand_dev());
        MoveVector moves;
        generate_all_moves(moves, pos, side);
        int opener = std::uniform_int_distribution<int>(0, moves.size - 1)(rand_engine);
        pos.make_move(moves[opener]);
        print_move(moves[opener], move_num, side, util::NORMAL, 0);
        pgn_out_move(pgn_file, moves[opener], move_num, side, util::NORMAL);
        print_position(pos);
        side = sides::black;
    }
    else
    {
        /*fen::FenParseResult result = fen::parse_fen(fen, &pos, &move_num, &side);
        if (result.error != fen::FenParseOK)
        {
            LOG_ERROR("Failed to parse FEN: %s", result.error_string.c_str());
            return;
        }*/
    }

    const int DEPTH = 3;

    while (1)
    {
        MoveAndEval result        = alpha_beta(side, pos, DEPTH, -2*evals::MATE_SCORE, 2*evals::MATE_SCORE);
        MoveAndEval minimax_check = minimax(side, pos, DEPTH);

        if (result.best_eval != minimax_check.best_eval)
        {
            LOG_ERROR("eval mismatch");
            break;
        }
        if (result.best_move.data != minimax_check.best_move.data)
        {
            LOG_ERROR("move mismatch");
            break;
        }
        if (!result.best_move.data)
        {
            LOG_ERROR("NO LEGAL MOVES: should have been mate or stalemate last move!");
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
            printf("\nMaterial: %+d, Fifty-move counter: %d\n", pos.material / 100, pos.fifty_move_count);
        }

        assert(pos.kings[0]);
        assert(pos.kings[1]);

        side = swap_side(side);

        if (side == sides::white)
        {
            move_num++;
        }

        pgn_file.flush();
        //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
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

static bool perft_driver_nodesonly(Position pos, const int depth, Side side, uint64_t nodes_expected, bool quiet)
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

static void perft_bench()
{
    Side side_to_move;
    Position pos = fen::parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", nullptr, &side_to_move);

    for (int i = 0; i < 3; ++i)
        perft_driver_nodesonly(pos, 6, side_to_move, 119060324, false);
}

int main(int argc, char **argv)
{
    constants_initialize();

    //play_self("");//"rnbqkbnr/pp1ppppp/8/2p5/8/2N5/PPPPPPPP/R1BQKBNR/");
    //return 0;

    perft_bench();
    return 0;
}
