#ifndef BASICOPERATIONS_HPP
#define BASICOPERATIONS_HPP

#include "BasicTypes.hpp"
#include "ChessConstants.hpp"

namespace chess 
{
    Square   get_first_occ_square(Bitboard b);
	Bitboard get_and_clear_first_occ_square(Bitboard b, Square *sq);
	
	Bitboard get_file_occupancy(Bitboard b, RankFile rank);
	Bitboard project_occupancy_from_a1h8(Bitboard b, Square square);
	Bitboard project_occupancy_from_a8h1(Bitboard b, Square square);

    OINK_INLINE Side swap_side(Side side)
    {
        return side ^ 1;
    }

    // White pieces are always odd
    OINK_INLINE Side get_piece_side(Piece piece)
    {
        return util::one ^ (piece & util::one);
    }

    // Get occupancy of given rank [ranks::first, ranks::eighth].
	// Returns occupancy on [0,255] in lowest eight bits of return value.
    OINK_INLINE Bitboard get_rank_occupancy(Bitboard b, RankFile rank)
    {
        return (b >> (rank << 3)) & util::fullrank;
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

