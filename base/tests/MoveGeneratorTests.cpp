#include "../MoveGenerator.hpp"
#include "../Position.hpp"

#include <gtest/gtest.h>

#include <algorithm>

using namespace chess;
using namespace chess::squares;

namespace
{

class MoveGeneratorTests : public ::testing::Test
{
protected:
	Position	   position;
	MoveGenerator  generator;

	virtual void SetUp()
	{
		constants_initialize();
		position.clear();
	}
};

TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesNothing_WhenThereAreNoKnights)
{
    MoveVector moves;
	generator.GenerateKnightMoves(moves, position, sides::white);
	ASSERT_TRUE(moves.empty());
}

TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesNothing_WhenThereAreNoKnights_OfTheSpecifiedSide)
{
	position.knights[sides::black] = starting::black_knights;
	position.update_sides();

    MoveVector moves;
	generator.GenerateKnightMoves(moves, position, sides::white);

	ASSERT_TRUE(moves.empty());
}

void SetWhiteKnightAt(Position &position, Square square)
{
	position.knights[sides::white] |= squarebits::indexed[square];
	position.update_sides();
	position.squares[square] = pieces::WHITE_KNIGHT;
}

void SetWhiteKingAt(Position &position, Square square)
{
	position.kings[sides::white] |= squarebits::indexed[square];
	position.update_sides();
	position.squares[square] = pieces::WHITE_KING;
}

void SetBlackKingAt(Position &position, Square square)
{
	position.kings[sides::black] |= squarebits::indexed[square];
	position.update_sides();
	position.squares[square] = pieces::BLACK_KING;
}

void SetWhitePawnAt(Position &position, Square square)
{
	position.pawns[sides::white] |= squarebits::indexed[square];
	position.update_sides();
	position.squares[square] = pieces::WHITE_PAWN;
}

void SetBlackPawnAt(Position &position, Square square)
{
	position.pawns[sides::black] |= squarebits::indexed[square];
	position.update_sides();
	position.squares[square] = pieces::BLACK_PAWN;
}

void SetWhiteRookAt(Position &position, Square square)
{
	position.rooks[sides::white] |= squarebits::indexed[square];
	position.update_sides();
	position.squares[square] = pieces::WHITE_ROOK;
}

void SetBlackRookAt(Position &position, Square square)
{
	position.rooks[sides::black] |= squarebits::indexed[square];
	position.update_sides();
	position.squares[square] = pieces::BLACK_ROOK;
}

void SetWhiteBishopAt(Position &position, Square square)
{
	position.bishops[sides::white] |= squarebits::indexed[square];
	position.update_sides();
	position.squares[square] = pieces::WHITE_BISHOP;
}

void SetBlackBishopAt(Position &position, Square square)
{
	position.bishops[sides::black] |= squarebits::indexed[square];
	position.update_sides();
	position.squares[square] = pieces::BLACK_BISHOP;
}

void SetWhiteQueenAt(Position &position, Square square)
{
	position.queens[sides::white] |= squarebits::indexed[square];
	position.update_sides();
	position.squares[square] = pieces::WHITE_QUEEN;
}

void CheckMoveIsInList(const MoveVector &moves, Square source, Square destination, Piece piece, Piece captured, Piece promotion = pieces::NONE, Piece castling = pieces::NONE, Piece enPassant = pieces::NONE)
{
	auto moveIt = std::find_if(moves.begin(), moves.end(), [&](const Move& move) { 
		return move.get_source()	      == source && 
			   move.get_destination()     == destination && 
			   move.get_promotion_piece() == promotion &&
               move.get_castling()        == castling &&
			   move.get_piece()			  == piece &&
			   move.get_captured_piece()  == captured &&
               move.get_en_passant()      == enPassant;
	});
	ASSERT_NE(moves.end(), moveIt);
}

void CheckMovesDestinationVarying(const MoveVector &moves, Square source, Piece piece, Piece captured, const std::vector<Square> &destinations, Piece promotion = pieces::NONE)
{
    for (auto const &destination : destinations)
    {
        CheckMoveIsInList(moves, source, destination, piece, captured, promotion);
    }
}

//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//********************************************************** KNIGHTS ***********************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************

//minimal targets: 2
TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesExpectedMovesForKnightOn_a8_OnOtherwiseClearBoard)
{
	SetWhiteKnightAt(position, a8);

    MoveVector moves;
	generator.GenerateKnightMoves(moves, position, sides::white);

	ASSERT_EQ(2, moves.size());
    CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_KNIGHT, pieces::NONE, { c7, b6 });
}

//maximal targets: 8
TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesExpectedMovesForCentralKnight_OnOtherwiseClearBoard)
{
	SetWhiteKnightAt(position, e5);

    MoveVector moves;
	generator.GenerateKnightMoves(moves, position, sides::white);

	ASSERT_EQ(8, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_KNIGHT, pieces::NONE,
    { f7, f3, d7, d3, g4, g6, c4, c6 });
}

//middling
TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesExpectedMovesForKnightOnG3_OnOtherwiseClearBoard)
{
	SetWhiteKnightAt(position, squares::g3);

    MoveVector moves;
	generator.GenerateKnightMoves(moves, position, sides::white);

	ASSERT_EQ(6, moves.size());
	CheckMovesDestinationVarying(moves, squares::g3, pieces::WHITE_KNIGHT, pieces::NONE,
    { squares::h5, squares::h1, squares::f5, squares::f1, squares::e4, squares::e2 });
}

//middling
TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesExpectedMovesForKnightOn_a8_WithCapture)
{
	SetWhiteKnightAt(position, squares::a8);
	SetBlackPawnAt(position, squares::c7);

    MoveVector moves;
	generator.GenerateKnightMoves(moves, position, sides::white);

	ASSERT_EQ(2, moves.size());
	CheckMoveIsInList(moves, squares::a8, squares::c7, pieces::WHITE_KNIGHT, pieces::BLACK_PAWN);
	CheckMoveIsInList(moves, squares::a8, squares::b6, pieces::WHITE_KNIGHT, pieces::NONE);
}

//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//************************************************************ KINGS ***********************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************

TEST_F(MoveGeneratorTests, TestThat_GenerateKingMoves_GeneratesExpectedMovesForKingOn_a8_OnOtherwiseClearBoard)
{
	SetWhiteKingAt(position, squares::a8);

    MoveVector moves;
	generator.GenerateKingMoves(moves, position, sides::white);

	ASSERT_EQ(3, moves.size());
    CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_KING, pieces::NONE, { b8, b7, a7 });
}

TEST_F(MoveGeneratorTests, TestThat_GenerateKingMoves_GeneratesExpectedMovesForKingOn_e5_OnOtherwiseClearBoard)
{
	SetWhiteKingAt(position, squares::e5);

    MoveVector moves;
	generator.GenerateKingMoves(moves, position, sides::white);

	ASSERT_EQ(8, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_KING, pieces::NONE,
    { e6, e4, f5, d5, f6, d6, d4, f4 });
}

// Castling: white
TEST_F(MoveGeneratorTests, TestThat_GenerateKingMoves_GeneratesExpectedMovesForWhiteKingOn_e1_WithRooksInAppropriatePlaces)
{
	SetWhiteKingAt(position, squares::e1);
    SetWhiteRookAt(position, squares::a1);
    SetWhiteRookAt(position, squares::h1);

    MoveVector moves;
	generator.GenerateKingMoves(moves, position, sides::white);

	ASSERT_EQ(7, moves.size());
	CheckMovesDestinationVarying(moves, squares::e1, pieces::WHITE_KING, pieces::NONE, { d1, d2, e2, f2, f1 });
    

    CheckMoveIsInList(moves, squares::e1, squares::g1, pieces::WHITE_KING, pieces::NONE, pieces::NONE, pieces::WHITE_KING);
    CheckMoveIsInList(moves, squares::e1, squares::c1, pieces::WHITE_KING, pieces::NONE, pieces::NONE, pieces::WHITE_KING);
}

// Castling: black
TEST_F(MoveGeneratorTests, TestThat_GenerateKingMoves_GeneratesExpectedMovesForBlackKingOn_e8_WithRooksInAppropriatePlaces)
{
	SetBlackKingAt(position, squares::e8);
    SetBlackRookAt(position, squares::a8);
    SetBlackRookAt(position, squares::h8);

    MoveVector moves;
	generator.GenerateKingMoves(moves, position, sides::black);

	ASSERT_EQ(7, moves.size());
    CheckMovesDestinationVarying(moves, squares::e8, pieces::BLACK_KING, pieces::NONE, { d8, d7, e7, f7, f8 });

    CheckMoveIsInList(moves, squares::e8, squares::g8, pieces::BLACK_KING, pieces::NONE, pieces::NONE, pieces::BLACK_KING);
    CheckMoveIsInList(moves, squares::e8, squares::c8, pieces::BLACK_KING, pieces::NONE, pieces::NONE, pieces::BLACK_KING);
}

//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//************************************************************ PAWNS ***********************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_OnOtherwiseClearBoard)
{
	SetWhitePawnAt(position, squares::e2);

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::white);

	ASSERT_EQ(2, moves.size());
    CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::NONE, { e3, e4 });
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e3_OnOtherwiseClearBoard)
{
	SetWhitePawnAt(position, squares::e3);

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::white);

	ASSERT_EQ(1, moves.size());
	CheckMoveIsInList(moves, squares::e3, squares::e4, pieces::WHITE_PAWN, pieces::NONE);
}

//Test that jumps are disallowed
TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithABlackPieceOn_e3)
{
	SetWhitePawnAt(position, squares::e2);
	SetBlackPawnAt(position, squares::e3);

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::white);

	ASSERT_EQ(0, moves.size());
}

//Test that jumps are disallowed
TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithAWhitePieceOn_e3)
{
	SetWhitePawnAt(position, squares::e2);
	SetWhiteBishopAt(position, squares::e3);

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::white);

	ASSERT_EQ(0, moves.size());
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithABlackPieceOn_e4)
{
	SetWhitePawnAt(position, squares::e2);
	SetBlackPawnAt(position, squares::e4);

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::white);

	ASSERT_EQ(1, moves.size());
    CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::NONE, { e3 });
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithAWhitePieceOn_e4)
{
	SetWhitePawnAt(position, squares::e2);
	SetWhiteBishopAt(position, squares::e4);

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::white);

	ASSERT_EQ(1, moves.size());
	CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::NONE, { e3 });
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithBlackPieceOn_f3)
{
	SetWhitePawnAt(position, squares::e2);
	SetBlackPawnAt(position, squares::f3);

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::white);

	ASSERT_EQ(3, moves.size());
	CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::NONE, { e3, e4 });
	CheckMoveIsInList(moves, squares::e2, squares::f3, pieces::WHITE_PAWN, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithBlackPieceOn_f3_and_d3)
{
	SetWhitePawnAt(position, squares::e2);
	SetBlackPawnAt(position, squares::f3);
	SetBlackPawnAt(position, squares::d3);

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::white);

	ASSERT_EQ(4, moves.size());
	CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::NONE,		 { e3, e4 });
	CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::BLACK_PAWN, { f3, d3 });
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_c7_OnOtherwiseClearBoard)
{
	SetWhitePawnAt(position, squares::c7);

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::white);

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

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::white);

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

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesEnPassantMovesForWhite)
{
	SetWhitePawnAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f5);
	SetBlackPawnAt(position, squares::d5);

    position.ep_target_square = squares::f6;

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::white);

	ASSERT_EQ(2, moves.size());
    CheckMoveIsInList(moves, squares::e5, squares::e6, pieces::WHITE_PAWN, pieces::NONE,       pieces::NONE, pieces::NONE, pieces::NONE);
    CheckMoveIsInList(moves, squares::e5, squares::f6, pieces::WHITE_PAWN, pieces::BLACK_PAWN, pieces::NONE, pieces::NONE, pieces::WHITE_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesEnPassantMovesForBlack)
{
	SetWhitePawnAt(position, squares::b4);
	SetBlackPawnAt(position, squares::a4);

    position.ep_target_square = squares::b3;

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::black);

	ASSERT_EQ(2, moves.size());
    CheckMoveIsInList(moves, squares::a4, squares::a3, pieces::BLACK_PAWN, pieces::NONE,       pieces::NONE, pieces::NONE, pieces::NONE);
    CheckMoveIsInList(moves, squares::a4, squares::b3, pieces::BLACK_PAWN, pieces::WHITE_PAWN, pieces::NONE, pieces::NONE, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_PawnCapturesAreNotGeneratedAcrossBoardEdges)
{
	SetWhitePawnAt(position, squares::h3);
	SetBlackPawnAt(position, squares::a4);

    MoveVector moves;
	generator.GeneratePawnMoves(moves, position, sides::black);

	ASSERT_EQ(1, moves.size());
    CheckMoveIsInList(moves, squares::a4, squares::a3, pieces::BLACK_PAWN, pieces::NONE);
}

//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//************************************************************ ROOKS ***********************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_a8_OnOtherwiseClearBoard)
{
	SetWhiteRookAt(position, squares::a8);

    MoveVector moves;
	generator.GenerateRookMoves(moves, position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_ROOK, pieces::NONE,
    { b8, c8, d8, e8, f8, g8, h8, a7, a6, a5, a4, a3, a2, a1 });
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_e5_OnOtherwiseClearBoard)
{
	SetWhiteRookAt(position, squares::e5);

    MoveVector moves;
	generator.GenerateRookMoves(moves, position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_ROOK, pieces::NONE,
    { e6, e7, e8, e4, e3, e2, e1, d5, c5, b5, a5, f5, g5, h5 });
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_h1_OnOtherwiseClearBoard)
{
	SetWhiteRookAt(position, squares::h1);

    MoveVector moves;
	generator.GenerateRookMoves(moves, position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMovesDestinationVarying(moves, squares::h1, pieces::WHITE_ROOK, pieces::NONE,
    { h2, h3, h4, h5, h6, h7, h8, g1, f1, e1, d1, c1, b1, a1 });
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_a8_WithBlackPieceOn_a3)
{
	SetWhiteRookAt(position, squares::a8);
	SetBlackPawnAt(position, squares::a3);

    MoveVector moves;
	generator.GenerateRookMoves(moves, position, sides::white);

	ASSERT_EQ(12, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_ROOK, pieces::NONE,
    { b8, c8, d8, e8, f8, g8, h8, a7, a6, a5, a4 });

	CheckMoveIsInList(moves, squares::a8, squares::a3, pieces::WHITE_ROOK, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_a8_WithBlackPieceOn_a1)
{
	SetWhiteRookAt(position, squares::a8);
	SetBlackBishopAt(position, squares::a1);

    MoveVector moves;
	generator.GenerateRookMoves(moves, position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_ROOK, pieces::NONE,
    { b8, c8, d8, e8, f8, g8, h8, a7, a6, a5, a4, a3, a2 });

	CheckMoveIsInList(moves, squares::a8, squares::a1, pieces::WHITE_ROOK, pieces::BLACK_BISHOP);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_a8_WithWhitePieceOn_a3)
{
	SetWhiteRookAt(position, squares::a8);
	SetWhiteKingAt(position, squares::a3);

    MoveVector moves;
	generator.GenerateRookMoves(moves, position, sides::white);

	ASSERT_EQ(11, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_ROOK, pieces::NONE,
	{ b8, c8, d8, e8, f8, g8, h8, a7, a6, a5, a4 });
}

//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//********************************************************** BISHOPS ***********************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_a1_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::a1);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(7, moves.size());
	CheckMovesDestinationVarying(moves, squares::a1, pieces::WHITE_BISHOP, pieces::NONE,
    { b2, c3, d4, e5, f6, g7, h8 });
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_h8_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::h8);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(7, moves.size());
	CheckMovesDestinationVarying(moves, squares::h8, pieces::WHITE_BISHOP, pieces::NONE,
    { b2, c3, d4, e5, f6, g7, a1 });
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_a8_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::a8);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(7, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_BISHOP, pieces::NONE,
    { b7, c6, d5, e4, f3, g2, h1 });
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_h1_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::h1);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(7, moves.size());
	CheckMovesDestinationVarying(moves, squares::h1, pieces::WHITE_BISHOP, pieces::NONE,
    { g2, f3, e4, d5, c6, b7, a8 });
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_b2_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::b2);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(9, moves.size());
	CheckMovesDestinationVarying(moves, squares::b2, pieces::WHITE_BISHOP, pieces::NONE,
    { c3, d4, e5, f6, g7, h8, a1, c1, a3 });
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_g7_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::g7);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(9, moves.size());
	CheckMovesDestinationVarying(moves, squares::g7, pieces::WHITE_BISHOP, pieces::NONE,
    { c3, d4, e5, f6, b2, h8, a1, h6, f8 });
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_b7_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::b7);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(9, moves.size());
	CheckMovesDestinationVarying(moves, squares::b7, pieces::WHITE_BISHOP, pieces::NONE,
    { a8, c6, d5, e4, f3, g2, h1, a6, c8 });
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_g2_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::g2);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(9, moves.size());
	CheckMovesDestinationVarying(moves, squares::g2, pieces::WHITE_BISHOP, pieces::NONE,
    { a8, b7, c6, d5, e4, f3, h1, h3, f1 });
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::e5);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(13, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
    { f6, g7, h8, d4, c3, b2, a1, d6, c7, b8, f4, g3, h2 });
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e4_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::e4);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(13, moves.size());
	CheckMovesDestinationVarying(moves, squares::e4, pieces::WHITE_BISHOP, pieces::NONE,
    { f5, g6, h7, d3, c2, b1, d5, c6, b7, a8, f3, g2, h1 });
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_d4_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::d4);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(13, moves.size());
	CheckMovesDestinationVarying(moves, squares::d4, pieces::WHITE_BISHOP, pieces::NONE,
    { e5, f6, g7, h8, c3, b2, a1, c5, b6, a7, e3, f2, g1 });
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_d5_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::d5);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(13, moves.size());
	CheckMovesDestinationVarying(moves, squares::d5, pieces::WHITE_BISHOP, pieces::NONE,
    { e6, f7, g8, c4, b3, a2, c6, b7, a8, e4, f3, g2, h1 });
}

//Down left capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_b2)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::b2);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(12, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
    { f6, g7, h8, d4, c3, d6, c7, b8, f4, g3, h2 });

	CheckMoveIsInList(moves, squares::e5, squares::b2, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

//Up left capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_c7)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::c7);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(12, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
    { f6, g7, h8, d4, c3, b2, a1, d6, f4, g3, h2 });

	CheckMoveIsInList(moves, squares::e5, squares::c7, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

//Down right capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_f4)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f4);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(11, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
    { f6, g7, h8, d4, c3, b2, a1, d6, c7, b8 });

	CheckMoveIsInList(moves, squares::e5, squares::f4, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

//Up right capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_f6)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f6);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(11, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
    { f4, g3, h2, d4, c3, b2, a1, d6, c7, b8 });

	CheckMoveIsInList(moves, squares::e5, squares::f6, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

//Up right capture from corner
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_a1_WithBlackPieceOn_c3)
{
	SetWhiteBishopAt(position, squares::a1);
	SetBlackPawnAt(position, squares::c3);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(2, moves.size());
	CheckMoveIsInList(moves, squares::a1, squares::b2, pieces::WHITE_BISHOP, pieces::NONE);
	CheckMoveIsInList(moves, squares::a1, squares::c3, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_c4_WithBlackPieceOn_b3)
{
	SetWhiteBishopAt(position, squares::c4);
	SetBlackPawnAt(position, squares::b3);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(10, moves.size());
	CheckMovesDestinationVarying(moves, squares::c4, pieces::WHITE_BISHOP, pieces::NONE,
    { d5, e6, f7, g8, b5, a6, d3, e2, f1 });

	CheckMoveIsInList(moves, squares::c4, squares::b3, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

//No clash, so same as clear board.
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_f3)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f3);

    MoveVector moves;
	generator.GenerateBishopMoves(moves, position, sides::white);

	ASSERT_EQ(13, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
    { f6, g7, h8, d4, c3, b2, a1, d6, c7, b8, f4, g3, h2 });
}

//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//********************************************************** QUEENS ************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_a1_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::a1);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(21, moves.size());
	CheckMovesDestinationVarying(moves, squares::a1, pieces::WHITE_QUEEN, pieces::NONE,
    { b2, c3, d4, e5, f6, g7, h8, a2, a3, a4, a5, a6, a7, a8, b1, c1, d1, e1, f1, g1, h1 });
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_h8_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::h8);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(21, moves.size());
	CheckMovesDestinationVarying(moves, squares::h8, pieces::WHITE_QUEEN, pieces::NONE,
    { b2, c3, d4, e5, f6, g7, a1, a8, b8, c8, d8, e8, f8, g8, h7, h6, h5, h4, h3, h2, h1 });
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_a8_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::a8);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(21, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_QUEEN, pieces::NONE,
    { b7, c6, d5, e4, f3, g2, h1, a2, a3, a4, a5, a6, a7, a1, h8, b8, c8, d8, e8, f8, g8 });
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_h1_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::h1);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(21, moves.size());
	CheckMovesDestinationVarying(moves, squares::h1, pieces::WHITE_QUEEN, pieces::NONE,
    { b7, c6, d5, e4, f3, g2, a8, h7, h6, h5, h4, h3, h2, h8, b1, c1, d1, e1, f1, g1, a1 });
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_b2_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::b2);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(23, moves.size());
	CheckMovesDestinationVarying(moves, squares::b2, pieces::WHITE_QUEEN, pieces::NONE,
    { a1, c3, d4, e5, f6, g7, h8, c1, a3, b1, b3, b4, b5, b6, b7, b8, a2, c2, d2, e2, f2, g2, h2 });
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_g7_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::g7);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(23, moves.size());
	CheckMovesDestinationVarying(moves, squares::g7, pieces::WHITE_QUEEN, pieces::NONE,
    { c3, d4, e5, f6, b2, h8, a1, h6, f8, g1, g2, g3, g4, g5, g6, g8, a7, b7, c7, d7, e7, f7, h7 });
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_b7_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::b7);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(23, moves.size());
	CheckMovesDestinationVarying(moves, squares::b7, pieces::WHITE_QUEEN, pieces::NONE,
    { a8, c6, d5, e4, f3, g2, h1, a6, c8, b1, b3, b4, b5, b6, b2, b8, a7, g7, c7, d7, e7, f7, h7 });
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_g2_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::g2);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(23, moves.size());
	CheckMovesDestinationVarying(moves, squares::g2, pieces::WHITE_QUEEN, pieces::NONE,
    { a8, b7, c6, d5, e4, f3, h1, h3, f1, g1, g7, g3, g4, g5, g6, g8, a2, c2, d2, e2, f2, b2, h2 });
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::e5);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(27, moves.size());
    CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
    { f6, g7, h8, d4, c3, b2, a1, d6, c7, b8, f4, g3, h2, e6, e7, e8, e4, e3, e2, e1, a5, b5, c5, d5, f5, g5, h5 });
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e4_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::e4);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(27, moves.size());
	CheckMovesDestinationVarying(moves, squares::e4, pieces::WHITE_QUEEN, pieces::NONE,
    { f5, g6, h7, d3, c2, b1, d5, c6, b7, a8, f3, g2, h1, e6, e7, e8, e5, e3, e2, e1, a4, b4, c4, d4, f4, g2, h4 });
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_d4_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::d4);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(27, moves.size());
	CheckMovesDestinationVarying(moves, squares::d4, pieces::WHITE_QUEEN, pieces::NONE,
    { e5, f6, g7, h8, c3, b2, a1, c5, b6, a7, e3, f2, g1, d6, d7, d8, d5, d3, d2, d1, a4, b4, c4, e4, f4, g4, h4 });
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_d5_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::d5);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(27, moves.size());
	CheckMovesDestinationVarying(moves, squares::d5, pieces::WHITE_QUEEN, pieces::NONE,
    { e6, f7, g8, c4, b3, a2, c6, b7, a8, e4, f3, g2, h1, d6, d7, d8, d4, d3, d2, d1, a5, b5, c5, e5, f5, g5, h5 });
}

//Down left capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_WithBlackPieceOn_b2)
{
	SetWhiteQueenAt(position, squares::e5);
	SetBlackPawnAt(position, squares::b2);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(26, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
    { f6, g7, h8, d4, c3, d6, c7, b8, f4, g3, h2, e6, e7, e8, e4, e3, e2, e1, a5, b5, c5, d5, f5, g5, h5 });

	CheckMoveIsInList(moves, squares::e5, squares::b2, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

//Up left capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_WithBlackPieceOn_c7)
{
	SetWhiteQueenAt(position, squares::e5);
	SetBlackPawnAt(position, squares::c7);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(26, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
    { f6, g7, h8, d4, c3, b2, a1, d6, f4, g3, h2, e6, e7, e8, e4, e3, e2, e1, a5, b5, c5, d5, f5, g5, h5 });

	CheckMoveIsInList(moves, squares::e5, squares::c7, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

//Down right capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_WithBlackPieceOn_f4)
{
	SetWhiteQueenAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f4);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(25, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
    { f6, g7, h8, d4, c3, b2, a1, d6, c7, b8, e6, e7, e8, e4, e3, e2, e1, a5, b5, c5, d5, f5, g5, h5 });

	CheckMoveIsInList(moves, squares::e5, squares::f4, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

//Up right capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_WithBlackPieceOn_f6)
{
	SetWhiteQueenAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f6);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(25, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
    { f4, g3, h2, d4, c3, b2, a1, d6, c7, b8, e6, e7, e8, e4, e3, e2, e1, a5, b5, c5, d5, f5, g5, h5 });

	CheckMoveIsInList(moves, squares::e5, squares::f6, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

//Up right capture from corner
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_a1_WithBlackPieceOn_c3)
{
	SetWhiteQueenAt(position, squares::a1);
	SetBlackPawnAt(position, squares::c3);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(16, moves.size());
    CheckMovesDestinationVarying(moves, squares::a1, pieces::WHITE_QUEEN, pieces::NONE,
    { a2, a3, a4, a5, a6, a7, a8, b1, c1, d1, e1, f1, g1, h1, b2 });

	CheckMoveIsInList(moves, squares::a1, squares::b2, pieces::WHITE_QUEEN, pieces::NONE);
	CheckMoveIsInList(moves, squares::a1, squares::c3, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_c4_WithBlackPieceOn_b3)
{
	SetWhiteQueenAt(position, squares::c4);
	SetBlackPawnAt(position, squares::b3);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(24, moves.size());
	CheckMovesDestinationVarying(moves, squares::c4, pieces::WHITE_QUEEN, pieces::NONE,
    { d5, e6, f7, g8, b5, a6, d3, e2, f1, c3, c2, c1, c5, c6, c7, c8, a4, b4, d4, e4, f4, g4, h4 });

	CheckMoveIsInList(moves, squares::c4, squares::b3, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

//No clash, so same as clear board.
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_WithBlackPieceOn_f3)
{
	SetWhiteQueenAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f3);

    MoveVector moves;
	generator.GenerateQueenMoves(moves, position, sides::white);

	ASSERT_EQ(27, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
    { f6, g7, h8, d4, c3, b2, a1, d6, c7, b8, f4, g3, h2, e6, e7, e8, e4, e3, e2, e1, a5, b5, c5, d5, f5, g5, h5 });
}

TEST_F(MoveGeneratorTests, TestThat_GenerateAllMoves_WorksForStartingPosition)
{
    position.setup_starting_position();

	auto moves = generator.GenerateAllMoves(position, sides::white);

	ASSERT_EQ(20, moves.size());
    CheckMovesDestinationVarying(moves, squares::a2, pieces::WHITE_PAWN, pieces::NONE, { a3, a4 });
    CheckMovesDestinationVarying(moves, squares::b2, pieces::WHITE_PAWN, pieces::NONE, { b3, b4 });
    CheckMovesDestinationVarying(moves, squares::c2, pieces::WHITE_PAWN, pieces::NONE, { c3, c4 });
    CheckMovesDestinationVarying(moves, squares::d2, pieces::WHITE_PAWN, pieces::NONE, { d3, d4 });
    CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::NONE, { e3, e4 });
    CheckMovesDestinationVarying(moves, squares::f2, pieces::WHITE_PAWN, pieces::NONE, { f3, f4 });
    CheckMovesDestinationVarying(moves, squares::g2, pieces::WHITE_PAWN, pieces::NONE, { g3, g4 });
    CheckMovesDestinationVarying(moves, squares::h2, pieces::WHITE_PAWN, pieces::NONE, { h3, h4 });

    CheckMovesDestinationVarying(moves, squares::b1, pieces::WHITE_KNIGHT, pieces::NONE, { a3, c3 });
    CheckMovesDestinationVarying(moves, squares::g1, pieces::WHITE_KNIGHT, pieces::NONE, { f3, h3 });
}


}

