#include "BasicOperations.hpp"
#include "ChessConstants.hpp"

#ifdef OINK_MSVC_64
    #include <intrin.h>

    #pragma intrinsic(_BitScanForward64)
#endif

namespace chess 
{
    Square get_first_occ_square(Bitboard b)
	{
#ifdef OINK_MSVC_64
        // On release, this gets compiled down to a bsf followed by a cmov for the ternary.
        unsigned long square;
        unsigned char any = _BitScanForward64(&square, b);
        return any ? (Square)square : squares::NO_SQUARE;
#else
        Square square = squares::NO_SQUARE;
        for (square = 0; square < util::NUM_SQUARES; ++square)
		{
            if ((b & (util::one << square)) != util::nil)
			{
                break;
            }
        }
        return square;
#endif
	}

    Bitboard get_and_clear_first_occ_square(Bitboard b, Square *square)
    {
        *square = get_first_occ_square(b);
        b ^= (util::one << *square); // Clear this bit from b
        return b;
    }

    // Take position, get occupancy on 3rd/6th rank, and return position with the 4th/5th rank
    // also filled with the given rank's pieces (as well as its own). This can be used
    // to eliminate jumps over pieces by pawns going 2nd -> 4th rank.
    Bitboard exclude_fourth_or_fifth_rank_if_third_or_sixth_occupied(Bitboard pos, Side side) 
    {
        if (side == sides::white)
            return pos | ((pos & moves::rank_masks[ranks::third]) << 8); //OINK_TODO: ugly if test
        else
            return pos | ((pos & moves::rank_masks[ranks::sixth]) >> 8); 
    }

    Bitboard get_6bit_file_occupancy(Bitboard b, RankFile file)
    {
        Bitboard this_file_occ = b & moves::sixbit_file_masks[file]; // mask off everything but this file
        return (this_file_occ * moves::FILE_ROTATORS[file]) >> 57;   // rotate onto horizontal (last rank), and then shift back down.
    }

	// Get occupancy of given file on [files::a, files::h].
	// Returns the six-bit occupancy (by excluding redundant bottom and top bits) in lowest six bits of return value.
	// Replaced by magic-multiplier implementation above.
    /*
    Bitboard get_6bit_file_occupancy(Bitboard b, RankFile file)
    {
        Bitboard eightbit = 0;
        Bitboard this_file_occ = b & moves::file_masks[file]; //mask off everything but this file
        Square down_shift = file;
        for (RankFile rank = 0; rank < util::BOARD_SIZE; ++rank)
		{
			eightbit |= ( ( (this_file_occ >> down_shift) & util::fullrank) //deal with one bit at a time
				        << rank); //shift up to appropriate bit on [0, 7]

            down_shift += util::BOARD_SIZE;
        }
        return (eightbit & util::OCC_8_TO_6_MASK) >> 1;
    }
    */

    Bitboard project_occupancy_from_a1h8_to6bit(Bitboard b, Square square)
    {
        Bitboard occ = 0;
        Bitboard a1h8_diag_occ = b & moves::diag_masks_a1h8[square];

        // If we intercept the first rank, and do so not on the 'a' file, then an additional shift will be
        // required at the end, otherwise the low bits are wrong.
        RankFile rank, file;
        square_to_rank_file(square, rank, file);
        RankFile extraInterceptShift = rank >= file ? 0 : file - rank;

        for (RankFile rankOffset = 0; rankOffset < util::BOARD_SIZE; ++rankOffset)
		{
			occ |= (a1h8_diag_occ >> rank_file_to_square(rankOffset, 0)) // shift occupancy down to the bottom eight bits.
				   & util::fullrank;                                     // only first rank should contribute.
        }

        occ = occ >> extraInterceptShift;
        return (occ & util::OCC_8_TO_6_MASK) >> 1;
    }

    Bitboard project_occupancy_from_a8h1_to6bit(Bitboard b, Square square)
    {
        Bitboard occ = 0;
        Bitboard a8h1_diag_occ = b & moves::diag_masks_a8h1[square];

        RankFile rank, file;
        square_to_rank_file(square, rank, file);
        // If we intercept the eighth rank, and do so not on the 'a' file, then an additional shift will be
        // required at the end, otherwise the low bits are wrong.
        RankFile extraInterceptShift = rank + file >= util::BOARD_SIZE ? rank + file - (util::BOARD_SIZE - 1) : 0;

        for (RankFile rankOffset = 0; rankOffset < util::BOARD_SIZE; ++rankOffset)
		{
			occ |= (a8h1_diag_occ >> rank_file_to_square(rankOffset, 0))
                   & util::fullrank;
        }

        occ = occ >> extraInterceptShift;
        return (occ & util::OCC_8_TO_6_MASK) >> 1;
    }
}
