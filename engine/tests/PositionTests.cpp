#include <engine/Position.hpp>
#include <display/ConsoleDisplay.hpp>

#include <gtest/gtest.h>

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
	ASSERT_EQ(util::nil, position.whole_board);
}

TEST_F(PositionTests, TestThat_get_empty_squares_ReturnsFullBoardForClearBoard)
{
	Position position;
	ASSERT_EQ(util::full, position.get_empty_squares());
}

}
