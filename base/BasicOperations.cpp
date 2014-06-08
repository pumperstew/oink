#include "BasicOperations.hpp"
#include "ChessConstants.hpp"
#include "Display.hpp"

namespace chess 
{
    Square SinglePieceBitboardToIndex(bitboard singlePiece)
    {
        Square counter = 0;
        while (!(singlePiece & util::one)) 
		{
            singlePiece >>= 1;
            ++counter;
        }
        return counter;
    }

    /*get first set bit in b, return a bitboard with that bit set, with the index
    in sq. clear that bit from b*/
    bitboard GetAndClearFirstSetBitReturningIndex(bitboard &b, Square &square)
    {
        bitboard test;
        for (square = 0; square < util::NUM_SQUARES; ++square)
		{
            test = b & (util::one << square);
            if (test)
			{
                b ^= (util::one << square); //clear bit from b
                return test;
            }
        }
        return util::nil;
    }

	bitboard GetAndClearFirstSetBit(bitboard &b)
    {
        Square square;
        return GetAndClearFirstSetBitReturningIndex(b, square);
    }

	Square GetFirstIndexAndClear(bitboard &b)
	{
		Square square;
        GetAndClearFirstSetBitReturningIndex(b, square);
		return square;
	}

    Square GetFirstIndex(bitboard b)
	{
        Square square = squares::NO_SQUARE;
        for (square = 0; square < util::NUM_SQUARES; ++square)
		{
            if ((b & (util::one << square)) != util::nil)
			{
                break;
            }
        }
        return square;
	}

    bool IsSquareOccupied(bitboard b, Square square)
    {
        return (b & (util::one << square)) != util::nil;
    }

    //take position, get occupancy on 3rd/6th rank, and return position with the 4th/5th rank
    //also filled with the given rank's pieces (as well as its own). this can be used
    //to eliminate jumps over pieces by pawns going 2nd -> 4th rank.
    bitboard ExcludeFourthOrFifthRank(bitboard pos, Side side) 
    {
        if (side == sides::white)
            return pos | ((pos & moves::rank_masks[2]) << 8); //OINK_TODO: ugly if test
        else
            return pos | ((pos & moves::rank_masks[5]) >> 8); 
    }

    //Get occupancy of given rank [ranks::first, ranks::eighth].
	//Returns occupancy on [0,255] in lowest eight bits of return value.
    bitboard GetRankOccupancy(bitboard b, RankFile rank)
    {
        return (b >> (rank << 3)) & util::fullrank;
    }

	//Get occupancy of given file [0,7].
	//Returns occupancy on [0,255] in lowest eight bits of return value.
	//non-trivial, current impl. isn't great. OINK_TODO: replace with magic multiplier implementation, which will be much faster.
    bitboard GetFileOccupancy(bitboard b, RankFile file)
    {
        bitboard occ = 0;
        bitboard thisFileOccupancy = b & moves::file_masks[file]; //mask off everything but this file
        for (RankFile rank = 0; rank < util::BOARD_SIZE; ++rank)
		{
			occ |= ( ( (thisFileOccupancy >> RankFileToIndex(rank, file) ) & util::fullrank) //deal with one bit at a time
				   << rank); //shift up to appropriate bit on [0, 7]
        }
        return occ;
    }

	//OINK_TODO
    bitboard GetDiagonalOccupancy_a1h8(bitboard b, Square square)
    {
        bitboard occ = 0;
        bitboard a1h8_DiagonalOccupancy = b & moves::diagMasks_a1h8[square];

        for (RankFile rankOffset = 0; rankOffset < util::BOARD_SIZE; ++rankOffset)
		{
			occ |= ( 
				   ( (a1h8_DiagonalOccupancy >> RankFileToIndex(rankOffset, 0) ) //shift occupancy down by current rank offset.
				   & util::fullrank) //only first rank should contribute.
				   );
        }
        return occ;
    }

	//OINK_TODO
    bitboard GetDiagonalOccupancy_a8h1(bitboard b, Square square)
    {
        bitboard occ = 0;
        bitboard a8h1_DiagonalOccupancy = b & moves::diagMasks_a8h1[square];

        for (RankFile rankOffset = 0; rankOffset < util::BOARD_SIZE; ++rankOffset)
		{
			bitboard shiftedDown = ( (a8h1_DiagonalOccupancy >> RankFileToIndex(rankOffset, 0) ) );
			occ |= (shiftedDown & util::fullrank);
			//int index;
			//GetAndClearFirstSetBitReturningIndex(shiftedDown, index);
			//int fileOnFirstRank, unused;
			//IndexToRankAndFile(index, unused, fileOnFirstRank);
			//int shift = rankOffset - fileOnFirstRank;
			//occ |= ( 
			//	   ( (a8h1_DiagonalOccupancy >> RankFileToIndex(rankOffset, 0) ) //shift occupancy down by current rank offset.
			//	   & util::fullrank) //only first rank should contribute.
			//	   << shift);
        }
        return occ;
    }


	//bitboard GetDiagonalOccupancy_a8h1(bitboard b, Square square)
	//{
	//	bitboard occ = 0;
	//	bitboard a8h1_DiagonalOccupancy = b & moves::diagMasks_a8h1[square];

	//	int rank, file;
	//	IndexToRankAndFile(square, rank, file);

	//	PrintBitboard(a8h1_DiagonalOccupancy, "a8h1_DiagonalOccupancy", square);
 //       for (int rf = 0; rf < util::BOARD_SIZE; ++rf) //rank and file offset
	//	{
	//		int offset = RankFileToIndex(rf, 0);
	//		//square + offset = up + left
	//		//square - offset = down + right
	//		PrintBitboard(( (a8h1_DiagonalOccupancy >> (offset) ) & util::fullrank), "ARGH");
	//		PrintBitboard(( (a8h1_DiagonalOccupancy >> (offset) ) & util::fullrank) << rf, "ARGH2");
	//		//PrintBitboard(( (a8h1_DiagonalOccupancy >> (square - offset) ) & util::fullrank));

	//		

	//		bitboard shiftedToFirstRank = ( ( (a8h1_DiagonalOccupancy >> (offset) ) & util::fullrank) );
	//		int index;
	//		int a, frank0;
	//		
	//		
	//		GetAndClearFirstSetBitReturningIndex(shiftedToFirstRank, index);
	//		IndexToRankAndFile(index, a, frank0);
	//		int shift = rf - frank0;
	//		occ |= ( ( (a8h1_DiagonalOccupancy >> (offset) ) & util::fullrank)  //deal with one bit at a time
	//			   << shift); //shift up to appropriate bit on [0, 7]

	//		//occ |= ( ( (a8h1_DiagonalOccupancy >> (square - offset) ) & util::fullrank) //deal with one bit at a time
	//		//	   << rf); //shift up to appropriate bit on [0, 7]

	//		//PrintBitboard(occ, "occ", square);
 //       }
 //       return occ;
	//}
	
	void IndexToRankAndFile(Square index, RankFile &rank, RankFile &file)
	{
		rank = index / 8;
		file = index % 8;
	}

	RankFile IndexToRank(Square index)
	{
		return index / 8;
	}

	Square RankFileToIndex(RankFile rank, RankFile file)
	{
		return file + (rank << 3);
	}
}
