#include <engine/BasicOperations.hpp>

#include <gtest/gtest.h>

using namespace chess;
using namespace std;

namespace { // internal only

struct BoardIndexRank
{
	Bitboard board;
	Square	 index;
	Bitboard rankOccupancy;

	BoardIndexRank(Bitboard board_, Square index_, Bitboard rankOccupancy_)
		: board(board_), index(index_), rankOccupancy(rankOccupancy_)
	{}
};

struct GenerateSquares
{
	vector<BoardIndexRank> operator()()
	{
		vector<BoardIndexRank> allSquares;
		allSquares.reserve(util::NUM_SQUARES);
		Bitboard square = util::one;
		for (Square i = 0; i < util::NUM_SQUARES; ++i)
		{
			Bitboard rankOccupancy = square >> (square_to_rank(i) << 3);
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
		constants_initialize();
	}
};

TEST_P(BitwiseOpsTests, get_and_clear_first_occ_square_WorksWith_SingleSquareSetOnInitialBoard)
{
	Bitboard board = GetParam().board;
	Square index;
	board = get_and_clear_first_occ_square(board, &index);
	ASSERT_EQ(util::nil, board);
	ASSERT_EQ(GetParam().index, index);
}

TEST_P(BitwiseOpsTests, get_first_occ_square_WorksWith_SingleSquareSetOnInitialBoard)
{
	Bitboard board = GetParam().board;
    Square index = get_first_occ_square(board);
	ASSERT_EQ(GetParam().index, index);
}

INSTANTIATE_TEST_CASE_P(SingleSquareInputs,
                        BitwiseOpsTests,
                        ::testing::ValuesIn(GenerateSquares()()));

} //anonymous namespace

int main(int argc, char **argv)
{
  ::testing::GTEST_FLAG(catch_exceptions) = false; //Useful for debugging test crashes, as it lets us actually use the debugger properly
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}