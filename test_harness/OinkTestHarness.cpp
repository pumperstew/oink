#include "OinkTestHarness.hpp"
#include "Perft.hpp"

#include <engine/Position.hpp>
#include <engine/BasicOperations.hpp>
#include <engine/MoveGenerator.hpp>
#include <engine/Evaluator.hpp>
#include <engine/Search.hpp>
#include <fen_parser/FenParser.hpp>
#include <display/PgnWriter.hpp>
#include <display/ConsoleDisplay.hpp>

#include <thread>
#include <random>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <memory>
#include <atomic>
#include <queue>

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

static void perft_bench()
{
    auto perft_results = &kiwipete_perft_expectations;
    Side side_to_move;
    Position pos = fen::parse_fen(perft_results->fen, nullptr, &side_to_move);

    for (int i = 0; i < 4; ++i)
        perft_driver_nodesonly(pos, perft_results->depth_supported, perft_results->side_to_move, perft_results->nodes_expected[perft_results->depth_supported]);
}

static void perft_correctness()
{
    auto perft_results = &kiwipete_perft_expectations;

    Side side_to_move;
    Position pos = fen::parse_fen(perft_results->fen, nullptr, &side_to_move);
    assert(perft_results->side_to_move == side_to_move);
    print_position(pos);

    for (int depth = 0; depth <= perft_results->depth_supported; ++depth)
    {
        perft_driver_correctness(pos, depth, *perft_results);
    }
}

// OINK_TODO: find a home for this
std::pair<chess::Position, std::vector<uint64_t>> parse_epd_line(const std::string &line, int *fullmove_count, Side *side_to_move)
{
    //OINK_TODO: not fully robust
    auto start_of_node_info = line.rfind("d1");
    if (start_of_node_info == std::string::npos)
    {
        throw std::runtime_error("No node info in EPD line");
    }

    auto pos = fen::parse_fen(line.substr(0, start_of_node_info), fullmove_count, side_to_move);

    //OINK_TODO: obviously dreadful
    std::vector<uint64_t> node_info;
    for (auto i = start_of_node_info; i < line.size(); )
    {
        i += 2;
        auto next_space = line.find(' ', i + 1);
        if (next_space == std::string::npos)
            break;
        node_info.push_back(_atoi64(line.substr(i, next_space - i).c_str()));
        i = line.find("; ", i);
        if (i == std::string::npos)
            break;
        i += 2;
    }

    return std::make_pair(pos, node_info);
}

// OINK_TODO: find a home for this
static void marcel_6838_epd_test()
{
    ifstream epd_test_stream("../../../engine/tests/marcel_6838.epd");
    if (!epd_test_stream)
    {
        cout << "Failed to open test positions file 'marcel_6838.epd'" << endl;
        return;
    }

    string line;
    int line_num = 0;
    std::queue<std::unique_ptr<std::thread>> threads;
    std::atomic<int> fail_line = -1;

    while (getline(epd_test_stream, line))
    {
        ++line_num;

        Side side_to_move;
        auto results = parse_epd_line(line, nullptr, &side_to_move);
        
        threads.push(std::unique_ptr<std::thread>(new std::thread([=, &fail_line]
        {
            for (int depth = 1; depth <= results.second.size(); ++depth)
            {
                if (!perft_driver_nodesonly(results.first, depth, side_to_move, results.second[depth - 1], true))
                {
                    if (fail_line.load(std::memory_order_acquire) < 0)
                        fail_line.store(line_num, std::memory_order_release);
                    return;
                }
            }
        })));

        // TODO: we should really order the queue by the expected nodes at highest depth, and wait for the
        // thread with fewest nodes. This will keep the CPU busy better. Or, could just poll all threads here ("wait_any").
        if (threads.size() == std::thread::hardware_concurrency() - 1)
        {
            threads.front()->join();
            threads.pop();
        }

        if (fail_line.load(std::memory_order_acquire) >= 0)
        {
            break;
        }

        if (line_num % 50 == 0)
        {
            cout << "line = " << line_num << endl;
        }
    }

    while (!threads.empty())
    {
        threads.front()->join();
        threads.pop();
    }

    int fail = fail_line.load(std::memory_order_acquire);
    if (fail >= 0)
    {
        cout << "failed at line " << fail << endl;
    }
    else
    {
        cout << "ok: " << line_num << " lines passed" << endl;
    }
}

int main(int argc, char **argv)
{
    constants_initialize();

    //play_self("");//"rnbqkbnr/pp1ppppp/8/2p5/8/2N5/PPPPPPPP/R1BQKBNR/");
    //return 0;

    /*perft_bench();
    return 0;*/

    /*perft_correctness();
    return 0;*/

    marcel_6838_epd_test();
    return 0;
}
