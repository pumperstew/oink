#include "BasicOperations.hpp"
#include "ChessConstants.hpp"

#ifdef OINK_MOVEGEN_DIAGNOSTICS
    #include "Display.hpp"
#endif

#include <cassert>

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

    Bitboard project_occupancy_from_file_to6bit(Bitboard b, RankFile file)
    {
        Bitboard this_file_occ = b & moves::sixbit_file_masks[file]; // mask off everything but this file
        return (this_file_occ * moves::FILE_ROTATORS[file]) >> 57;   // rotate onto horizontal (last rank), and then shift back down.
    }

    Bitboard project_occupancy_from_a1h8_to6bit(Bitboard b, Square square)
    {
        RankFile rank, file;
        square_to_rank_file(square, rank, file);
        RankFile diagonal_idx = 7 - (rank - file);

        Bitboard a1h8_diag_occ = b & moves::sixbit_diag_masks_a1h8[diagonal_idx];
        return (a1h8_diag_occ * moves::DIAG_A1H8_ROTATORS[diagonal_idx]) >> 57;
    }

    Bitboard project_occupancy_from_a8h1_to6bit(Bitboard b, Square square)
    {
        // diagonal indexing: a1, a2b1, a3b2c1, ..., a8h1, ..., g8h7, h8
        RankFile rank, file;
        square_to_rank_file(square, rank, file);
        RankFile diagonal_idx = rank + file;

        Bitboard a8h1_diag_occ = b & moves::sixbit_diag_masks_a8h1[diagonal_idx];
        return (a8h1_diag_occ * moves::DIAG_A8H1_ROTATORS[diagonal_idx]) >> 57;
    }
}
