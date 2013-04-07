#include <gtest/gtest.h>
#include "../BasicOperations.hpp"
#include "../ChessConstants.hpp"

using namespace chess;
using namespace std;

namespace { //internal linkage

struct BoardIndexRank
{
	bitboard board;
	int	     index;
	bitboard rankOccupancy;

	BoardIndexRank(bitboard board_, int index_, bitboard rankOccupancy_)
		: board(board_), index(index_), rankOccupancy(rankOccupancy_)
	{}
};

struct GenerateSquares
{
	vector<BoardIndexRank> operator()()
	{
		vector<BoardIndexRank> allSquares;
		allSquares.reserve(util::NUM_SQUARES);
		bitboard square = util::one;
		for (int i = 0; i < util::NUM_SQUARES; ++i)
		{
			bitboard rankOccupancy = square >> (IndexToRank(i) << 3);
			allSquares.push_back(BoardIndexRank(square, i, rankOccupancy));
			square <<= util::one;
		}
		return allSquares;
	}
};

class BitwiseOpsTests : public ::testing::TestWithParam<BoardIndexRank> 
{
protected:
	virtual void SetUp()
	{
		InitializeConstants();
	}
};

TEST_P(BitwiseOpsTests, SinglePieceBitboardToIndex_ReturnsExpectedIndices)
{
	ASSERT_EQ(GetParam().index, SinglePieceBitboardToIndex(GetParam().board));
}

TEST_P(BitwiseOpsTests, GetAndClearFirstSetBitReturningIndex_WorksWith_SingleSquareSetOnInitialBoard)
{
	bitboard board = GetParam().board;
	int index;
	bitboard result = GetAndClearFirstSetBitReturningIndex(board, index);
	ASSERT_EQ(GetParam().board, result);
	ASSERT_EQ(util::nil, board);
	ASSERT_EQ(GetParam().index, index);
}

INSTANTIATE_TEST_CASE_P(SingleSquareInputs,
                        BitwiseOpsTests,
                        ::testing::ValuesIn(GenerateSquares()()));

} //anonymous namespace

int main(int argc, char **argv)
{
  //::testing::GTEST_FLAG(catch_exceptions) = false; //Useful for debugging test crashes, as it lets us actually use the debugger properly
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}