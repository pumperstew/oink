#include "Move.hpp"

namespace chess
{
	/*storage scheme:

	* data has 32 bits
	* source and destination square are on the interval [0, 63] or [0, 0b111111] or [0, 0x3f] => 6 bits each.
	* for the piece moving, there are 14 possibilities (technically, although we'll include zero too for 15), so we need 4 bits on [0, 0xf]
	* for the captured piece, 15 possibilities (including none == 0) -- again 4 bits.
	* if we promote with this move, we need to store the piece we promote to -- again 4 bits.
	layout (LSB on left)

	[source:6][destination:6][piece:4][captured:4][promotion piece:4] ..OINK_TODO: rest

	*/

	static const Move::MoveData ALLBITS = 0xffffffff;
	static const int DESTINATION_OFFSET = 6;
	static const int PIECE_OFFSET		= 12;
	static const int CAPTURED_OFFSET    = 16;
	static const int PROMOTION_OFFSET   = 20;
	static const Move::MoveData SOURCE_MASK      = 0x3f;
	static const Move::MoveData DESTINATION_MASK = 0x3f << DESTINATION_OFFSET;
	static const Move::MoveData PIECE_MASK       = 0xf  << PIECE_OFFSET;
	static const Move::MoveData CAPTURED_MASK    = 0xf  << CAPTURED_OFFSET;
	static const Move::MoveData PROMOTION_MASK   = 0xf  << PROMOTION_OFFSET;
	static const Move::MoveData NOT_SOURCE_MASK       = ALLBITS ^ SOURCE_MASK;
	static const Move::MoveData NOT_DESTINATION_MASK  = ALLBITS ^ DESTINATION_MASK;
	static const Move::MoveData NOT_PIECE_MASK        = ALLBITS ^ PIECE_MASK;
	static const Move::MoveData NOT_CAPTURED_MASK     = ALLBITS ^ CAPTURED_MASK;
	static const Move::MoveData NOT_PROMOTION_MASK    = ALLBITS ^ PROMOTION_MASK;

	int Move::GetSource() const
	{
		return data & SOURCE_MASK;
	}

	int Move::GetDestination() const
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
		return (data & PROMOTION_MASK) >> PROMOTION_OFFSET;
	}

	void Move::SetSource(int square)
	{
		data = (data & NOT_SOURCE_MASK) | (MoveData)square;
	}

	void Move::SetDestination(int square)
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
		data = (data & NOT_PROMOTION_MASK) | (piece << PROMOTION_OFFSET);
	}
}