#ifndef BASICOPERATIONS_HPP
#define BASICOPERATIONS_HPP

#include "BasicTypes.hpp"
#include "ChessConstants.hpp"

namespace chess 
{
    Square   get_first_occ_square(Bitboard b);
	Bitboard get_and_clear_first_occ_square(Bitboard b, Square *sq);
	
	Bitboard project_occupancy_from_file_to6bit(Bitboard b, RankFile rank);
	Bitboard project_occupancy_from_a1h8_to6bit(Bitboard b, Square square);
	Bitboard project_occupancy_from_a8h1_to6bit(Bitboard b, Square square);

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
	
	Bitboard exclude_fourth_or_fifth_rank_if_third_or_sixth_occupied(Bitboard pos, Side side);
}

#endif //BASICOPERATIONS_HPP

