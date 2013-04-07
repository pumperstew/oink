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
		int GetSource() const;
		int GetDestination() const;
		Piece GetCapturedPiece() const;
		Piece GetPromotionPiece() const;

		void SetPiece(Piece piece);
		void SetSource(int square);
		void SetDestination(int square);
		void SetCapturedPiece(Piece piece);
		void SetPromotionPiece(Piece piece);
	};

	typedef std::vector<Move> MoveVector;


}

#endif