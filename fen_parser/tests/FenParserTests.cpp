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
	string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Position expected;
    expected.setup_starting_position();

	test_helper(fen, ParsingOK, expected);
}

TEST_F(FenParserTests, TestThat_SicilianGamePosition_ParsesCorrectly)
{
	string fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
	Position expected;
    expected.setup_starting_position();
    expected.manually_move_piece(WHITE_PAWN,   e2, e4);
    expected.manually_move_piece(BLACK_PAWN,   c7, c5);
    expected.manually_move_piece(WHITE_KNIGHT, g1, f3);
    expected.update_sides();
    expected.fifty_move_count = 1;
	test_helper(fen, ParsingOK, expected);
}

TEST_F(FenParserTests, TestThat_ValidFenString_IsParsedCorrectly_ExampleMiddlegamePosition)
{
	string fen = "1r2k1nr/3q4/2np1p1b/4p3/Q1NPP1pp/2P1BN2/P4PPP/R2R2K1 w KQkq - 1 20";
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

    expected.fifty_move_count = 1;
    expected.castling_rights = sides::CASTLING_RIGHTS_ANY_BLACK | sides::CASTLING_RIGHTS_ANY_WHITE;

	test_helper(fen, ParsingOK, expected);
}

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

TEST_F(FenParserTests, TestThat_CastlingRightsAreParsedCorrectly)
{
	string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    auto pos = parse_fen(fen);
    ASSERT_EQ(pos.castling_rights, sides::CASTLING_RIGHTS_ANY_BLACK | sides::CASTLING_RIGHTS_ANY_WHITE);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w K - 0 1";
    pos = parse_fen(fen);
    ASSERT_EQ(pos.castling_rights, sides::CASTLING_RIGHTS_WHITE_KINGSIDE);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQ - 0 1";
    pos = parse_fen(fen);
    ASSERT_EQ(pos.castling_rights, sides::CASTLING_RIGHTS_WHITE_KINGSIDE | sides::CASTLING_RIGHTS_WHITE_QUEENSIDE);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQk - 0 1";
    pos = parse_fen(fen);
    ASSERT_EQ(pos.castling_rights, sides::CASTLING_RIGHTS_WHITE_KINGSIDE | sides::CASTLING_RIGHTS_WHITE_QUEENSIDE | sides::CASTLING_RIGHTS_BLACK_KINGSIDE);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w kQ - 0 1";
    pos = parse_fen(fen);
    ASSERT_EQ(pos.castling_rights, sides::CASTLING_RIGHTS_BLACK_KINGSIDE | sides::CASTLING_RIGHTS_WHITE_QUEENSIDE);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1";
    pos = parse_fen(fen);
    ASSERT_EQ(pos.castling_rights, 0);
}

TEST_F(FenParserTests, TestThat_EPSquareIsParsedCorrectly)
{
    string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    auto pos = parse_fen(fen);
    ASSERT_EQ(pos.ep_target_square, squares::NO_SQUARE);

	fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e3 0 1";
    pos = parse_fen(fen);
    ASSERT_EQ(pos.ep_target_square, squares::e3);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq h8 0 1";
    test_helper(fen, ParsingFail); // bad rank

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq b6 0 1";
    pos = parse_fen(fen);
    ASSERT_EQ(pos.ep_target_square, squares::b6);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq h0 0 1";
	test_helper(fen, ParsingFail);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq bb 0 1";
	test_helper(fen, ParsingFail);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq 44 0 1";
	test_helper(fen, ParsingFail);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq f 0 1";
	test_helper(fen, ParsingFail);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq 2 0 1";
	test_helper(fen, ParsingFail);
}

TEST_F(FenParserTests, TestThat_FiftyMoveClockIsParsedCorrectly)
{
    string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    auto pos = parse_fen(fen);
    ASSERT_EQ(pos.fifty_move_count, 0);

	fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 11 1";
    pos = parse_fen(fen);
    ASSERT_EQ(pos.fifty_move_count, 11);
}

TEST_F(FenParserTests, TestThat_FullMoveCountIsParsedCorrectly)
{
    string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    int move_count;
    auto pos = parse_fen(fen, &move_count);
    ASSERT_EQ(move_count, 1);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 84";
    pos = parse_fen(fen, &move_count);
    ASSERT_EQ(move_count, 84);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";
	test_helper(fen, ParsingFail);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 08";
	test_helper(fen, ParsingFail);
}

TEST_F(FenParserTests, TestThat_SideToMoveIsParsedCorrectly)
{
    string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Side side_to_move;
    auto pos = parse_fen(fen, nullptr, &side_to_move);
    ASSERT_EQ(side_to_move, sides::white);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 84";
    pos = parse_fen(fen, nullptr, &side_to_move);
    ASSERT_EQ(side_to_move, sides::black);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR q KQkq - 0 08";
	test_helper(fen, ParsingFail);

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR t KQkq - 0 08";
	test_helper(fen, ParsingFail);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}