#include <engine/Perft.hpp>
#include <engine/Position.hpp>
#include <fen_parser/FenParser.hpp>
#include <display/ConsoleDisplay.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <memory>
#include <atomic>
#include <queue>
#include <thread>

using namespace chess;
using namespace std;

namespace
{

static const int MAX_PERFT_RESULTS_DEPTH = 6;
static const int MAX_PERFT_RESULTS_ARR_SIZE = MAX_PERFT_RESULTS_DEPTH + 1;

struct DetailedPerftExpectations
{
    const char* fen;
    int         depth_supported;
    Side        side_to_move;
    const uint64_t leaves_expected[MAX_PERFT_RESULTS_ARR_SIZE];
    const uint64_t captures_expected[MAX_PERFT_RESULTS_ARR_SIZE];
    const uint64_t eps_expected[MAX_PERFT_RESULTS_ARR_SIZE];
    const uint64_t castles_expected[MAX_PERFT_RESULTS_ARR_SIZE];
    const uint64_t proms_expected[MAX_PERFT_RESULTS_ARR_SIZE];
    const uint64_t checks_expected[MAX_PERFT_RESULTS_ARR_SIZE];
    const uint64_t mates_expected[MAX_PERFT_RESULTS_ARR_SIZE];
};

static const DetailedPerftExpectations starting_position_perft_expectations = 
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

static const DetailedPerftExpectations kiwipete_perft_expectations = 
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

class PerftBasedTests : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		constants_initialize();
	}

    const char* perft_checker(int depth, uint64_t got, const uint64_t expected_array[7])
    {
        return got == expected_array[depth] ? "        OK" : " ===============> FAIL";
    }

    void perft_driver_correctness(Position pos, const int depth, const DetailedPerftExpectations &expected_results)
    {
        auto results = perft_correctness(depth, pos, expected_results.side_to_move);

        cout.imbue(std::locale(""));
        cout << "\nperft("     << depth << ")"
                << "\nNodes: "    << results.total_leaves    << perft_checker(depth, results.total_leaves, expected_results.leaves_expected)
                << "\nCaptures: " << results.capture_count   << perft_checker(depth, results.capture_count, expected_results.captures_expected)
                << "\nEPs: "      << results.ep_count        << perft_checker(depth, results.ep_count, expected_results.eps_expected)
                << "\nCastles: "  << results.castle_count    << perft_checker(depth, results.castle_count, expected_results.castles_expected)
                << "\nProms: "    << results.prom_count      << perft_checker(depth, results.prom_count, expected_results.proms_expected)
                << "\nChecks: "   << results.check_count     << perft_checker(depth, results.check_count, expected_results.checks_expected)
                << "\nMates: "    << results.mate_count      << perft_checker(depth, results.mate_count, expected_results.mates_expected)
                << endl;

        ASSERT_EQ(results.total_leaves, expected_results.leaves_expected[depth]);
        ASSERT_EQ(results.capture_count, expected_results.captures_expected[depth]);
        ASSERT_EQ(results.ep_count, expected_results.eps_expected[depth]);
        ASSERT_EQ(results.castle_count, expected_results.castles_expected[depth]);
        ASSERT_EQ(results.prom_count, expected_results.proms_expected[depth]);
        ASSERT_EQ(results.mate_count, expected_results.mates_expected[depth]);
    }
};

TEST_F(PerftBasedTests, TestPerftStartingPosition)
{
    Side side_to_move;
    Position pos = fen::parse_fen(starting_position_perft_expectations.fen, nullptr, &side_to_move);
    ASSERT_EQ(starting_position_perft_expectations.side_to_move, side_to_move);
    print_position(pos);

    for (int depth = 0; depth <= starting_position_perft_expectations.depth_supported; ++depth)
        perft_driver_correctness(pos, depth, starting_position_perft_expectations);
}

TEST_F(PerftBasedTests, TestPerftKiwiPete)
{
    Side side_to_move;
    Position pos = fen::parse_fen(kiwipete_perft_expectations.fen);
    ASSERT_EQ(kiwipete_perft_expectations.side_to_move, side_to_move);
    print_position(pos);

    for (int depth = 0; depth <= kiwipete_perft_expectations.depth_supported; ++depth)
        perft_driver_correctness(pos, depth, kiwipete_perft_expectations);
}

static std::pair<chess::Position, std::vector<uint64_t>> parse_epd_line(const std::string &line, int *fullmove_count, Side *side_to_move)
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

TEST_F(PerftBasedTests, Marcel6838_EPD_Test)
{
    ifstream epd_test_stream("../../../../engine/tests/marcel_6838.epd");
    ASSERT_TRUE(epd_test_stream.is_open());

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
                Position pos_copy = results.first;
                if (perft_nodesonly(depth, pos_copy, side_to_move) != results.second[depth - 1])
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

    ASSERT_EQ(fail, 0);
}

}
