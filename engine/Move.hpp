#ifndef MOVE_HPP
#define MOVE_HPP

#include "ChessConstants.hpp"

#include <vector>

namespace chess
{
    /*************

    Storage scheme:

	* data has 32 bits
	* source and destination square are on the interval [0, 63] or [0, 0b111111] or [0, 0x3f] => 6 bits each.
	* for the piece moving, there are 14 possibilities (technically, although we'll include zero too for 15), so we need 4 bits on [0, 0xf]
	* for the captured piece, 15 possibilities (including none == 0) -- again 4 bits.
	* if we promote, castle, or do an en-passant capture with this move, we store the piece promoting to, or the relevant king, or the relevant pawn -- again 4 bits.
    * this works because promotions are never to king or pawn.
	layout (LSB on left)

	[source:6][destination:6][piece:4][captured:4][promotion/castling/en-passant ("special"):4]

	*************/

	class Move
	{
    public:
		typedef uint32_t MoveData;

    private:
	    static const MoveData ALLBITS = 0xffffffff;
	    static const int DESTINATION_OFFSET = 6;
	    static const int PIECE_OFFSET		= 12;
	    static const int CAPTURED_OFFSET    = 16;
	    static const int SPECIAL_OFFSET     = 20;

	    static const MoveData SOURCE_MASK      = 0x3f;
	    static const MoveData DESTINATION_MASK = 0x3f << DESTINATION_OFFSET;
	    static const MoveData PIECE_MASK       = 0xf  << PIECE_OFFSET;
	    static const MoveData CAPTURED_MASK    = 0xf  << CAPTURED_OFFSET;
	    static const MoveData SPECIAL_MASK     = 0xf  << SPECIAL_OFFSET;
	    static const MoveData NOT_SOURCE_MASK       = ALLBITS ^ SOURCE_MASK;
	    static const MoveData NOT_DESTINATION_MASK  = ALLBITS ^ DESTINATION_MASK;
	    static const MoveData NOT_PIECE_MASK        = ALLBITS ^ PIECE_MASK;
	    static const MoveData NOT_CAPTURED_MASK     = ALLBITS ^ CAPTURED_MASK;
	    static const MoveData NOT_SPECIAL_MASK      = ALLBITS ^ SPECIAL_MASK;

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

        OINK_INLINE Piece get_special() const
        {
            return (data & SPECIAL_MASK) >> SPECIAL_OFFSET;
        }

	    OINK_INLINE Piece get_promotion_piece() const
	    {
		    Piece special = get_special();
            return (special == pieces::WHITE_KING || special == pieces::BLACK_KING || special == pieces::WHITE_PAWN || special == pieces::BLACK_PAWN) ?
                    pieces::NONE : special;
	    }

        OINK_INLINE Piece get_castling() const
	    {
		    Piece special = get_special();
            return (special == pieces::WHITE_KING || special == pieces::BLACK_KING) ? special : pieces::NONE;
	    }

        OINK_INLINE Piece get_en_passant() const
	    {
		    Piece special = get_special();
            return (special == pieces::WHITE_PAWN || special == pieces::BLACK_PAWN) ? special : pieces::NONE;
	    }

	    OINK_INLINE void set_source(Square square)
	    {
		    data = (data & NOT_SOURCE_MASK) | (MoveData)square;
	    }

	    OINK_INLINE void set_destination(Square square)
	    {
		    data = (data & NOT_DESTINATION_MASK) | ((MoveData)square << DESTINATION_OFFSET);
	    }

	    OINK_INLINE void set_piece(Piece piece)
	    {
		    data = (data & NOT_PIECE_MASK) | (piece << PIECE_OFFSET);
	    }

	    OINK_INLINE void set_captured_piece(Piece piece)
	    {
		    data = (data & NOT_CAPTURED_MASK) | (piece << CAPTURED_OFFSET);
	    }

	    OINK_INLINE void set_promotion_piece(Piece piece)
	    {
		    data = (data & NOT_SPECIAL_MASK) | (piece << SPECIAL_OFFSET);
	    }

        OINK_INLINE void set_castling(Piece piece)
        {
            data = (data & NOT_SPECIAL_MASK) | (piece << SPECIAL_OFFSET);
        }
    
        OINK_INLINE void set_en_passant(Piece piece)
        {
            data = (data & NOT_SPECIAL_MASK) | (piece << SPECIAL_OFFSET);
        }
	};

	typedef std::vector<Move> MoveVector;
}

#endif