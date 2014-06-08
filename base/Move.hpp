#ifndef MOVE_HPP
#define MOVE_HPP

#include "ChessConstants.hpp"
#include <vector>

namespace chess
{
	class Move
	{
	public:
		typedef unsigned int MoveData;

		MoveData data;

		Move() { data = 0; }

		Piece GetPiece() const;
		Square GetSource() const;
		Square GetDestination() const;
		Piece GetCapturedPiece() const;
		Piece GetPromotionPiece() const;
        Piece GetCastling() const;
        Piece GetEnPassant() const;
        Piece GetSpecial() const;

		void SetPiece(Piece piece);
		void SetSource(Square square);
		void SetDestination(Square square);
		void SetCapturedPiece(Piece piece);
		void SetPromotionPiece(Piece piece);
        void SetCastling(Piece piece);
        void SetEnPassant(Piece piece);
	};

	typedef std::vector<Move> MoveVector;


}

#endif