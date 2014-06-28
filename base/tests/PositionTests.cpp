#include <gtest/gtest.h>
#include "../Position.hpp"
#include "../Display.hpp"

using namespace chess;

namespace
{

class PositionTests : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		constants_initialize();
	}
};

TEST_F(PositionTests, TestThat_SetupStarting_CreatesExpectedPosition)
{
	Position position;
	position.setup_starting_position();
	
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

	ASSERT_EQ(position.sides[sides::black] | position.sides[sides::white], position.whole_board);

	print_position(position);
}
	
TEST_F(PositionTests, TestThat_Clear_DoesClear)
{
	Position position;
	position.clear();
	
	ASSERT_EQ(util::nil, position.whole_board);
}

TEST_F(PositionTests, TestThat_get_empty_squares_ReturnsFullBoardForClearBoard)
{
	Position position;
	position.clear();
	
	ASSERT_EQ(util::full, position.get_empty_squares());
}

TEST_F(PositionTests, TestThat_RemoveCaptured_IsNoOpWhenDiffBitboardIsEmpty)
{
	Position position;
	position.setup_starting_position();

	Bitboard before = position.whole_board;
	position.remove_captured(sides::white, util::nil);

	ASSERT_EQ(before, position.whole_board);
}

TEST_F(PositionTests, TestThat_RemoveCaptured_WorksWithSinglePieceDiff)
{
	Position position;
	position.setup_starting_position();

	Bitboard before = position.whole_board;
	Bitboard diff = starting::white_queen;
	position.remove_captured(sides::black, diff);

	Bitboard expected = before & ~starting::white_queen;
	ASSERT_EQ(expected, position.whole_board);
}

TEST_F(PositionTests, TestThat_RemoveCaptured_WorksWithMultiPieceDiff)
{
	Position position;
	position.setup_starting_position();

	Bitboard before = position.whole_board;
	Bitboard diff = starting::white_rooks;
	position.remove_captured(sides::black, diff);

	Bitboard expected = before & ~starting::white_rooks;
	ASSERT_EQ(expected, position.whole_board);
}

}
