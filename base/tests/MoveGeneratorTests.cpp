#include <gtest/gtest.h>
#include "../MoveGenerator.hpp"

#include <boost/range/algorithm.hpp>
#include <boost/assign.hpp>

using namespace chess;
namespace ba = boost::assign;

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

void SetWhiteKnightAt(Position &position, Square square)
{
	position.knights[sides::white] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::WHITE_KNIGHT;
}

void SetWhiteKingAt(Position &position, Square square)
{
	position.kings[sides::white] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::WHITE_KING;
}

void SetBlackKingAt(Position &position, Square square)
{
	position.kings[sides::black] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::BLACK_KING;
}

void SetWhitePawnAt(Position &position, Square square)
{
	position.pawns[sides::white] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::WHITE_PAWN;
}

void SetBlackPawnAt(Position &position, Square square)
{
	position.pawns[sides::black] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::BLACK_PAWN;
}

void SetWhiteRookAt(Position &position, Square square)
{
	position.rooks[sides::white] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::WHITE_ROOK;
}

void SetBlackRookAt(Position &position, Square square)
{
	position.rooks[sides::black] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::BLACK_ROOK;
}

void SetWhiteBishopAt(Position &position, Square square)
{
	position.bishops[sides::white] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::WHITE_BISHOP;
}

void SetBlackBishopAt(Position &position, Square square)
{
	position.bishops[sides::black] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::BLACK_BISHOP;
}

void SetWhiteQueenAt(Position &position, Square square)
{
	position.queens[sides::white] |= squarebits::indexed[square];
	position.UpdateSides();
	position.squares[square] = pieces::WHITE_QUEEN;
}

void CheckMoveIsInList(const MoveVector &moves, Square source, Square destination, Piece piece, Piece captured, Piece promotion = pieces::NONE, Piece castling = pieces::NONE)
{
	auto moveIt = boost::find_if(moves, [&](const Move& move) { 
		return move.GetSource()			== source && 
			   move.GetDestination()    == destination && 
			   move.GetPromotionPiece() == promotion &&
               move.GetCastling()       == castling &&
			   move.GetPiece()			== piece &&
			   move.GetCapturedPiece()  == captured;
	});
	ASSERT_NE(moves.end(), moveIt);
}

void CheckMovesDestinationVarying(const MoveVector &moves, Square source, Piece piece, Piece captured, const std::vector<Square> &destinations, Piece promotion = pieces::NONE)
{
	boost::for_each(destinations, [&](Square destination) { CheckMoveIsInList(moves, source, destination, piece, captured, promotion); } );
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
	SetWhiteKnightAt(position, squares::a8);

	auto moves = generator.GenerateKnightMoves(position, sides::white);

	ASSERT_EQ(2, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_KNIGHT, pieces::NONE, ba::list_of(squares::c7)(squares::b6));
}

//maximal targets: 8
TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesExpectedMovesForCentralKnight_OnOtherwiseClearBoard)
{
	SetWhiteKnightAt(position, squares::e5);

	auto moves = generator.GenerateKnightMoves(position, sides::white);

	ASSERT_EQ(8, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_KNIGHT, pieces::NONE,
		ba::list_of(squares::f7)(squares::f3)(squares::d7)(squares::d3)(squares::g4)(squares::g6)(squares::c4)(squares::c6));
}

//middling
TEST_F(MoveGeneratorTests, TestThat_GenerateKnightMoves_GeneratesExpectedMovesForKnightOnG3_OnOtherwiseClearBoard)
{
	SetWhiteKnightAt(position, squares::g3);

	auto moves = generator.GenerateKnightMoves(position, sides::white);

	ASSERT_EQ(6, moves.size());
	CheckMovesDestinationVarying(moves, squares::g3, pieces::WHITE_KNIGHT, pieces::NONE,
		ba::list_of(squares::h5)(squares::h1)(squares::f5)(squares::f1)(squares::e4)(squares::e2));
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

	auto moves = generator.GenerateKingMoves(position, sides::white);

	ASSERT_EQ(3, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_KING, pieces::NONE,
		ba::list_of(squares::b8)(squares::b7)(squares::a7));
}

TEST_F(MoveGeneratorTests, TestThat_GenerateKingMoves_GeneratesExpectedMovesForKingOn_e5_OnOtherwiseClearBoard)
{
	SetWhiteKingAt(position, squares::e5);

	auto moves = generator.GenerateKingMoves(position, sides::white);

	ASSERT_EQ(8, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_KING, pieces::NONE,
		ba::list_of(squares::e6)(squares::e4)(squares::f5)(squares::d5)(squares::f6)(squares::d6)(squares::d4)(squares::f4));
}

// Castling: white
TEST_F(MoveGeneratorTests, TestThat_GenerateKingMoves_GeneratesExpectedMovesForWhiteKingOn_e1_WithRooksInAppropriatePlaces)
{
	SetWhiteKingAt(position, squares::e1);
    SetWhiteRookAt(position, squares::a1);
    SetWhiteRookAt(position, squares::h1);

	auto moves = generator.GenerateKingMoves(position, sides::white);

	ASSERT_EQ(7, moves.size());
	CheckMovesDestinationVarying(moves, squares::e1, pieces::WHITE_KING, pieces::NONE,
		ba::list_of(squares::d1)(squares::d2)(squares::e2)(squares::f2)(squares::f1)
    );
    CheckMoveIsInList(moves, squares::e1, squares::g1, pieces::WHITE_KING, pieces::NONE, pieces::NONE, pieces::WHITE_KING);
    CheckMoveIsInList(moves, squares::e1, squares::c1, pieces::WHITE_KING, pieces::NONE, pieces::NONE, pieces::WHITE_KING);
}

// Castling: black
TEST_F(MoveGeneratorTests, TestThat_GenerateKingMoves_GeneratesExpectedMovesForBlackKingOn_e8_WithRooksInAppropriatePlaces)
{
	SetBlackKingAt(position, squares::e8);
    SetBlackRookAt(position, squares::a8);
    SetBlackRookAt(position, squares::h8);

	auto moves = generator.GenerateKingMoves(position, sides::black);

	ASSERT_EQ(7, moves.size());
	CheckMovesDestinationVarying(moves, squares::e8, pieces::BLACK_KING, pieces::NONE,
		ba::list_of(squares::d8)(squares::d7)(squares::e7)(squares::f7)(squares::f8)
    );
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

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(2, moves.size());
	CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::NONE, ba::list_of(squares::e3)(squares::e4));
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e3_OnOtherwiseClearBoard)
{
	SetWhitePawnAt(position, squares::e3);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(1, moves.size());
	CheckMoveIsInList(moves, squares::e3, squares::e4, pieces::WHITE_PAWN, pieces::NONE);
}

//Test that jumps are disallowed
TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithABlackPieceOn_e3)
{
	SetWhitePawnAt(position, squares::e2);
	SetBlackPawnAt(position, squares::e3);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(0, moves.size());
}

//Test that jumps are disallowed
TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithAWhitePieceOn_e3)
{
	SetWhitePawnAt(position, squares::e2);
	SetWhiteBishopAt(position, squares::e3);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(0, moves.size());
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithABlackPieceOn_e4)
{
	SetWhitePawnAt(position, squares::e2);
	SetBlackPawnAt(position, squares::e4);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(1, moves.size());
	CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::NONE, ba::list_of(squares::e3));
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithAWhitePieceOn_e4)
{
	SetWhitePawnAt(position, squares::e2);
	SetWhiteBishopAt(position, squares::e4);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(1, moves.size());
	CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::NONE, ba::list_of(squares::e3));
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithBlackPieceOn_f3)
{
	SetWhitePawnAt(position, squares::e2);
	SetBlackPawnAt(position, squares::f3);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(3, moves.size());
	CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::NONE, ba::list_of(squares::e3)(squares::e4));
	CheckMoveIsInList(moves, squares::e2, squares::f3, pieces::WHITE_PAWN, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GeneratePawnMoves_GeneratesExpectedMovesForPawnOn_e2_WithBlackPieceOn_f3_and_d3)
{
	SetWhitePawnAt(position, squares::e2);
	SetBlackPawnAt(position, squares::f3);
	SetBlackPawnAt(position, squares::d3);

	auto moves = generator.GeneratePawnMoves(position, sides::white);

	ASSERT_EQ(4, moves.size());
	CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::NONE,		 ba::list_of(squares::e3)(squares::e4));
	CheckMovesDestinationVarying(moves, squares::e2, pieces::WHITE_PAWN, pieces::BLACK_PAWN, ba::list_of(squares::f3)(squares::d3));
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

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_ROOK, pieces::NONE,
		ba::list_of(squares::b8)(squares::c8)(squares::d8)(squares::e8)(squares::f8)(squares::g8)(squares::h8)
				   (squares::a7)(squares::a6)(squares::a5)(squares::a4)(squares::a3)(squares::a2)(squares::a1));
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_e5_OnOtherwiseClearBoard)
{
	SetWhiteRookAt(position, squares::e5);

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_ROOK, pieces::NONE,
		ba::list_of(squares::e6)(squares::e7)(squares::e8)(squares::e4)(squares::e3)(squares::e2)(squares::e1)
				   (squares::d5)(squares::c5)(squares::b5)(squares::a5)(squares::f5)(squares::g5)(squares::h5));
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_h1_OnOtherwiseClearBoard)
{
	SetWhiteRookAt(position, squares::h1);

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMovesDestinationVarying(moves, squares::h1, pieces::WHITE_ROOK, pieces::NONE,
		ba::list_of(squares::h2)(squares::h3)(squares::h4)(squares::h5)(squares::h6)(squares::h7)(squares::h8)
				   (squares::g1)(squares::f1)(squares::e1)(squares::d1)(squares::c1)(squares::b1)(squares::a1));
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_a8_WithBlackPieceOn_a3)
{
	SetWhiteRookAt(position, squares::a8);
	SetBlackPawnAt(position, squares::a3);

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(12, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_ROOK, pieces::NONE,
		ba::list_of(squares::b8)(squares::c8)(squares::d8)(squares::e8)(squares::f8)(squares::g8)(squares::h8)
				   (squares::a7)(squares::a6)(squares::a5)(squares::a4));
	CheckMoveIsInList(moves, squares::a8, squares::a3, pieces::WHITE_ROOK, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_a8_WithBlackPieceOn_a1)
{
	SetWhiteRookAt(position, squares::a8);
	SetBlackBishopAt(position, squares::a1);

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(14, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_ROOK, pieces::NONE,
		ba::list_of(squares::b8)(squares::c8)(squares::d8)(squares::e8)(squares::f8)(squares::g8)(squares::h8)
				   (squares::a7)(squares::a6)(squares::a5)(squares::a4)(squares::a3)(squares::a2));
	CheckMoveIsInList(moves, squares::a8, squares::a1, pieces::WHITE_ROOK, pieces::BLACK_BISHOP);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateRookMoves_GeneratesExpectedMovesForRookOn_a8_WithWhitePieceOn_a3)
{
	SetWhiteRookAt(position, squares::a8);
	SetWhiteKingAt(position, squares::a3);

	auto moves = generator.GenerateRookMoves(position, sides::white);

	ASSERT_EQ(11, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_ROOK, pieces::NONE,
		ba::list_of(squares::b8)(squares::c8)(squares::d8)(squares::e8)(squares::f8)(squares::g8)(squares::h8)
				   (squares::a7)(squares::a6)(squares::a5)(squares::a4));
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

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(7, moves.size());
	CheckMovesDestinationVarying(moves, squares::a1, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::b2)(squares::c3)(squares::d4)(squares::e5)(squares::f6)(squares::g7)(squares::h8));
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_h8_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::h8);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(7, moves.size());
	CheckMovesDestinationVarying(moves, squares::h8, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::b2)(squares::c3)(squares::d4)(squares::e5)(squares::f6)(squares::g7)(squares::a1));
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_a8_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::a8);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(7, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::b7)(squares::c6)(squares::d5)(squares::e4)(squares::f3)(squares::g2)(squares::h1));
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_h1_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::h1);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(7, moves.size());
	CheckMovesDestinationVarying(moves, squares::h1, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::b7)(squares::c6)(squares::d5)(squares::e4)(squares::f3)(squares::g2)(squares::a8));
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_b2_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::b2);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(9, moves.size());
	CheckMovesDestinationVarying(moves, squares::b2, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::c3)(squares::d4)(squares::e5)(squares::f6)(squares::g7)(squares::h8)(squares::a1)(squares::c1)(squares::a3));
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_g7_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::g7);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(9, moves.size());
	CheckMovesDestinationVarying(moves, squares::g7, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::c3)(squares::d4)(squares::e5)(squares::f6)(squares::b2)(squares::h8)(squares::a1)(squares::h6)(squares::f8));
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_b7_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::b7);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(9, moves.size());
	CheckMovesDestinationVarying(moves, squares::b7, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::a8)(squares::c6)(squares::d5)(squares::e4)(squares::f3)(squares::g2)(squares::h1)(squares::a6)(squares::c8));
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_g2_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::g2);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(9, moves.size());
	CheckMovesDestinationVarying(moves, squares::g2, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::a8)(squares::b7)(squares::c6)(squares::d5)(squares::e4)(squares::f3)(squares::h1)(squares::h3)(squares::f1));
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::e5);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(13, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::f6)(squares::g7)(squares::h8)(squares::d4)(squares::c3)(squares::b2)(squares::a1)
				   (squares::d6)(squares::c7)(squares::b8)(squares::f4)(squares::g3)(squares::h2));
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e4_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::e4);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(13, moves.size());
	CheckMovesDestinationVarying(moves, squares::e4, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::f5)(squares::g6)(squares::h7)(squares::d3)(squares::c2)(squares::b1)
				   (squares::d5)(squares::c6)(squares::b7)(squares::a8)(squares::f3)(squares::g2)(squares::h1));
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_d4_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::d4);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(13, moves.size());
	CheckMovesDestinationVarying(moves, squares::d4, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::e5)(squares::f6)(squares::g7)(squares::h8)(squares::c3)(squares::b2)(squares::a1)
				   (squares::c5)(squares::b6)(squares::a7)(squares::e3)(squares::f2)(squares::g1));
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_d5_OnOtherwiseClearBoard)
{
	SetWhiteBishopAt(position, squares::d5);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(13, moves.size());
	CheckMovesDestinationVarying(moves, squares::d5, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::e6)(squares::f7)(squares::g8)(squares::c4)(squares::b3)(squares::a2)
				   (squares::c6)(squares::b7)(squares::a8)(squares::e4)(squares::f3)(squares::g2)(squares::h1));
}

//Down left capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_b2)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::b2);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(12, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::f6)(squares::g7)(squares::h8)(squares::d4)(squares::c3)
				   (squares::d6)(squares::c7)(squares::b8)(squares::f4)(squares::g3)(squares::h2));
	CheckMoveIsInList(moves, squares::e5, squares::b2, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

//Up left capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_c7)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::c7);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(12, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::f6)(squares::g7)(squares::h8)(squares::d4)(squares::c3)(squares::b2)(squares::a1)
				   (squares::d6)(squares::f4)(squares::g3)(squares::h2));
	CheckMoveIsInList(moves, squares::e5, squares::c7, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

//Down right capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_f4)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f4);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(11, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::f6)(squares::g7)(squares::h8)(squares::d4)(squares::c3)(squares::b2)(squares::a1)
				   (squares::d6)(squares::c7)(squares::b8));
	CheckMoveIsInList(moves, squares::e5, squares::f4, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

//Up right capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_f6)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f6);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(11, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::f4)(squares::g3)(squares::h2)(squares::d4)(squares::c3)(squares::b2)(squares::a1)
				   (squares::d6)(squares::c7)(squares::b8));
	CheckMoveIsInList(moves, squares::e5, squares::f6, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

//Up right capture from corner
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
	CheckMovesDestinationVarying(moves, squares::c4, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::d5)(squares::e6)(squares::f7)(squares::g8)(squares::b5)(squares::a6)(squares::d3)
				   (squares::e2)(squares::f1));
	CheckMoveIsInList(moves, squares::c4, squares::b3, pieces::WHITE_BISHOP, pieces::BLACK_PAWN);
}

//No clash, so same as clear board.
TEST_F(MoveGeneratorTests, TestThat_GenerateBishopMoves_GeneratesExpectedMovesForBishopOn_e5_WithBlackPieceOn_f3)
{
	SetWhiteBishopAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f3);

	auto moves = generator.GenerateBishopMoves(position, sides::white);

	ASSERT_EQ(13, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_BISHOP, pieces::NONE,
		ba::list_of(squares::f6)(squares::g7)(squares::h8)(squares::d4)(squares::c3)(squares::b2)(squares::a1)
				   (squares::d6)(squares::c7)(squares::b8)(squares::f4)(squares::g3)(squares::h2));
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

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(21, moves.size());
	CheckMovesDestinationVarying(moves, squares::a1, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::b2)(squares::c3)(squares::d4)(squares::e5)(squares::f6)(squares::g7)(squares::h8)
				   (squares::a2)(squares::a3)(squares::a4)(squares::a5)(squares::a6)(squares::a7)(squares::a8)
				   (squares::b1)(squares::c1)(squares::d1)(squares::e1)(squares::f1)(squares::g1)(squares::h1)
	);
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_h8_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::h8);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(21, moves.size());
	CheckMovesDestinationVarying(moves, squares::h8, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::b2)(squares::c3)(squares::d4)(squares::e5)(squares::f6)(squares::g7)(squares::a1)
				   (squares::a8)(squares::b8)(squares::c8)(squares::d8)(squares::e8)(squares::f8)(squares::g8)
				   (squares::h7)(squares::h6)(squares::h5)(squares::h4)(squares::h3)(squares::h2)(squares::h1)
	);
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_a8_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::a8);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(21, moves.size());
	CheckMovesDestinationVarying(moves, squares::a8, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::b7)(squares::c6)(squares::d5)(squares::e4)(squares::f3)(squares::g2)(squares::h1)
		           (squares::a2)(squares::a3)(squares::a4)(squares::a5)(squares::a6)(squares::a7)(squares::a1)
				   (squares::h8)(squares::b8)(squares::c8)(squares::d8)(squares::e8)(squares::f8)(squares::g8)
	);
}

//Corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_h1_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::h1);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(21, moves.size());
	CheckMovesDestinationVarying(moves, squares::h1, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::b7)(squares::c6)(squares::d5)(squares::e4)(squares::f3)(squares::g2)(squares::a8)
				   (squares::h7)(squares::h6)(squares::h5)(squares::h4)(squares::h3)(squares::h2)(squares::h8)
				   (squares::b1)(squares::c1)(squares::d1)(squares::e1)(squares::f1)(squares::g1)(squares::a1)
	);
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_b2_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::b2);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(23, moves.size());
	CheckMovesDestinationVarying(moves, squares::b2, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::c3)(squares::d4)(squares::e5)(squares::f6)(squares::g7)(squares::h8)(squares::a1)(squares::c1)(squares::a3)
				   (squares::b1)(squares::b3)(squares::b4)(squares::b5)(squares::b6)(squares::b7)(squares::b8)
				   (squares::a2)(squares::c2)(squares::d2)(squares::e2)(squares::f2)(squares::g2)(squares::h2)
	);
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_g7_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::g7);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(23, moves.size());
	CheckMovesDestinationVarying(moves, squares::g7, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::c3)(squares::d4)(squares::e5)(squares::f6)(squares::b2)(squares::h8)(squares::a1)(squares::h6)(squares::f8)
		           (squares::g1)(squares::g2)(squares::g3)(squares::g4)(squares::g5)(squares::g6)(squares::g8)
		           (squares::a7)(squares::b7)(squares::c7)(squares::d7)(squares::e7)(squares::f7)(squares::h7)
	);
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_b7_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::b7);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(23, moves.size());
	CheckMovesDestinationVarying(moves, squares::b7, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::a8)(squares::c6)(squares::d5)(squares::e4)(squares::f3)(squares::g2)(squares::h1)(squares::a6)(squares::c8)
        		   (squares::b1)(squares::b3)(squares::b4)(squares::b5)(squares::b6)(squares::b2)(squares::b8)
		           (squares::a7)(squares::g7)(squares::c7)(squares::d7)(squares::e7)(squares::f7)(squares::h7)
	);
}

//Next to corners
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_g2_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::g2);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(23, moves.size());
	CheckMovesDestinationVarying(moves, squares::g2, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::a8)(squares::b7)(squares::c6)(squares::d5)(squares::e4)(squares::f3)(squares::h1)(squares::h3)(squares::f1)
                   (squares::g1)(squares::g7)(squares::g3)(squares::g4)(squares::g5)(squares::g6)(squares::g8)
		           (squares::a2)(squares::c2)(squares::d2)(squares::e2)(squares::f2)(squares::b2)(squares::h2)
	);
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::e5);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(27, moves.size());
    CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
        ba::list_of(squares::f6)(squares::g7)(squares::h8)(squares::d4)(squares::c3)(squares::b2)(squares::a1)
                   (squares::d6)(squares::c7)(squares::b8)(squares::f4)(squares::g3)(squares::h2)
                   (squares::e6)(squares::e7)(squares::e8)(squares::e4)(squares::e3)(squares::e2)(squares::e1)
                   (squares::a5)(squares::b5)(squares::c5)(squares::d5)(squares::f5)(squares::g5)(squares::h5)
    );
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e4_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::e4);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(27, moves.size());
	CheckMovesDestinationVarying(moves, squares::e4, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::f5)(squares::g6)(squares::h7)(squares::d3)(squares::c2)(squares::b1)
		           (squares::d5)(squares::c6)(squares::b7)(squares::a8)(squares::f3)(squares::g2)(squares::h1)
                   (squares::e6)(squares::e7)(squares::e8)(squares::e5)(squares::e3)(squares::e2)(squares::e1)
                   (squares::a4)(squares::b4)(squares::c4)(squares::d4)(squares::f4)(squares::g4)(squares::h4)
    );
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_d4_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::d4);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(27, moves.size());
	CheckMovesDestinationVarying(moves, squares::d4, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::e5)(squares::f6)(squares::g7)(squares::h8)(squares::c3)(squares::b2)(squares::a1)
		           (squares::c5)(squares::b6)(squares::a7)(squares::e3)(squares::f2)(squares::g1)
                   (squares::d6)(squares::d7)(squares::d8)(squares::d5)(squares::d3)(squares::d2)(squares::d1)
                   (squares::a4)(squares::b4)(squares::c4)(squares::e4)(squares::f4)(squares::g4)(squares::h4)
    );
}

//Central
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_d5_OnOtherwiseClearBoard)
{
	SetWhiteQueenAt(position, squares::d5);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(27, moves.size());
	CheckMovesDestinationVarying(moves, squares::d5, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::e6)(squares::f7)(squares::g8)(squares::c4)(squares::b3)(squares::a2)
		           (squares::c6)(squares::b7)(squares::a8)(squares::e4)(squares::f3)(squares::g2)(squares::h1)
                   (squares::d6)(squares::d7)(squares::d8)(squares::d4)(squares::d3)(squares::d2)(squares::d1)
                   (squares::a5)(squares::b5)(squares::c5)(squares::e5)(squares::f5)(squares::g5)(squares::h5)
    );
}

//Down left capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_WithBlackPieceOn_b2)
{
	SetWhiteQueenAt(position, squares::e5);
	SetBlackPawnAt(position, squares::b2);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(26, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::f6)(squares::g7)(squares::h8)(squares::d4)(squares::c3)
		           (squares::d6)(squares::c7)(squares::b8)(squares::f4)(squares::g3)(squares::h2)
                   (squares::e6)(squares::e7)(squares::e8)(squares::e4)(squares::e3)(squares::e2)(squares::e1)
                   (squares::a5)(squares::b5)(squares::c5)(squares::d5)(squares::f5)(squares::g5)(squares::h5)
    );
	CheckMoveIsInList(moves, squares::e5, squares::b2, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

//Up left capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_WithBlackPieceOn_c7)
{
	SetWhiteQueenAt(position, squares::e5);
	SetBlackPawnAt(position, squares::c7);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(26, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::f6)(squares::g7)(squares::h8)(squares::d4)(squares::c3)(squares::b2)(squares::a1)
		           (squares::d6)(squares::f4)(squares::g3)(squares::h2)
                   (squares::e6)(squares::e7)(squares::e8)(squares::e4)(squares::e3)(squares::e2)(squares::e1)
                   (squares::a5)(squares::b5)(squares::c5)(squares::d5)(squares::f5)(squares::g5)(squares::h5)
    );
	CheckMoveIsInList(moves, squares::e5, squares::c7, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

//Down right capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_WithBlackPieceOn_f4)
{
	SetWhiteQueenAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f4);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(25, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::f6)(squares::g7)(squares::h8)(squares::d4)(squares::c3)(squares::b2)(squares::a1)
		           (squares::d6)(squares::c7)(squares::b8)
                   (squares::e6)(squares::e7)(squares::e8)(squares::e4)(squares::e3)(squares::e2)(squares::e1)
                   (squares::a5)(squares::b5)(squares::c5)(squares::d5)(squares::f5)(squares::g5)(squares::h5)
    );
	CheckMoveIsInList(moves, squares::e5, squares::f4, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

//Up right capture from central position
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_WithBlackPieceOn_f6)
{
	SetWhiteQueenAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f6);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(25, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::f4)(squares::g3)(squares::h2)(squares::d4)(squares::c3)(squares::b2)(squares::a1)
		           (squares::d6)(squares::c7)(squares::b8)
                   (squares::e6)(squares::e7)(squares::e8)(squares::e4)(squares::e3)(squares::e2)(squares::e1)
                   (squares::a5)(squares::b5)(squares::c5)(squares::d5)(squares::f5)(squares::g5)(squares::h5)
    );
	CheckMoveIsInList(moves, squares::e5, squares::f6, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

//Up right capture from corner
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_a1_WithBlackPieceOn_c3)
{
	SetWhiteQueenAt(position, squares::a1);
	SetBlackPawnAt(position, squares::c3);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(16, moves.size());
    CheckMovesDestinationVarying(moves, squares::a1, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::a2)(squares::a3)(squares::a4)(squares::a5)(squares::a6)(squares::a7)(squares::a8)
		           (squares::b1)(squares::c1)(squares::d1)(squares::e1)(squares::f1)(squares::g1)(squares::h1)
                   (squares::b2)
    );
	CheckMoveIsInList(moves, squares::a1, squares::b2, pieces::WHITE_QUEEN, pieces::NONE);
	CheckMoveIsInList(moves, squares::a1, squares::c3, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_c4_WithBlackPieceOn_b3)
{
	SetWhiteQueenAt(position, squares::c4);
	SetBlackPawnAt(position, squares::b3);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(24, moves.size());
	CheckMovesDestinationVarying(moves, squares::c4, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::d5)(squares::e6)(squares::f7)(squares::g8)(squares::b5)(squares::a6)(squares::d3)
		           (squares::e2)(squares::f1)
                   (squares::c3)(squares::c2)(squares::c1)(squares::c5)(squares::c6)(squares::c7)(squares::c8)
                   (squares::a4)(squares::b4)(squares::d4)(squares::e4)(squares::f4)(squares::g4)(squares::h4)
    );
	CheckMoveIsInList(moves, squares::c4, squares::b3, pieces::WHITE_QUEEN, pieces::BLACK_PAWN);
}

//No clash, so same as clear board.
TEST_F(MoveGeneratorTests, TestThat_GenerateQueenMoves_GeneratesExpectedMovesForQueenOn_e5_WithBlackPieceOn_f3)
{
	SetWhiteQueenAt(position, squares::e5);
	SetBlackPawnAt(position, squares::f3);

	auto moves = generator.GenerateQueenMoves(position, sides::white);

	ASSERT_EQ(27, moves.size());
	CheckMovesDestinationVarying(moves, squares::e5, pieces::WHITE_QUEEN, pieces::NONE,
		ba::list_of(squares::f6)(squares::g7)(squares::h8)(squares::d4)(squares::c3)(squares::b2)(squares::a1)
		           (squares::d6)(squares::c7)(squares::b8)(squares::f4)(squares::g3)(squares::h2)
                   (squares::e6)(squares::e7)(squares::e8)(squares::e4)(squares::e3)(squares::e2)(squares::e1)
                   (squares::a5)(squares::b5)(squares::c5)(squares::d5)(squares::f5)(squares::g5)(squares::h5)
    );
}

}

