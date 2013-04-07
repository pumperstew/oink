#ifndef POSITION_HPP
#define POSITION_HPP

#include <cassert>
#include "BasicTypes.hpp"
#include "BasicOperations.hpp"
#include "ChessConstants.hpp"
#include "Move.hpp"

namespace chess
{
    class Position
    {
		bitboard GenerateSide(int side) const;
	public:
		bitboard kings[2];
        bitboard rooks[2];
        bitboard knights[2];
        bitboard bishops[2];
        bitboard queens[2];
        bitboard pawns[2];
        bitboard sides[2];
		bitboard wholeBoard;
		Piece    squares[64];
        //bitboard all_rot45_a1h8, all_rot45_a8h1;

        Position();
        void Clear();
        void SetupStarting();
        void UpdateSides();
		
		void MakeMove(Move move)
		{
			//? how do we know what pieces to update, or do we have to regenerate all pieces from full position??
		}

		Piece GetSquare(int square) const
		{
			return squares[square];
		}

		void SetSquare(int square, Piece contents)
		{
			squares[square] = contents;
		}

        bitboard GetOtherSide(int side) const
        {
            return sides[side ^ 1]; //1 ^ 1 = 0; 0 ^ 1 = 1
        }

        /*bitboard GetSideWithoutKing(int side) const
        {
            return sides[side] & ~kings[side];
        }

        bitboard GetOtherSideWithoutKing(int side) const
        {
            return GetOtherSide(side) & ~kings[side ^ 1];
        }*/

        bitboard GetEmptySquares() const
        {
            return ~wholeBoard;
        }

		void test_rot45();

        //Remove any pieces captured due to an update. 
        //sideCapturing:  the side which did the capture.
		//diff: "diff" bitboard (bits only set where there's a change).
        void RemoveCaptured(int sideCapturing, bitboard diff);

		char GetPieceSymbolAtIndex(int index) const;

        char GetPieceSymbolAtIndex(int rank, int file) const
        {
            return GetPieceSymbolAtIndex(RankFileToIndex(rank, file));
        }

		//Checks that the sides contain no overlaps (two pieces on same square).
        void CheckConsistency() const 
        {
            assert(util::nil == (sides[sides::white] & sides[sides::black]));
        }
    };
}

#endif