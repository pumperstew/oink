#include <gtest/gtest.h>
#include "../MoveGenerator.hpp"

#include <boost/range/algorithm.hpp>

using namespace chess;

namespace
{

class MoveGeneratorTests : public ::testing::Test
{
protected:
	Position	   position;
	MoveGenerator  generator;

	virtual void SetUp()
	{
		InitializeConstants();
		position.Clear();
	}
};

TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesNothing_WhenThereAreNoKnights)
{
	auto moves = generator.GenerateKnightMoves(position, sides::white);
	ASSERT_TRUE(moves.empty());
}

TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesNothing_WhenThereAreNoKnights_OfTheSpecifiedSide)
{
	position.knights[sides::black] = starting::black_knights;
	position.UpdateSides();

	auto moves = generator.GenerateKnightMoves(position, sides::white);

	ASSERT_TRUE(moves.empty());
}

void SetWhiteKnightAt(Position &position, int square)
{
	position.knights[sides::white] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::WHITE_KNIGHT;
}

void SetWhiteKingAt(Position &position, int square)
{
	position.kings[sides::white] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::WHITE_KING;
}

void SetWhitePawnAt(Position &position, int square)
{
	position.pawns[sides::white] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::WHITE_PAWN;
}

void SetWhiteRookAt(Position &position, int square)
{
	position.rooks[sides::white] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::WHITE_ROOK;
}

void SetWhiteBishopAt(Position &position, int square)
{
	position.bishops[sides::white] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::WHITE_BISHOP;
}

void SetBlackPawnAt(Position &position, int square)
{
	position.pawns[sides::black] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::BLACK_PAWN;
}

void SetBlackBishopAt(Position &position, int square)
{
	position.bishops[sides::black] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::BLACK_BISHOP;
}

void CheckMoveIsInList(const MoveVector &moves, int source, int destination, Piece piece, Piece captured, Piece promotion = pieces::NONE)
{
	auto moveIt = boost::find_if(moves, [&](const Move& move) { 
		return move.GetSource()			== source && 
			   move.GetDestination()    == destination && 
			   move.GetPromotionPiece() == promotion &&
			   move.GetPiece()			== piece &&
			   move.GetCapturedPiece()  == captured;
	});
	ASSERT_NE(moves.end(), moveIt);
}

//minimal targets: 2
TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesExpectedMovesForKnightOn_a8_OnOtherwiseClearBoard)
{
	SetWhiteKnightAt(position, squares::a8);

	auto moves = generator.GenerateKnightMoves(position, sides::white);

	ASSERT_EQ(2, moves.size());
	CheckMoveIsInList(moves, squares::a8, squares::c7, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::b6, pieces::WHITE_KNIGHT, pieces::NONE);
}

//maximal targets: 8
TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesExpectedMovesForCentralKnight_OnOtherwiseClearBoard)
{
	SetWhiteKnightAt(position, squares::e5);

	auto moves = generator.GenerateKnightMoves(position, sides::white);

	ASSERT_EQ(8, moves.size());
	CheckMoveIsInList(moves, squares::e5, squares::f7, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::f3, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::d7, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::d3, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g4, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g6, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c4, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c6, pieces::WHITE_KNIGHT, pieces::NONE);
}

//middling
TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesExpectedMovesForKnightOnG3_OnOtherwiseClearBoard)
{
	SetWhiteKnightAt(position, squares::g3);

	auto moves = generator.GenerateKnightMoves(position, sides::white);

	ASSERT_EQ(6, moves.size());
	CheckMoveIsInList(moves, squares::g3, squares::h5, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::g3, squares::h1, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::g3, squares::f5, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::g3, squares::f1, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::g3, squares::e4, pieces::WHITE_KNIGHT, pieces::NONE);
	CheckMoveIsInList(moves, squares::g3, squares::e2, pieces::WHITE_KNIGHT, pieces::NONE);
}

//middling
TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesExpectedMovesForKnightOn_a8_WithCapture)
{
	SetWhiteKnightAt(position, squares::a8);
	SetBlackPawnAt(position, squares::c7);

	auto moves = generator.GenerateKnightMoves(position, sides::white);

	ASSERT_EQ(2, moves.size());
	CheckMoveIsInList(moves, squares::a8, squares::c7, pieces::WHITE_KNIGHT, pieces::BLACK_PAWN);
	CheckMoveIsInList(moves, squares::a8, squares::b6, pieces::WHITE_KNIGHT, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateKingMoves_GeneratesExpectedMovesForKingOn_a8_OnOtherwiseClearBoard)
{
	SetWhiteKingAt(position, squares::a8);

	auto moves = generator.GenerateKingMoves(position, sides::white);

	ASSERT_EQ(3, moves.size());
	CheckMoveIsInList(moves, squares::a8, squares::b8, pieces::WHITE_KING, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::b7, pieces::WHITE_KING, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a7, pieces::WHITE_KING, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateKingMoves_GeneratesExpectedMovesForKingOn_e5_OnOtherwiseClearBoard)
{
	SetWhiteKingAt(position, squares::e5);

	auto moves = generator.GenerateKingMoves(position, sides::white);

	ASSERT_EQ(8, moves.size());
	CheckMoveIsInList(moves, squares::e5, squares::e6, pieces::WHITE_KING, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::e4, pieces::WHITE_KING, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::f5, pieces::WHITE_KING, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::d5, pieces::WHITE_KING, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::f6, pieces::WHITE_KING, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::d6, pieces::WHITE_KING, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::d4, pieces::WHITE_KING, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::f4, pieces::WHITE_KING, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_OnOtherwiseClearBoard)
{
	SetWhitePawnAt(position, squares::e2);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(2, moves.size());
	CheckMoveIsInList(moves, squares::e2, squares::e3, pieces::WHITE_PAWN, pieces::NONE);
	CheckMoveIsInList(moves, squares::e2, squares::e4, pieces::WHITE_PAWN, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e3_OnOtherwiseClearBoard)
{
	SetWhitePawnAt(position, squares::e3);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(1, moves.size());
	CheckMoveIsInList(moves, squares::e3, squares::e4, pieces::WHITE_PAWN, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithBlackPieceOn_f3)
{
	SetWhitePawnAt(position, squares::e2);
	SetBlackPawnAt(position, squares::f3);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(3, moves.size());
	CheckMoveIsInList(moves, squares::e2, squares::e3, pieces::WHITE_PAWN, pieces::NONE);
	CheckMoveIsInList(moves, squares::e2, squares::e4, pieces::WHITE_PAWN, pieces::NONE);
	CheckMoveIsInList(moves, squares::e2, squares::f3, pieces::WHITE_PAWN, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithBlackPieceOn_f3_and_d3)
{
	SetWhitePawnAt(position, squares::e2);
	SetBlackPawnAt(position, squares::f3);
	SetBlackPawnAt(position, squares::d3);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(4, moves.size());
	CheckMoveIsInList(moves, squares::e2, squares::e3, pieces::WHITE_PAWN, pieces::NONE);
	CheckMoveIsInList(moves, squares::e2, squares::e4, pieces::WHITE_PAWN, pieces::NONE);
	CheckMoveIsInList(moves, squares::e2, squares::f3, pieces::WHITE_PAWN, pieces::BLACK_PAWN);
	CheckMoveIsInList(moves, squares::e2, squares::d3, pieces::WHITE_PAWN, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_c7_OnOtherwiseClearBoard)
{
	SetWhitePawnAt(position, squares::c7);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(4, moves.size());
	CheckMoveIsInList(moves, squares::c7, squares::c8, pieces::WHITE_PAWN, pieces::NONE, pieces::WHITE_QUEEN);
	CheckMoveIsInList(moves, squares::c7, squares::c8, pieces::WHITE_PAWN, pieces::NONE, pieces::WHITE_ROOK);
	CheckMoveIsInList(moves, squares::c7, squares::c8, pieces::WHITE_PAWN, pieces::NONE, pieces::WHITE_BISHOP);
	CheckMoveIsInList(moves, squares::c7, squares::c8, pieces::WHITE_PAWN, pieces::NONE, pieces::WHITE_KNIGHT);
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_c7_WithBlackPieceOn_b8)
{
	SetWhitePawnAt(position, squares::c7);
	SetBlackBishopAt(position, squares::b8);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(8, moves.size());
	CheckMoveIsInList(moves, squares::c7, squares::c8, pieces::WHITE_PAWN, pieces::NONE, pieces::WHITE_QUEEN);
	CheckMoveIsInList(moves, squares::c7, squares::c8, pieces::WHITE_PAWN, pieces::NONE, pieces::WHITE_ROOK);
	CheckMoveIsInList(moves, squares::c7, squares::c8, pieces::WHITE_PAWN, pieces::NONE, pieces::WHITE_BISHOP);
	CheckMoveIsInList(moves, squares::c7, squares::c8, pieces::WHITE_PAWN, pieces::NONE, pieces::WHITE_KNIGHT);
	CheckMoveIsInList(moves, squares::c7, squares::b8, pieces::WHITE_PAWN, pieces::BLACK_BISHOP, pieces::WHITE_QUEEN);
	CheckMoveIsInList(moves, squares::c7, squares::b8, pieces::WHITE_PAWN, pieces::BLACK_BISHOP, pieces::WHITE_ROOK);
	CheckMoveIsInList(moves, squares::c7, squares::b8, pieces::WHITE_PAWN, pieces::BLACK_BISHOP, pieces::WHITE_BISHOP);
	CheckMoveIsInList(moves, squares::c7, squares::b8, pieces::WHITE_PAWN, pieces::BLACK_BISHOP, pieces::WHITE_KNIGHT);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_a8_OnOtherwiseClearBoard)
{
	SetWhiteRookAt(position, squares::a8);

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMoveIsInList(moves, squares::a8, squares::b8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::c8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::d8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::e8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::f8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::g8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::h8, pieces::WHITE_ROOK, pieces::NONE);

	CheckMoveIsInList(moves, squares::a8, squares::a7, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a6, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a5, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a4, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a3, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a2, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a1, pieces::WHITE_ROOK, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_e5_OnOtherwiseClearBoard)
{
	SetWhiteRookAt(position, squares::e5);

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMoveIsInList(moves, squares::e5, squares::e6, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::e7, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::e8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::e4, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::e3, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::e2, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::e1, pieces::WHITE_ROOK, pieces::NONE);

	CheckMoveIsInList(moves, squares::e5, squares::d5, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c5, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::b5, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::a5, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::f5, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g5, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::h5, pieces::WHITE_ROOK, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_h1_OnOtherwiseClearBoard)
{
	SetWhiteRookAt(position, squares::h1);

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMoveIsInList(moves, squares::h1, squares::h2, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::h3, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::h4, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::h5, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::h6, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::h7, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::h8, pieces::WHITE_ROOK, pieces::NONE);

	CheckMoveIsInList(moves, squares::h1, squares::g1, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::f1, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::e1, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::d1, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::c1, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::b1, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::h1, squares::a1, pieces::WHITE_ROOK, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_a8_WithBlackPieceOn_a3)
{
	SetWhiteRookAt(position, squares::a8);
	SetBlackPawnAt(position, squares::a3);

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(12, moves.size());
	CheckMoveIsInList(moves, squares::a8, squares::b8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::c8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::d8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::e8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::f8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::g8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::h8, pieces::WHITE_ROOK, pieces::NONE);

	CheckMoveIsInList(moves, squares::a8, squares::a7, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a6, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a5, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a4, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a3, pieces::WHITE_ROOK, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_a8_WithBlackPieceOn_a1)
{
	SetWhiteRookAt(position, squares::a8);
	SetBlackBishopAt(position, squares::a1);

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMoveIsInList(moves, squares::a8, squares::b8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::c8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::d8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::e8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::f8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::g8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::h8, pieces::WHITE_ROOK, pieces::NONE);

	CheckMoveIsInList(moves, squares::a8, squares::a7, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a6, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a5, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a4, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a3, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a2, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a1, pieces::WHITE_ROOK, pieces::BLACK_BISHOP);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_a8_WithWhitePieceOn_a3)
{
	SetWhiteRookAt(position, squares::a8);
	SetWhiteKingAt(position, squares::a3);

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(11, moves.size());
	CheckMoveIsInList(moves, squares::a8, squares::b8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::c8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::d8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::e8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::f8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::g8, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::h8, pieces::WHITE_ROOK, pieces::NONE);

	CheckMoveIsInList(moves, squares::a8, squares::a7, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a6, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a5, pieces::WHITE_ROOK, pieces::NONE);
	CheckMoveIsInList(moves, squares::a8, squares::a4, pieces::WHITE_ROOK, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_a1_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::a1);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(7, moves.size());
	CheckMoveIsInList(moves, squares::a1, squares::b2, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::a1, squares::c3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::a1, squares::d4, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::a1, squares::e5, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::a1, squares::f6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::a1, squares::g7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::a1, squares::h8, pieces::WHITE_BISHOP, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_b2_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::b2);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(9, moves.size());
	
	CheckMoveIsInList(moves, squares::b2, squares::c3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::b2, squares::d4, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::b2, squares::e5, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::b2, squares::f6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::b2, squares::g7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::b2, squares::h8, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::b2, squares::a1, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::b2, squares::c1, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::b2, squares::a3, pieces::WHITE_BISHOP, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::e5);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(13, moves.size());
	
	CheckMoveIsInList(moves, squares::e5, squares::f6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::h8, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::d4, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::b2, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::a1, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::e5, squares::d6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::b8, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::f4, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::h2, pieces::WHITE_BISHOP, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_a1_WithBlackPieceOn_c3)
{
	SetWhiteBishopAt(position, squares::a1);
	SetBlackPawnAt(position, squares::c3);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(2, moves.size());
	CheckMoveIsInList(moves, squares::a1, squares::b2, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::a1, squares::c3, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_c4_WithBlackPieceOn_b3)
{
	SetWhiteBishopAt(position, squares::c4);
	SetBlackPawnAt(position, squares::b3);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(10, moves.size());
	
	CheckMoveIsInList(moves, squares::c4, squares::d5, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::c4, squares::e6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::c4, squares::f7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::c4, squares::g8, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::c4, squares::b3, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);

	CheckMoveIsInList(moves, squares::c4, squares::b5, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::c4, squares::a6, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::c4, squares::d3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::c4, squares::e2, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::c4, squares::f1, pieces::WHITE_BISHOP, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_b2)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::b2);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(12, moves.size());
	
	CheckMoveIsInList(moves, squares::e5, squares::f6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::h8, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::d4, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::b2, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);

	CheckMoveIsInList(moves, squares::e5, squares::d6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::b8, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::f4, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::h2, pieces::WHITE_BISHOP, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_c7)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::c7);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(12, moves.size());
	
	CheckMoveIsInList(moves, squares::e5, squares::f6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::h8, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::e5, squares::d4, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::b2, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::a1, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::e5, squares::d6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c7, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);

	CheckMoveIsInList(moves, squares::e5, squares::f4, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::h2, pieces::WHITE_BISHOP, pieces::NONE);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_f4)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f4);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(11, moves.size());
	
	CheckMoveIsInList(moves, squares::e5, squares::f6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::h8, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::e5, squares::d4, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::b2, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::a1, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::e5, squares::d6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::b8, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::e5, squares::f4, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

//No clash, so same as clear board.
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_f3)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f3);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(13, moves.size());
	
	CheckMoveIsInList(moves, squares::e5, squares::f6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::h8, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::e5, squares::d4, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::b2, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::a1, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::e5, squares::d6, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::c7, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::b8, pieces::WHITE_BISHOP, pieces::NONE);

	CheckMoveIsInList(moves, squares::e5, squares::f4, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::g3, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::e5, squares::h2, pieces::WHITE_BISHOP, pieces::NONE);
}

}

