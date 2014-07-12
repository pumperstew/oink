#ifndef BASICOPERATIONS_HPP
#define BASICOPERATIONS_HPP

#include "BasicTypes.hpp"
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
    OINK_INLINE Square get_first_occ_square(Bitboard b)
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

    OINK_INLINE Bitboard get_and_clear_first_occ_square(Bitboard b, Square *square)
    {
        assert(b);

        *square = get_first_occ_square(b);
        //b ^= (util::one << *square); // Clear this bit from b
        return b & (b - 1);
    }
	
    OINK_INLINE Side swap_side(Side side)
    {
        return side ^ 1;
    }

    // White pieces are always odd
    OINK_INLINE Side get_piece_side(Piece piece)
    {
        return util::one ^ (piece & util::one);
    }

    // Get occupancy of given rank on [ranks::first, ranks::eighth].
	// Returns the six-bit occupancy (by excluding redundant bottom and top bits) in lowest six bits of return value.
    OINK_INLINE Bitboard get_6bit_rank_occupancy(Bitboard b, RankFile rank)
    {        
        // These two produce the same asm when optimized (two shrs), despite
        // the last form's extra gumph -- the optimizer is able to reduce it down.
        // Note that the point about using 'int' to generate lea instructions
        // applies too.
        Bitboard eightbit = b >> (rank << 3);
        return (eightbit >> 1) & util::FULL_6BITOCC;
        //Bitboard eightbit = (b >> (rank << 3)) & util::fullrank;
        //return (eightbit & util::OCC_8_TO_6_MASK) >> 1;

        // This produces slightly different asm when optimised; it
        // does an inc, then a single shift, rather than two shrs.
        // BUT if we use an int for shift, it instead produces an lea, no inc, and a single shr.
        //RankFile shift = (rank << 3) + 1; // bottom bit is not wanted, so extra shift
        //return (b >> shift) & 0x3f;     // remove bits beyond first 6
    }

    OINK_INLINE bool is_square_occupied(Bitboard b, Square square)
    {
        return (b & (util::one << square)) != util::nil;
    }

    OINK_INLINE void square_to_rank_file(Square square, RankFile &rank, RankFile &file)
	{
		rank = square >> 3;
		file = square % 8;
	}

	OINK_INLINE RankFile square_to_rank(Square square)
	{
		return square >> 3;
	}

	OINK_INLINE Square rank_file_to_square(RankFile rank, RankFile file)
	{
		return file + (rank << 3);
	}

    OINK_INLINE Square calc_rank_shift(RankFile rank)
    {
        return rank << 3;
    }
	
	// Take position, get occupancy on 3rd/6th rank, and return position with the 4th/5th rank
    // also filled with the given rank's pieces (as well as its own). This can be used
    // to eliminate jumps over pieces by pawns going 2nd -> 4th rank.
    OINK_INLINE Bitboard exclude_fourth_or_fifth_rank_if_third_or_sixth_occupied(Bitboard pos, Side side) 
    {
        if (side == sides::white)
            return pos | ((pos & moves::eightbit_rank_masks[ranks::third]) << 8); //OINK_TODO: ugly if test
        else
            return pos | ((pos & moves::eightbit_rank_masks[ranks::sixth]) >> 8); 
    }

    OINK_INLINE Bitboard project_occupancy_from_file_to6bit(Bitboard b, RankFile file)
    {
        Bitboard this_file_occ = b & moves::sixbit_file_masks[file]; // mask off everything but this file
        return (this_file_occ * moves::FILE_ROTATORS[file]) >> 57;   // rotate onto horizontal (last rank), and then shift back down.
    }

	OINK_INLINE Bitboard project_occupancy_from_a8h1_to6bit(Bitboard b, RankFile rank, RankFile file)
    {
        // diagonal indexing: a1, a2b1, a3b2c1, ..., a8h1, ..., g8h7, h8
        RankFile diagonal_idx = rank + file;
        Bitboard a8h1_diag_occ = b & moves::sixbit_diag_masks_a8h1[diagonal_idx];
        return (a8h1_diag_occ * moves::DIAG_A8H1_ROTATORS[diagonal_idx]) >> 57;
    }

    OINK_INLINE Bitboard project_occupancy_from_a1h8_to6bit(Bitboard b, RankFile rank, RankFile file)
    {
        RankFile diagonal_idx = 7 - (rank - file);
        Bitboard a1h8_diag_occ = b & moves::sixbit_diag_masks_a1h8[diagonal_idx];
        return (a1h8_diag_occ * moves::DIAG_A1H8_ROTATORS[diagonal_idx]) >> 57;
    }
}

#endif // BASICOPERATIONS_HPP

