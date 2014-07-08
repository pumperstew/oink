#include "../Position.hpp"
#include "../BasicOperations.hpp"
#include "../ChessConstants.hpp"
#include "../Display.hpp"

#include <set>
#include <random>
#include <algorithm>

using namespace chess;
using namespace std;

Bitboard hash_fn(Bitboard in, Bitboard magic)
{
    Bitboard mult = (in * magic);
    Bitboard hash = (mult >> 57) & util::FULL_6BITOCC; 
    return hash;
}


void calc_files(int file)
{
    set<Bitboard> hashes;
    hashes.insert(util::nil);

    Square file_start = file;
    
    std::random_device rand_dev;
    std::mt19937 rand_engine(rand_dev());

    set<Bitboard> best_magics;
    Bitboard best_magic = util::full;
    Bitboard best_count = 64;
    int bests = 0;

    Bitboard magic = util::one; //0x8040201008040200;
    while (1)
    {
        Bitboard occ_count = 1;
        for (; occ_count < 64; ++occ_count)
        {
            Bitboard occ = util::nil;
            RankFile r = 1;
            for (int x = 0; x < 6; ++x)
            {
                int shift = rank_file_to_square(r, file);
                occ |= (occ_count & (util::one << x)) << (shift - x);
                ++r;
            }

            Bitboard hash = hash_fn(occ, magic);
            auto it = hashes.find(hash);
            if (it != hashes.end())
            {
                break;
            }
            else
            {
                /*print_bitboards({
                    make_pair(occ_count, "occ_count"),
                    make_pair(occ, "occ"),
                    make_pair(magic, "magic"),
                    make_pair(hash, "hash")
                });*/
                hashes.insert(hash);
            }
        }

        if (occ_count == 64)
        {
            if (_mm_popcnt_u64(magic) < best_count)
            {
                best_magics.insert(magic);
                best_magic = magic;
                best_count = _mm_popcnt_u64(best_magic);
                ++bests;
                printf("found a best with popcount %d\n", best_count);
            }

            if (bests >= 100 || best_count <= 6)
            {
                //for (auto h : hashes) print_bitboard(h, "h");
                printf("ok: bests = %d best_magic = %I64x", bests, best_magic);
                print_bitboards({
                    make_pair(best_magic, "magic"),
                });
                /*for (auto m : best_magics)
                {
                    if (_mm_popcnt_u64(m) == best_count)
                        print_bitboard(m, "best_magics");
                }*/
                break;
            }
        }

        //printf("RETRY\n");
        hashes.clear();
        hashes.insert(util::nil);

        magic = 0;
        set<int> chosen;
        for (int i = 0; i < std::min(12, (int)best_count) - 1; ++i)
        {
            int bit;
            do
            {
                std::uniform_int_distribution<Bitboard> rand_dist(0, 63);
                bit = rand_dist(rand_engine);
            }
            while (chosen.find(bit) != chosen.end());

            magic |= util::one << bit;
            chosen.insert(bit);
        }
        //std::uniform_int_distribution<Bitboard> rand_dist(0, util::full);
        //magic = rand_dist(rand_engine);
    }
}

void a8h1(int diag_significant_start, int diag_significant_length)
{
    set<Bitboard> hashes;
    hashes.insert(util::nil);

    RankFile r_start, f_start;
    square_to_rank_file(diag_significant_start, r_start, f_start);

    Bitboard magic = util::one;
    while (1)
    {
        Bitboard occ_count = 1;
        for (; occ_count < pow(2, diag_significant_length); ++occ_count)
        {
            Bitboard occ = util::nil;
            RankFile r = r_start, f = f_start;
            for (int x = 0; x < diag_significant_length; ++x)
            {
                int shift = rank_file_to_square(r, f);
                occ |= (occ_count & (util::one << x)) << (shift - x);
                --r;
                ++f;
            }

            Bitboard hash = hash_fn(occ, magic);
            auto it = hashes.find(hash);
            if (it != hashes.end())
            {
                break;
            }
            else
            {
                print_bitboards({
                    make_pair(occ_count, "occ_count"),
                    make_pair(occ, "occ"),
                    make_pair(magic, "magic"),
                    make_pair(hash, "hash")
                });
                hashes.insert(hash);
            }
        }

        if (occ_count == pow(2, diag_significant_length))
        {
            break;
        }

        printf("RETRY\n");
        hashes.clear();
        magic |= magic << 8;         
    }

    return;

    for (Bitboard occ_count = 1; occ_count < pow(2, diag_significant_length); ++occ_count)
    {
        Bitboard occ = util::nil;
        RankFile r = r_start, f = f_start;
        for (int x = 0; x < diag_significant_length; ++x)
        {
            int shift = rank_file_to_square(r, f);
            occ |= (occ_count & (util::one << x)) << (shift - x);
            --r;
            ++f;
        }

        bool ok = false;
        int count = 0;

        while (!ok)
        {
            Bitboard hash = hash_fn(occ, magic);

            auto it = hashes.find(hash);
            if (it != hashes.end())
            {
                Bitboard existing = *it;
                magic |= magic << 8;
                ++count;
            }
            else
            {
                ok = true;
                print_bitboards({
                    make_pair(occ_count, "occ_count"),
                    make_pair(occ, "occ"),
                    make_pair(magic, "magic"),
                    make_pair(hash, "hash")
                });
                printf("iterations: %d\n", count);

                hashes.insert(hash);
            }
        }
    }
}

int main()
{
    constants_initialize();

    calc_files(0);
    //a8h1(33, 4); //b5, c4, d3, e2
}
