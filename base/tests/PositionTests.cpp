#include <gtest/gtest.h>
#include "../Position.hpp"

using namespace chess;

namespace
{

class PositionTests : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		InitializeConstants();
	}
};

TEST_F(PositionTests, TestThat_SetupStarting_CreatesExpectedPosition)
{
	Position position;
	position.SetupStarting();
	
	ASSERT_EQ(starting::white_king, position.kings[sides::white]);
	ASSERT_EQ(starting::black_king, position.kings[sides::black]);
	ASSERT_EQ(starting::white_king    | 
			  starting::white_queen   | 
			  starting::white_bishops |
			  starting::white_knights |
			  starting::white_rooks   |
			  starting::white_pawns, position.sides[sides::white]);

	ASSERT_EQ(starting::black_king    | 
			  starting::black_queen   | 
			  starting::black_bishops |
			  starting::black_knights |
			  starting::black_rooks   |
			  starting::black_pawns, position.sides[sides::black]);

	ASSERT_EQ(position.sides[sides::black] | position.sides[sides::white], position.wholeBoard);
}
	
TEST_F(PositionTests, TestThat_Clear_DoesClear)
{
	Position position;
	position.Clear();
	
	ASSERT_EQ(util::nil, position.wholeBoard);
}

TEST_F(PositionTests, TestThat_GetEmptySquares_ReturnsFullBoardForClearBoard)
{
	Position position;
	position.Clear();
	
	ASSERT_EQ(util::full, position.GetEmptySquares());
}

TEST_F(PositionTests, TestThat_RemoveCaptured_IsNoOpWhenDiffBitBoardIsEmpty)
{
	Position position;
	position.SetupStarting();

	bitboard before = position.wholeBoard;
	position.RemoveCaptured(sides::white, util::nil);

	ASSERT_EQ(before, position.wholeBoard);
}

TEST_F(PositionTests, TestThat_RemoveCaptured_WorksWithSinglePieceDiff)
{
	Position position;
	position.SetupStarting();

	bitboard before = position.wholeBoard;
	bitboard diff = starting::white_queen;
	position.RemoveCaptured(sides::black, diff);

	bitboard expected = before & ~starting::white_queen;
	ASSERT_EQ(expected, position.wholeBoard);
}

TEST_F(PositionTests, TestThat_RemoveCaptured_WorksWithMultiPieceDiff)
{
	Position position;
	position.SetupStarting();

	bitboard before = position.wholeBoard;
	bitboard diff = starting::white_rooks;
	position.RemoveCaptured(sides::black, diff);

	bitboard expected = before & ~starting::white_rooks;
	ASSERT_EQ(expected, position.wholeBoard);
}

}
