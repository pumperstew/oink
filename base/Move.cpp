#include "Move.hpp"

namespace chess
{
	/*storage scheme:

	* data has 32 bits
	* source and destination square are on the interval [0, 63] or [0, 0b111111] or [0, 0x3f] => 6 bits each.
	* for the piece moving, there are 14 possibilities (technically, although we'll include zero too for 15), so we need 4 bits on [0, 0xf]
	* for the captured piece, 15 possibilities (including none == 0) -- again 4 bits.
	* if we promote, castle, or do an en-passant capture with this move, we store the piece promoting to, or the relevant king, or the relevant pawn -- again 4 bits.
    * this works because promotions are never to king or pawn.
	layout (LSB on left)

	[source:6][destination:6][piece:4][captured:4][promotion/castling/en-passant ("special"):4]

	*/

	static const Move::MoveData ALLBITS = 0xffffffff;
	static const int DESTINATION_OFFSET = 6;
	static const int PIECE_OFFSET		= 12;
	static const int CAPTURED_OFFSET    = 16;
	static const int SPECIAL_OFFSET     = 20;

	static const Move::MoveData SOURCE_MASK      = 0x3f;
	static const Move::MoveData DESTINATION_MASK = 0x3f << DESTINATION_OFFSET;
	static const Move::MoveData PIECE_MASK       = 0xf  << PIECE_OFFSET;
	static const Move::MoveData CAPTURED_MASK    = 0xf  << CAPTURED_OFFSET;
	static const Move::MoveData SPECIAL_MASK     = 0xf  << SPECIAL_OFFSET;
	static const Move::MoveData NOT_SOURCE_MASK       = ALLBITS ^ SOURCE_MASK;
	static const Move::MoveData NOT_DESTINATION_MASK  = ALLBITS ^ DESTINATION_MASK;
	static const Move::MoveData NOT_PIECE_MASK        = ALLBITS ^ PIECE_MASK;
	static const Move::MoveData NOT_CAPTURED_MASK     = ALLBITS ^ CAPTURED_MASK;
	static const Move::MoveData NOT_SPECIAL_MASK      = ALLBITS ^ SPECIAL_MASK;

	Square Move::GetSource() const
	{
		return data & SOURCE_MASK;
	}

	Square Move::GetDestination() const
	{
		return (data & DESTINATION_MASK) >> DESTINATION_OFFSET;
	}
		
	Piece Move::GetPiece() const
	{
		return (data & PIECE_MASK) >> PIECE_OFFSET;
	}

	Piece Move::GetCapturedPiece() const
	{
		return (data & CAPTURED_MASK) >> CAPTURED_OFFSET;
	}

	Piece Move::GetPromotionPiece() const
	{
		auto special = (data & SPECIAL_MASK) >> SPECIAL_OFFSET;
        return (special == pieces::WHITE_KING || special == pieces::BLACK_KING || special == pieces::WHITE_PAWN || special == pieces::BLACK_PAWN) ?
                pieces::NONE : special;
	}

    Piece Move::GetCastling() const
	{
		auto special = (data & SPECIAL_MASK) >> SPECIAL_OFFSET;
        return (special == pieces::WHITE_KING || special == pieces::BLACK_KING) ?
                special : pieces::NONE;
	}

    Piece Move::GetEnPassant() const
	{
		auto special = (data & SPECIAL_MASK) >> SPECIAL_OFFSET;
        return (special == pieces::WHITE_PAWN || special == pieces::BLACK_PAWN) ?
                special : pieces::NONE;
	}

    Piece Move::GetSpecial() const
    {
        return (data & SPECIAL_MASK) >> SPECIAL_OFFSET;
    }

	void Move::SetSource(Square square)
	{
		data = (data & NOT_SOURCE_MASK) | (MoveData)square;
	}

	void Move::SetDestination(Square square)
	{
		data = (data & NOT_DESTINATION_MASK) | ((MoveData)square << DESTINATION_OFFSET);
	}

	void Move::SetPiece(Piece piece)
	{
		data = (data & NOT_PIECE_MASK) | (piece << PIECE_OFFSET);
	}

	void Move::SetCapturedPiece(Piece piece)
	{
		data = (data & NOT_CAPTURED_MASK) | (piece << CAPTURED_OFFSET);
	}

	void Move::SetPromotionPiece(Piece piece)
	{
		data = (data & NOT_SPECIAL_MASK) | (piece << SPECIAL_OFFSET);
	}

    void Move::SetCastling(Piece piece)
    {
        data = (data & NOT_SPECIAL_MASK) | (piece << SPECIAL_OFFSET);
    }
    
    void Move::SetEnPassant(Piece piece)
    {
        data = (data & NOT_SPECIAL_MASK) | (piece << SPECIAL_OFFSET);
    }
}