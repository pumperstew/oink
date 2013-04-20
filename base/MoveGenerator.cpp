#include "MoveGenerator.hpp"
#include "BasicOperations.hpp"
#include "Display.hpp"

#include <boost/assign.hpp>

using namespace std;
namespace ba = boost::assign;

#define OINK_MOVEGEN_DIAGNOSTICS

namespace chess
{
	//OINK_TODO: probably better to specialise for pawns to prevent branch for all other pieces.
	void GenerateMoves(bitboard destinations, Move &move, MoveVector &moves, const Position &position, int side, bool promoting = false)
	{
		while (destinations)
        {
			int destinationSquare = GetFirstIndexAndClear(destinations);
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
				moves.push_back(move);
			}
			else moves.push_back(move);
        }
	}

	MoveVector MoveGenerator::GenerateKingMoves(const Position &position, int side)
    {
		MoveVector moves;
		Move move;
		move.SetPiece(pieces::KINGS[side]);

		bitboard king = position.kings[side];
		int square = GetFirstIndexAndClear(king);
		move.SetSource(square);

		GenerateMoves(moves::king_moves[square] & ~position.sides[side], move, moves, position, side);

		return moves;
    }

    MoveVector MoveGenerator::GenerateKnightMoves(const Position &position, int side)
    {
		MoveVector moves;
		Move move;
		move.SetPiece(pieces::KNIGHTS[side]);

        bitboard knights = position.knights[side];
        while (knights)
        {
			int square = GetFirstIndexAndClear(knights);
			move.SetSource(square);

			GenerateMoves(moves::knight_moves[square] & ~position.sides[side], move, moves, position, side);
        }
		return moves;
    }

	//If we wanted to store the new position in the move, we could do (in innner loop)
	//But RemoveCaptured step is ugly (switch). But we have to do it either here or when the move is made.
    //Position newPosition = position;
    //newPosition.knights[side] = (newPosition.knights[side] | destinationBitboard) & ~thisKnight;
    //newPosition.RemoveCaptured(side, newPosition.knights[side]);

	MoveVector MoveGenerator::GeneratePawnMoves(const Position &position, int side)
    {
		MoveVector moves;
		Move move;
		move.SetPiece(pieces::PAWNS[side]);

        bitboard pawns = position.pawns[side];
        while (pawns)
        {
            int square = GetFirstIndexAndClear(pawns);
			move.SetSource(square);

			int rank = IndexToRank(square);
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
			GenerateMoves(destinations, move, moves, position, side, promoting);

			destinations = moves::pawn_captures[side][square] & position.GetOtherSide(side);
			GenerateMoves(destinations, move, moves, position, side, promoting);
            //OINK_TODO: we need to generate EP moves!
        }
		return moves;
    }

    MoveVector MoveGenerator::GenerateRookMoves(const Position &position, int side)
    {
        MoveVector moves;
		Move move;
		move.SetPiece(pieces::ROOKS[side]);

        bitboard rooks = position.rooks[side];
        while (rooks)
        {
            int square = GetFirstIndexAndClear(rooks);
			move.SetSource(square);

			int rank, file;
			IndexToRankAndFile(square, rank, file);

			bitboard rankOccupancy = GetRankOccupancy(position.wholeBoard, rank);
			bitboard fileOccupancy = GetFileOccupancy(position.wholeBoard, file);
			bitboard destinations = (moves::rook_horiz_moves[square][rankOccupancy] | 
									 moves::rook_vert_moves[square][fileOccupancy]) 
									 & ~position.sides[side];

			GenerateMoves(destinations, move, moves, position, side);
        }
		return moves;
    }

    MoveVector MoveGenerator::GenerateBishopMoves(const Position &position, int side)
    {
        MoveVector moves;
		Move move;
		move.SetPiece(pieces::BISHOPS[side]);

        bitboard bishops = position.bishops[side];
        while (bishops)
        {
			int square = GetFirstIndexAndClear(bishops);
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
			GenerateMoves(destinations, move, moves, position, side);
        }
		return moves;
    }
    
	/*
	MoveVector MoveGenerator::GenerateAllMoves(const Position &position, int side)
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