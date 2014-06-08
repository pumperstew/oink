#include "MoveGenerator.hpp"
#include "BasicOperations.hpp"

#define OINK_MOVEGEN_DIAGNOSTICS

using namespace std;

#ifdef OINK_MOVEGEN_DIAGNOSTICS
    #include "Display.hpp"
    #include <boost/assign.hpp>
    namespace ba = boost::assign;
#endif

namespace chess
{
	static void DestinationsMoveGenLoop(bitboard destinations, Move &move, MoveVector &moves, const Position &position, Side side)
	{
		while (destinations)
        {
			Square destinationSquare = GetFirstIndexAndClear(destinations);
			move.SetDestination(destinationSquare);
			move.SetCapturedPiece(position.squares[destinationSquare]);
            moves.push_back(move);
        }
	}

    static void DestinationsMoveGenLoopPawns(bitboard destinations, Move &move, MoveVector &moves, const Position &position, Side side, bool promoting)
	{
		while (destinations)
        {
			Square destinationSquare = GetFirstIndexAndClear(destinations);
			move.SetDestination(destinationSquare);
			move.SetCapturedPiece(position.squares[destinationSquare]);
			if (promoting)
			{
				move.SetPromotionPiece(pieces::QUEENS[side]);
				moves.push_back(move);
				move.SetPromotionPiece(pieces::ROOKS[side]);
				moves.push_back(move);
				move.SetPromotionPiece(pieces::KNIGHTS[side]);
				moves.push_back(move);
				move.SetPromotionPiece(pieces::BISHOPS[side]);
			}
            moves.push_back(move);
        }
	}

	//OINK_TODO: castling moves will need to be checked for validity later.
	MoveVector MoveGenerator::GenerateKingMoves(const Position &position, Side side)
    {
		MoveVector moves;
		Move move;
		move.SetPiece(pieces::KINGS[side]);

		bitboard king = position.kings[side];
		Square square = GetFirstIndex(king);
		move.SetSource(square);

		DestinationsMoveGenLoop(moves::king_moves[square] & ~position.sides[side], move, moves, position, side);

        if (side == sides::white && square == squares::e1)
        {
            if (position.squares[squares::h1] == pieces::WHITE_ROOK)
            {
                move.SetDestination(squares::g1);
                move.SetCapturedPiece(pieces::NONE);
                move.SetCastling(pieces::WHITE_KING);
                moves.push_back(move);
            }

            if (position.squares[squares::a1] == pieces::WHITE_ROOK)
            {
                move.SetDestination(squares::c1);
                move.SetCapturedPiece(pieces::NONE);
                move.SetCastling(pieces::WHITE_KING);
                moves.push_back(move);
            }
        }
        else if (side == sides::black && square == squares::e8)
        {
            if (position.squares[squares::h8] == pieces::BLACK_ROOK)
            {
                move.SetDestination(squares::g8);
                move.SetCapturedPiece(pieces::NONE);
                move.SetCastling(pieces::BLACK_KING);
                moves.push_back(move);
            }

            if (position.squares[squares::a8] == pieces::BLACK_ROOK)
            {
                move.SetDestination(squares::c8);
                move.SetCapturedPiece(pieces::NONE);
                move.SetCastling(pieces::BLACK_KING);
                moves.push_back(move);
            }
        }
		return moves;
    }

    MoveVector MoveGenerator::GenerateKnightMoves(const Position &position, Side side)
    {
		MoveVector moves;
		Move move;
		move.SetPiece(pieces::KNIGHTS[side]);

        bitboard knights = position.knights[side];
        while (knights)
        {
			Square square = GetFirstIndexAndClear(knights);
			move.SetSource(square);

			DestinationsMoveGenLoop(moves::knight_moves[square] & ~position.sides[side], move, moves, position, side);
        }
		return moves;
    }

	//If we wanted to store the new position in the move, we could do (in innner loop)
	//But RemoveCaptured step is ugly (switch). But we have to do it either here or when the move is made.
    //Position newPosition = position;
    //newPosition.knights[side] = (newPosition.knights[side] | destinationBitboard) & ~thisKnight;
    //newPosition.RemoveCaptured(side, newPosition.knights[side]);

	MoveVector MoveGenerator::GeneratePawnMoves(const Position &position, Side side)
    {
		MoveVector moves;
		Move move;
		move.SetPiece(pieces::PAWNS[side]);

        bitboard pawns = position.pawns[side];
        while (pawns)
        {
            Square square = GetFirstIndexAndClear(pawns);
			move.SetSource(square);

			RankFile rank = IndexToRank(square);
			bool promoting = (rank == sides::ABOUT_TO_PROMOTE[side]); //if we're on the 7th or 2nd ranks, we're gonna promote.

			bitboard wholeBoard = position.wholeBoard;
			if (rank == sides::ABOUT_TO_PROMOTE[side ^ 1])
				wholeBoard = ExcludeFourthOrFifthRank(wholeBoard, side);

			 //since for pawns we're doing captures separately, we use wholeBoard here. we also exclude 4th(4th) rank if 3rd(6th) is occupied.
			bitboard destinations = moves::pawn_moves[side][square] & ~wholeBoard;
			
#ifdef OINK_MOVEGEN_DIAGNOSTICS
			/*PrintBitboard(destinations, "destinations", square);
			PrintBitboard(position.wholeBoard, "wholeBoard before", square);
			PrintBitboard(wholeBoard, "wholeBoard", square);*/
#endif
			DestinationsMoveGenLoopPawns(destinations, move, moves, position, side, promoting);

			destinations = moves::pawn_captures[side][square] & position.GetOtherSide(side);
			DestinationsMoveGenLoopPawns(destinations, move, moves, position, side, promoting);
            //OINK_TODO: we need to generate EP moves!
        }
		return moves;
    }

	MoveVector GenerateRankFileSliderMoves(const Position &position, Side side, Move &move, bitboard movingPieceBitboard)
	{
		MoveVector moves;
		while (movingPieceBitboard)
		{
			Square square = GetFirstIndexAndClear(movingPieceBitboard);
			move.SetSource(square);

			RankFile rank, file;
			IndexToRankAndFile(square, rank, file);

			bitboard rankOccupancy = GetRankOccupancy(position.wholeBoard, rank);
			bitboard fileOccupancy = GetFileOccupancy(position.wholeBoard, file);
			bitboard destinations = (moves::rook_horiz_moves[square][rankOccupancy] |
									 moves::rook_vert_moves[square][fileOccupancy])
									 & ~position.sides[side];

			DestinationsMoveGenLoop(destinations, move, moves, position, side);
		}
		return moves;
	}

    MoveVector MoveGenerator::GenerateRookMoves(const Position &position, Side side)
    {
		Move move;
		move.SetPiece(pieces::ROOKS[side]);
		return GenerateRankFileSliderMoves(position, side, move, position.rooks[side]);
    }

	MoveVector GenerateDiagonalSliderMoves(const Position &position, Side side, Move &move, bitboard movingPieceBitboard)
	{
		MoveVector moves;

		while (movingPieceBitboard)
		{
			Square square = GetFirstIndexAndClear(movingPieceBitboard);
			move.SetSource(square);

#ifdef OINK_MOVEGEN_DIAGNOSTICS
			PrintBitboards(ba::list_of(make_pair(position.wholeBoard, "whole board"))
				(make_pair(moves::diagMasks_a1h8[square], "diagmasks for square")), square);
#endif

			bitboard diagonalOccupancy_a1h8 = GetDiagonalOccupancy_a1h8(position.wholeBoard, square);
			bitboard diagonalOccupancy_a8h1 = GetDiagonalOccupancy_a8h1(position.wholeBoard, square);
			bitboard destinations = (moves::diag_moves_a1h8[square][diagonalOccupancy_a1h8] | moves::diag_moves_a8h1[square][diagonalOccupancy_a8h1])
				& ~position.sides[side];

			assert(diagonalOccupancy_a8h1 < 256);
			assert(diagonalOccupancy_a1h8 < 256);

#ifdef OINK_MOVEGEN_DIAGNOSTICS
			PrintBitboards(ba::list_of(make_pair(diagonalOccupancy_a1h8, "diagonalOccupancy_a1h8"))
				(make_pair(diagonalOccupancy_a8h1, "diagonalOccupancy_a8h1"))
				(make_pair(moves::diag_moves_a1h8[square][diagonalOccupancy_a1h8], "a1h8 moves"))
				(make_pair(moves::diag_moves_a8h1[square][diagonalOccupancy_a8h1], "a8h1 moves"))
				(make_pair(destinations, "destinations")), square);
#endif
			DestinationsMoveGenLoop(destinations, move, moves, position, side);
		}

		return moves;
	}

    MoveVector MoveGenerator::GenerateBishopMoves(const Position &position, Side side)
    {
		Move move;
		move.SetPiece(pieces::BISHOPS[side]);
		return GenerateDiagonalSliderMoves(position, side, move, position.bishops[side]);
    }

	MoveVector MoveGenerator::GenerateQueenMoves(const Position &position, Side side)
	{
		Move rfMove;
		rfMove.SetPiece(pieces::QUEENS[side]);
		auto moves = GenerateRankFileSliderMoves(position, side, rfMove, position.queens[side]);

		Move diagMove;
		diagMove.SetPiece(pieces::QUEENS[side]);
		auto diagonalMoves = GenerateDiagonalSliderMoves(position, side, diagMove, position.queens[side]);

		moves.insert(moves.end(), diagonalMoves.begin(), diagonalMoves.end());
		return moves;
	}
    
	/*
	MoveVector MoveGenerator::GenerateAllMoves(const Position &position, Side side)
    {
        MoveVector moves = GenerateKingMoves(position, side);

		for (auto const &board : results)
		{
			board.CheckBoardConsistency();
		}

        //print_Boards(results);
    }
	*/
}