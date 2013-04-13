#include "BasicOperations.hpp"
#include "ChessConstants.hpp"

namespace chess 
{
    int SinglePieceBitboardToIndex(bitboard singlePiece)
    {
        int counter = 0;
        while (!(singlePiece & util::one)) 
		{
            singlePiece >>= 1;
            ++counter;
        }
        return counter;
    }

    /*bitboard get_first_set_bit(bitboard b)
    {
    int sq;
    return get_first_set_bit_with_index(b, sq);
    }*/

    /*bitboard get_first_set_bit_with_index(bitboard b, int &sq)
    {
    bitboard test;
    for (sq = 0; sq < util::NUM_SQUARES; ++sq) {
    test = b & (util::one << sq);
    if (test) return test;
    }
    return 0;
    }*/

    /*get first set bit in b, return a bitboard with that bit set, with the index
    in sq. clear that bit from b*/
    bitboard GetAndClearFirstSetBitReturningIndex(bitboard &b, int &sq)
    {
        bitboard test;
        for (sq = 0; sq < util::NUM_SQUARES; ++sq)
		{
            test = b & (util::one << sq);
            if (test)
			{
                b ^= (util::one << sq); //clear bit from b
                return test;
            }
        }
        return util::nil;
    }

	bitboard GetAndClearFirstSetBit(bitboard &b)
    {
        int sq;
        return GetAndClearFirstSetBitReturningIndex(b, sq);
    }

	int GetFirstIndexAndClear(bitboard &b)
	{
		int square;
        GetAndClearFirstSetBitReturningIndex(b, square);
		return square;
	}

    /*bitboard clear_first_set_bit(bitboard b)
    {
    bitboard test;
    for (int i = 0; i < util::NUM_SQUARES; ++i)
    {
    test = b & (util::one << i);
    if (test) return b & ~(util::one << i);
    }
    return 0;
    }*/

    //take position, get occupancy on given rank, and return position with the *next rank*
    //also filled with the given rank's pieces (as well as its own). this can be used
    //to eliminate jumps over pieces by pawns going 2nd -> 4th rank.
    bitboard ExcludeNextRank(bitboard pos, int rank, int side) 
    {
        if (side == sides::white)
            return pos | ((pos & moves::rank_masks[rank]) << 8); //OINK_TODO: ugly if test
        else
            return pos | ((pos & moves::rank_masks[rank]) >> 8); 
    }

    //Get occupancy of given rank [ranks::first, ranks::eighth].
	//Returns occupancy on [0,255] in lowest eight bits of return value.
    bitboard GetRankOccupancy(bitboard b, int rank)
    {
        return (b >> (rank << 3)) & util::fullrank;
    }

	//Get occupancy of given file [0,7].
	//Returns occupancy on [0,255] in lowest eight bits of return value.
	//non-trivial, current impl. isn't great. OINK_TODO: replace with magic multiplier implementation, which will be much faster.
    bitboard GetFileOccupancy(bitboard b, int file)
    {
        bitboard occ = 0;
		auto foo = moves::file_masks[file];
        bitboard bm = b & moves::file_masks[file]; //mask off everything but this file
        for (int i = 0; i < util::BOARD_SIZE; ++i) {
            occ |= (util::fullrank & ((bm >> (file + (i<<3))) << i)); //select bits up the file, shift them down to the low 8 bits
        }
        return occ;
    }

    bitboard get_diag_occupancy(bitboard b, int sq)
    {
        return 0;
    }
	
	void IndexToRankAndFile(int index, int &rank, int &file)
	{
		rank = index / 8;
		file = index % 8; //OINK_TODO: can hand optimize this i guess. compiler should do it anyway
	}

	int IndexToRank(int index)
	{
		return index / 8;
	}

	int RankFileToIndex(int rank, int file)
	{
		return file + (rank << 3);
	}
}
