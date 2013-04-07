#include "MoveGenerator.hpp"
#include "BasicOperations.hpp"

namespace chess
{
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
			bitboard destinations = moves::rook_horiz_moves[square][rankOccupancy] & ~position.sides[side];
			GenerateMoves(destinations, move, moves, position, side);

			bitboard fileOccupancy = GetFileOccupancy(position.wholeBoard, file);
			destinations = moves::rook_vert_moves[square][fileOccupancy] & ~position.sides[side];
			GenerateMoves(destinations, move, moves, position, side);

			/*
            bitboard this_rook = GetAndClearFirstSetBitReturningIndex(rooks, sq);
            //std::cout << "rook is on " << print_square(sq) << std::endl;

            int rank, file;
			IndexToRankAndFile(sq, rank, file);

            bitboard rank_occ = GetRankOccupancy(pos.wholeBoard, rank); //horizontal
            bitboard moves = moves::rook_horiz_moves[sq][rank_occ]; 

			bitboard file_occ = GetFileOccupancy(pos.wholeBoard, file); //vertical
            moves |= moves::rook_vert_moves[sq][file_occ]; 

			moves &= (pos.GetOtherSideWithoutKing(side) | pos.GetEmptySquares());

            while(moves)
            {
                Board new_pos = pos;
                new_pos.rooks[side] = (new_pos.rooks[side] | GetAndClearFirstSetBit(moves))
                    & ~this_rook;
                new_pos.RemoveCaptured(side, new_pos.rooks[side]);
                results.push_back(new_pos);
            }*/
        }
		return moves;
    }

    /*
    void MoveGenerator::GenerateBishopMoves(const Board &pos, PositionVector &results, int side)
    {
        bitboard bishops = pos.bishops[side];
        while (bishops)
        {
            int sq, rank, file;
            bitboard this_bishop = GetAndClearFirstSetBitReturningIndex(bishops, sq);
            //std::cout << "bishop is on " << print_square(sq) << std::endl;
            IndexToRankAndFile(sq, rank, file);
        }
    }
	*/
    
	/*
	void MoveGenerator::GenerateAllMoves(const Board &pos, PositionVector &results)
    {
        //king_moves(pos, results, sides::white);
        //std::cout << "GENERATING PAWN MOVES\n";
        //pawn_moves(pos, results, sides::white);
        //pawn_moves(pos, results, sides::black);
        //rook_moves(pos, results, sides::white);
		GenerateKnightMoves(pos, results, sides::white);

		for (auto const &board : results)
		{
			board.CheckBoardConsistency();
		}

        //print_Boards(results);
    }
	*/
}