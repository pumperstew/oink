#ifndef BASICOPERATIONS_HPP
#define BASICOPERATIONS_HPP

#include "BasicTypes.hpp"

namespace chess 
{
	Square SinglePieceBitboardToIndex(bitboard singlePiece);
	Square GetFirstIndexAndClear(bitboard &b);
    Square GetFirstIndex(bitboard b);
	bitboard GetAndClearFirstSetBitReturningIndex(bitboard &b, Square &sq);
	bitboard GetAndClearFirstSetBit(bitboard &b);
	
    bool IsSquareOccupied(bitboard b, Square square);
	bitboard GetRankOccupancy(bitboard b, RankFile rank);
	bitboard GetFileOccupancy(bitboard b, RankFile rank);
	bitboard GetDiagonalOccupancy_a1h8(bitboard b, Square square);
	bitboard GetDiagonalOccupancy_a8h1(bitboard b, Square square);

	void   IndexToRankAndFile(Square index, RankFile &rank, RankFile &file);
	Square RankFileToIndex(RankFile rank, RankFile file);
	RankFile    IndexToRank(Square index);
	
	bitboard ExcludeFourthOrFifthRank(bitboard pos, Side side);
}

#endif //BASICOPERATIONS_HPP

