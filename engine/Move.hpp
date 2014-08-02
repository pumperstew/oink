#ifndef MOVE_HPP
#define MOVE_HPP

#include "ChessConstants.hpp"

#include <cassert>

namespace chess
{
    /*************

    Storage scheme:

	* data has 32 bits
	* source and destination square are on the interval [0, 63] or [0, 0b111111] or [0, 0x3f] => 6 bits each.
	* the pieces fit into four bits.
	* if we promote, castle, or do an en-passant capture with this move, we store the piece promoting to, or the relevant king, or the relevant pawn -- again 4 bits.
	layout (LSB on left):

	[source:6][destination:6][piece:4][captured:4][promotion:4][castling:4][ep:4]

	*************/

	class Move
	{
    public:
		typedef uint32_t MoveData;

    private:
	    static const int DESTINATION_OFFSET = 6;
	    static const int PIECE_OFFSET		= 12;
	    static const int CAPTURED_OFFSET    = 16;
	    static const int PROMOTION_OFFSET   = 20;
        static const int CASTLING_OFFSET    = 24;
        static const int EP_OFFSET          = 28;

	    static const MoveData SOURCE_MASK      = 0x3f;
	    static const MoveData DESTINATION_MASK = 0x3f << DESTINATION_OFFSET;
	    static const MoveData PIECE_MASK       = 0xf  << PIECE_OFFSET;
	    static const MoveData CAPTURED_MASK    = 0xf  << CAPTURED_OFFSET;
	    static const MoveData PROMOTION_MASK   = 0xf  << PROMOTION_OFFSET;
        static const MoveData CASTLING_MASK    = 0xf  << CASTLING_OFFSET;
        static const MoveData EP_MASK          = 0xf  << EP_OFFSET;

    public:
		MoveData data;

		OINK_INLINE Move() { data = 0; }

        OINK_INLINE Square get_source() const
	    {
		    return data & SOURCE_MASK;
	    }

        OINK_INLINE Square get_destination() const
	    {
		    return (data & DESTINATION_MASK) >> DESTINATION_OFFSET;
	    }

        OINK_INLINE Piece get_piece() const
        {
            return (data & PIECE_MASK) >> PIECE_OFFSET;
        }

	    OINK_INLINE Piece get_captured_piece() const
	    {
		    return (data & CAPTURED_MASK) >> CAPTURED_OFFSET;
	    }

	    OINK_INLINE Piece get_promotion_piece() const
	    {
            return (data & PROMOTION_MASK) >> PROMOTION_OFFSET;
	    }

        OINK_INLINE Piece get_castling() const
	    {
		    return (data & CASTLING_MASK) >> CASTLING_OFFSET;
	    }

        OINK_INLINE Piece get_en_passant() const
	    {
		    return (data & EP_MASK) >> EP_OFFSET;
	    }

	    OINK_INLINE void set_source(Square square)
	    {
		    data = (data & ~SOURCE_MASK) | (MoveData)square;
	    }

	    OINK_INLINE void set_destination(Square square)
	    {
		    data = (data & ~DESTINATION_MASK) | ((MoveData)square << DESTINATION_OFFSET);
	    }

	    OINK_INLINE void set_piece(Piece piece)
	    {
		    data = (data & ~PIECE_MASK) | (piece << PIECE_OFFSET);
	    }

	    OINK_INLINE void set_captured_piece(Piece piece)
	    {
		    data = (data & ~CAPTURED_MASK) | (piece << CAPTURED_OFFSET);
	    }

	    OINK_INLINE void set_promotion_piece(Piece piece)
	    {
		    data = (data & ~PROMOTION_MASK) | (piece << PROMOTION_OFFSET);
	    }

        OINK_INLINE void set_castling(Piece piece)
        {
            data = (data & ~CASTLING_MASK) | (piece << CASTLING_OFFSET);
        }
    
        OINK_INLINE void set_en_passant(Piece piece)
        {
            data = (data & ~EP_MASK) | (piece << EP_OFFSET);
        }
	};

    // Simple movelist structure, avoiding heap allocation.
    // OINK_TODO: perhaps check if we reach limit, and resize.
    struct MoveVector
    {
        Move     moves[256];
        uint32_t size;

        MoveVector()
        {
            size = 0;
        }

        OINK_INLINE void push_back(Move move)
        {
            moves[size++] = move;
            assert(size < 256);
        }

        OINK_INLINE Move operator[](uint32_t i) const
        {
            return moves[i];
        }

        OINK_INLINE bool empty() const
        {
            return size == 0;
        }
    };
}

#endif