#include "../FenParser.hpp"

#include <display/ConsoleDisplay.hpp>

#include <gtest/gtest.h>

using namespace chess;
using namespace chess::fen;
using namespace chess::pieces;
using namespace chess::squares;
using namespace std;

class FenParserTests : public ::testing::Test
{
protected:
    FenParserTests()
    {
        constants_initialize();
    }

	enum ParsingResults
	{
		ParsingOK,
		ParsingFail,
	};

	void test_helper(const string &fen, ParsingResults expected_result, const Position &expected_pos = Position())
	{
		Position got_pos;
		ParsingResults result = ParsingOK;
		try
		{
			got_pos = parse_fen(fen);
		}
		catch (const exception&)
		{
            if (expected_result != ParsingFail)
                throw;
            result = ParsingFail;
		}

		ASSERT_EQ(expected_result, result);
        
		if (expected_result == ParsingOK)
		{
            if (!(got_pos == expected_pos))
            {
                printf("\nGOT:\n");
                print_position(got_pos);
                printf("\nEXPECTED:\n");
                print_position(expected_pos);
            }
            ASSERT_EQ(expected_pos, got_pos);
		}
	}
};

TEST_F(FenParserTests, TestThat_InitialGamePosition_ParsesCorrectly)
{
	string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/";
    Position expected;
    expected.setup_starting_position();

	test_helper(fen, ParsingOK, expected);
}

TEST_F(FenParserTests, TestThat_SicilianGamePosition_ParsesCorrectly)
{
	string fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R/";
	Position expected;
    expected.setup_starting_position();
    expected.manually_move_piece(WHITE_PAWN,   e2, e4);
    expected.manually_move_piece(BLACK_PAWN,   c7, c5);
    expected.manually_move_piece(WHITE_KNIGHT, g1, f3);
    expected.update_sides();
    
	test_helper(fen, ParsingOK, expected);
}

TEST_F(FenParserTests, TestThat_ValidFenString_IsParsedCorrectly_ExampleMiddlegamePosition)
{
	string fen = "1r2k1nr/3q4/2np1p1b/4p3/Q1NPP1pp/2P1BN2/P4PPP/R2R2K1/";
    Position expected;
    expected.place_piece(BLACK_ROOK,   b8);
    expected.place_piece(BLACK_KING,   e8);
    expected.place_piece(BLACK_KNIGHT, g8);
    expected.place_piece(BLACK_ROOK,   h8);
    expected.place_piece(BLACK_QUEEN,  d7);
    expected.place_piece(BLACK_KNIGHT, c6);
    expected.place_piece(BLACK_PAWN,   d6);
    expected.place_piece(BLACK_PAWN,   f6);
    expected.place_piece(BLACK_BISHOP, h6);
    expected.place_piece(BLACK_PAWN,   e5);
    expected.place_piece(WHITE_QUEEN,  a4);
    expected.place_piece(WHITE_KNIGHT, c4);
    expected.place_piece(WHITE_PAWN,   d4);
    expected.place_piece(WHITE_PAWN,   e4);
    expected.place_piece(BLACK_PAWN,   g4);
    expected.place_piece(BLACK_PAWN,   h4);
    expected.place_piece(WHITE_PAWN,  c3);
    expected.place_piece(WHITE_BISHOP, e3);
    expected.place_piece(WHITE_KNIGHT, f3);
    expected.place_piece(WHITE_PAWN,   a2);
    expected.place_piece(WHITE_PAWN,   f2);
    expected.place_piece(WHITE_PAWN,   g2);
    expected.place_piece(WHITE_PAWN,   h2);
    expected.place_piece(WHITE_ROOK,   a1);
    expected.place_piece(WHITE_ROOK,   d1);
    expected.place_piece(WHITE_KING,   g1);
    expected.update_sides();

	test_helper(fen, ParsingOK, expected);
}

//TEST_F(FenParserTests, TestThat_ValidFenString_IsParsedCorrectly_Example1)
//{
//	std::string fen = "5N1k/6p1/7p/4P3/pp2Q3/4q3/1P4PP/2b4K/";
//	int expected[] =
//	{
//		0,  0, 0,  0,  0,   9,  0, 5,
//		0,  0, 0,  0,  0,   0,  6, 0,
//		0,  0, 0,  0,  0,   0,  0, 6,
//		0,  0, 0,  0,  7,   0,  0, 0,
//		6,  6, 0,  0,  11,  0,  0, 0,
//		0,  0, 0,  0,  4,   0,  0, 0,
//		0,  7, 0,  0,  0,   0,  7, 7,
//		0,  0, 3,  0,  0,   0,  0, 12
//	};
//	ParseFenStringTestHelper(fen, ParsingOK, expected);
//}
//
//TEST_F(FenParserTests, TestThat_ValidFenString_IsParsedCorrectly_Example2)
//{
//	std::string fen = "r5k1/2p2ppp/r1p1q3/P2pP3/1P2bP2/2B1Q3/5RPP/R5K1/";
//	int expected[] =
//	{
//		1,  0, 0,  0,  0,   0,  5, 0,
//		0,  0, 6,  0,  0,   6,  6, 6,
//		1,  0, 6,  0,  4,   0,  0, 0,
//		7,  0, 0,  6,  7,   0,  0, 0,
//		0,  7, 0,  0,  3,   7,  0, 0,
//		0,  0, 10, 0,  11,  0,  0, 0,
//		0,  0, 0,  0,  0,   8,  7, 7,
//		8,  0, 0,  0,  0,   0,  12,0
//	};
//	ParseFenStringTestHelper(fen, ParsingOK, expected);
//}
//
TEST_F(FenParserTests, TestThat_FenStringWithTooFewRanks_FailsToParse)
{
	string fen = "5N1k/6p1/7p/4P3/pp2Q3/4q3/1P4PP/";
	test_helper(fen, ParsingFail);
}

TEST_F(FenParserTests, TestThat_FenStringWithOneRank_FailsToParse)
{
	string fen = "5N1k/";
	test_helper(fen, ParsingFail);
}

TEST_F(FenParserTests, TestThat_FenStringWithNoTrailingSlash_FailsToParse)
{
	string fen = "5N1k/6p1/7p/4P3/pp2Q3/4q3/1P4PP/2b4K";
	test_helper(fen, ParsingFail);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}