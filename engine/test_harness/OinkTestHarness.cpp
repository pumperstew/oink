#include "../Position.hpp"
#include "../Display.hpp"
#include "../MoveGenerator.hpp"

#include <thread>
#include <chrono>
#include <random>
#include <iostream>

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

uint64_t perft_fast(int depth, Position &pos, Side side)
{
    if (depth == 0)
        return 1;
 
    MoveVector moves = generate_all_moves(pos, side);
    size_t count = moves.size();
    uint64_t leaves = 0;
    for (int i = 0; i < count; ++i)
    {
        Position backup(pos);
        if (pos.make_move(moves[i]))
        {
            leaves += perft_fast(depth - 1, pos, swap_side(side));
            pos = backup; // undo
        }
    }
    return leaves;
}

uint64_t perft(int depth, Position &pos, Side side)
{
    uint64_t leaves = 0;
 
    if (depth == 0)
        return 1;
 
    MoveVector moves = generate_all_moves(pos, side);
    bool any = false;
    for (int i = 0; i < moves.size(); ++i)
    {
        Position backup(pos);
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

            leaves += perft(depth - 1, pos, swap_side(side));
            pos = backup; // undo
        }
    }

    // OINK_TODO: we detect this too late, so the results are for perft(n+1)
    if (depth == 1 && !any)
    {
        ++perft_mate_count;
    }

    return leaves;
}

void perft_driver(const int depth)
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
    uint64_t node_count = perft_fast(depth, pos, sides::white);

    int64_t elapsed = watch.elapsed();
    uint64_t nps = elapsed ? (uint64_t)(1000 * node_count / elapsed) : 0;

    auto check_print = [depth](uint64_t got, const uint64_t* expected_array) {
        return got == expected_array[depth] ? "        OK" : " ===============> FAIL";
    };

    cout.imbue(std::locale(""));
    cout << "\nperft(" << depth << ")"
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

//void collapsedFilesIndex2(Bitboard b) {
//   Bitboard r = b;
//   r |= r >> 32;
//   //r |= r >> 16;
//   //r |= r >>  8;
//   //Bitboard r = b & 0xFF;
//   print_bitboard(b, "unrotated");
//   print_bitboard(r, "rotated");
//}
//

static Bitboard rotate_occupancy_to_vertical(Bitboard occupancy /*in low eight bits*/, RankFile desired_file)
{
	Bitboard rotated_occ = util::nil;
    for (RankFile rank = 0; (rank < util::BOARD_SIZE) && occupancy; ++rank)
	{
        Bitboard thisbit = occupancy & util::one;
        occupancy >>= 1;

        rotated_occ |= thisbit << rank_file_to_square(rank, desired_file); // shift back up to correct point.
    }

	return rotated_occ;
}

void test_magic()
{
    Bitboard test = moves::file_masks[2] & ~4 & ~(1ULL<<58) & ~(1ULL<<10);

    print_bitboards({make_pair(test, "test"), make_pair(test * 0x2010080402010080ULL, "test*") });

    const Bitboard magic = 0x8040201008040201;
    const Bitboard magic2 = 0x8040201008040200;
    //print_bitboard(magic, "magic");

    Bitboard poo = 0x0100000000000001ULL;

    for (Bitboard b = 0; b <= 8; ++b)//util::fullrank; ++b)
    {
        Bitboard on_file = rotate_occupancy_to_vertical(b, 0);
        Bitboard only6 = on_file & (moves::file_masks[0] ^ poo);

        Bitboard multiplied = (only6 * magic);
        Bitboard multiplied2 = (only6 * magic2);
        Bitboard rotated_back = multiplied >> 57;
        Bitboard rotated_back2 = multiplied2 >> 57;

        assert(rotated_back == rotated_back2);
        //print_bitboard(poo, "poo");
        print_bitboards({
            make_pair(b, "b"),
            make_pair(magic, "magic"),
            make_pair(on_file, "on_file"),
            make_pair(only6, "only6"),
            make_pair(multiplied, "multiplied"),
            //make_pair(multiplied2, "multiplied2"),
            make_pair(rotated_back, "rotated_back")
        });

        assert((rotated_back & 0x3f) == rotated_back);
        //assert(rotated_back == (b & 0x7e));
        
    }
}


int main(int argc, char **argv)
{
    constants_initialize();

   // test_magic();

 //   return 0;
    

    for (int depth = 1; depth < 7; ++depth)
    {
        perft_driver(depth);
    }
    return 0;

    Position pos;
    pos.setup_starting_position();
    Side side = sides::white;
    
    std::mt19937 rand_engine(10);
    
    print_bitboard(pos.bishops[sides::white], "wb");

    int move_num = 1;

    while (pos.fifty_move_count < 100)
    {
        auto moves = generate_all_moves(pos, side);
        if (moves.empty())
        {
            break;
        }

        std::uniform_int_distribution<int> rand_dist(0, (int)moves.size() - 1);
        int move_index = rand_dist(rand_engine);

        print_move(moves[move_index], move_num);
        pos.make_move(moves[move_index]);
        print_position(pos);

        side = swap_side(side);

        if (side == sides::white)
        {
            move_num++;
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
}
