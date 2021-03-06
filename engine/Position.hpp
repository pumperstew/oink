#ifndef POSITION_HPP
#define POSITION_HPP

#include "BasicTypes.hpp"
#include "BasicOperations.hpp"
#include "ChessConstants.hpp"
#include "Move.hpp"

#include <cassert>
#include <cstring>

namespace chess
{
    class Position
    {
		Bitboard generate_side(Side side) const;
        void move_common_first_stage(Piece moving_piece, Side side, Square source, Square dest, Bitboard source_and_dest_bitboard);
        void move_common_second_stage(Piece captured_piece, Side side_capturing, Bitboard dest_bitboard, Bitboard source_bitboard, Bitboard source_and_dest_bitboard);
	public:
        union
        {
            Bitboard piece_bbs[13];
            struct
            {
                // Order matches constants in pieces
                Bitboard whole_board;
                Bitboard pawns[2];
                Bitboard kings[2];
                Bitboard rooks[2];
                Bitboard knights[2];
                Bitboard bishops[2];
                Bitboard queens[2];
            };
        };
        Bitboard sides[2];
		Piece    squares[util::NUM_SQUARES];
        Square   ep_target_square;
        unsigned char fifty_move_count;
        unsigned char castling_rights; // bitmask
        PosEvaluation material;

        Position();

        void clear();
        void setup_starting_position();
        void update_sides();
        // Returns whether the move was successfully made.
		bool make_move(Move move);
        bool detect_check(Side king_side) const;
        bool square_attacked(Square square, Side side) const;

        OINK_INLINE Bitboard get_empty_squares() const
        {
            return ~whole_board;
        }

		//Checks that the sides contain no overlaps (two pieces on same square).
        OINK_INLINE void check_consistency() const 
        {
            assert(util::nil == (sides[sides::white] & sides[sides::black]));
        }

        bool operator==(const Position& other) const
        {
            return memcmp(piece_bbs, other.piece_bbs, sizeof(piece_bbs)) == 0 &&
                   memcmp(sides, other.sides, sizeof(sides)) == 0 &&
                   memcmp(squares, other.squares, sizeof(squares)) == 0 &&
                   ep_target_square == other.ep_target_square &&
                   fifty_move_count == other.fifty_move_count &&
                   castling_rights  == other.castling_rights &&
                   material         == other.material;
        }

        void manually_move_piece(Piece piece, Square from, Square to)
        {
            piece_bbs[piece] |= squarebits::indexed[to];
            piece_bbs[piece] &= ~squarebits::indexed[from];
            squares[from] = pieces::NONE;
            squares[to]   = piece;
        }

        void place_piece(Piece piece, Square where)
        {
            piece_bbs[piece] |= squarebits::indexed[where];
            squares[where]   = piece;
        }
    };
}

#endif