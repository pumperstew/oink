#ifndef BASICOPERATIONS_HPP
#define BASICOPERATIONS_HPP

#include "BasicTypes.hpp"

namespace chess 
{
	int SinglePieceBitboardToIndex(bitboard singlePiece);
	int GetFirstIndexAndClear(bitboard &b);
	bitboard GetAndClearFirstSetBitReturningIndex(bitboard &b, int &sq);
	bitboard GetAndClearFirstSetBit(bitboard &b);
	
	bitboard GetRankOccupancy(bitboard b, int rank);
	bitboard GetFileOccupancy(bitboard b, int rank);
	bitboard GetDiagonalOccupancy_a1h8(bitboard b, int square);
	bitboard GetDiagonalOccupancy_a8h1(bitboard b, int square);

	void IndexToRankAndFile(int index, int &rank, int &file);
	int  RankFileToIndex(int rank, int file);
	int  IndexToRank(int index);
	
	bitboard ExcludeNextRank(bitboard pos, int rank, int side);
	/*
	bitboard get_first_set_bit(bitboard b);
	bitboard get_and_clear_first_set_bit(bitboard &b);
	bitboard get_first_set_bit_with_index(bitboard b, int &sq);
	
	bitboard clear_first_set_bit(bitboard b);*/
}

#endif //BASICOPERATIONS_HPP

