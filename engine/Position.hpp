#ifndef POSITION_HPP
#define POSITION_HPP

#include "BasicTypes.hpp"
#include "BasicOperations.hpp"
#include "ChessConstants.hpp"
#include "Move.hpp"

#include <cassert>

namespace chess
{
    class Position
    {
		Bitboard generate_side(Side side) const;
	public:
        //15 Bitboards (120) plus 64 pieces (256) = 376 bytes. Ouch.
		Bitboard kings[2];
        Bitboard rooks[2];
        Bitboard knights[2];
        Bitboard bishops[2];
        Bitboard queens[2];
        Bitboard pawns[2];
        Bitboard sides[2];
		Bitboard whole_board;
		Piece    squares[util::NUM_SQUARES];
        Square   ep_target_square;
        //Bitboard all_rot45_a1h8, all_rot45_a8h1;

        Position();

        void clear();
        void setup_starting_position();
        void update_sides();
		
		void make_move(Move move)
		{
			//? how do we know what pieces to update, or do we have to regenerate all pieces from full position??
		}

        OINK_INLINE Bitboard get_other_side_position(Side side) const
        {
            return sides[side ^ 1]; //1 ^ 1 = 0; 0 ^ 1 = 1
        }

        OINK_INLINE Bitboard get_empty_squares() const
        {
            return ~whole_board;
        }

		//void test_rot45();

        // Remove any pieces captured due to an update. 
        // side_capturing:  the side which did the capture.
		// diff: "diff" bitboard (bits only set where there's a change).
        void remove_captured(Side side_capturing, Bitboard diff);

		//Checks that the sides contain no overlaps (two pieces on same square).
        void check_consistency() const 
        {
            assert(util::nil == (sides[sides::white] & sides[sides::black]));
        }
    };
}

#endif