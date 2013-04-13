#include "MoveGenerator.hpp"
#include "BasicOperations.hpp"

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

			bool promoting = (IndexToRank(square) == sides::ABOUT_TO_PROMOTE[side]); //if we're on the 7th or 2nd ranks, we're gonna promote.

			bitboard destinations = moves::pawn_moves[side][square] & ~position.sides[side];
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

			int rank, file;
			IndexToRankAndFile(square, rank, file);
			//OINK_TODO
			//bitboard diagonalOccupancy = GetDiagonalOccupancy(position.wholeBoard, square);
			//bitboard destinations = moves::diag_moves[square][diagonalOccupancy] & ~position.sides[side];
			//GenerateMoves(destinations, move, moves, position, side);
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