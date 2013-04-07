#include "Position.hpp"

namespace chess
{
	Position::Position()
	{}

	void Position::Clear()
	{
		kings[sides::white]   = kings[sides::black]   = 
		rooks[sides::white]   = rooks[sides::black]   =
		knights[sides::white] = knights[sides::black] =
		bishops[sides::white] = bishops[sides::black] =
		queens[sides::white]  = queens[sides::black]  = 
		pawns[sides::white]   = pawns[sides::black]	  = util::nil;

		UpdateSides();

		std::fill(squares, squares + sizeof(squares)/sizeof(squares[0]), pieces::NONE);
	}

	void Position::SetupStarting()
	{
		kings[sides::white]   = starting::white_king;
		kings[sides::black]   = starting::black_king;
		rooks[sides::white]   = starting::white_rooks;
		rooks[sides::black]   = starting::black_rooks;
		knights[sides::white] = starting::white_knights;
		knights[sides::black] = starting::black_knights;
		bishops[sides::white] = starting::white_bishops;
		bishops[sides::black] = starting::black_bishops;
		queens[sides::white]  = starting::white_queen;
		queens[sides::black]  = starting::black_queen;
		pawns[sides::white]   = starting::white_pawns;
		pawns[sides::black]   = starting::black_pawns;

		UpdateSides();

		squares[squares::a1] = pieces::WHITE_ROOK;
		squares[squares::b1] = pieces::WHITE_KNIGHT;
		squares[squares::c1] = pieces::WHITE_BISHOP;
		squares[squares::d1] = pieces::WHITE_QUEEN;
		squares[squares::e1] = pieces::WHITE_KING;
		squares[squares::f1] = pieces::WHITE_BISHOP;
		squares[squares::g1] = pieces::WHITE_KNIGHT;
		squares[squares::h1] = pieces::WHITE_ROOK;

		squares[squares::a2] = pieces::WHITE_PAWN;
		squares[squares::b2] = pieces::WHITE_PAWN;
		squares[squares::c2] = pieces::WHITE_PAWN;
		squares[squares::d2] = pieces::WHITE_PAWN;
		squares[squares::e2] = pieces::WHITE_PAWN;
		squares[squares::f2] = pieces::WHITE_PAWN;
		squares[squares::g2] = pieces::WHITE_PAWN;
		squares[squares::h2] = pieces::WHITE_PAWN;

		squares[squares::a3] = pieces::NONE;
		squares[squares::b3] = pieces::NONE;
		squares[squares::c3] = pieces::NONE;
		squares[squares::d3] = pieces::NONE;
		squares[squares::e3] = pieces::NONE;
		squares[squares::f3] = pieces::NONE;
		squares[squares::g3] = pieces::NONE;
		squares[squares::h3] = pieces::NONE;

		squares[squares::a4] = pieces::NONE;
		squares[squares::b4] = pieces::NONE;
		squares[squares::c4] = pieces::NONE;
		squares[squares::d4] = pieces::NONE;
		squares[squares::e4] = pieces::NONE;
		squares[squares::f4] = pieces::NONE;
		squares[squares::g4] = pieces::NONE;
		squares[squares::h4] = pieces::NONE;

		squares[squares::a5] = pieces::NONE;
		squares[squares::b5] = pieces::NONE;
		squares[squares::c5] = pieces::NONE;
		squares[squares::d5] = pieces::NONE;
		squares[squares::e5] = pieces::NONE;
		squares[squares::f5] = pieces::NONE;
		squares[squares::g5] = pieces::NONE;
		squares[squares::h5] = pieces::NONE;

		squares[squares::a6] = pieces::NONE;
		squares[squares::b6] = pieces::NONE;
		squares[squares::c6] = pieces::NONE;
		squares[squares::d6] = pieces::NONE;
		squares[squares::e6] = pieces::NONE;
		squares[squares::f6] = pieces::NONE;
		squares[squares::g6] = pieces::NONE;
		squares[squares::h6] = pieces::NONE;

		squares[squares::a7] = pieces::BLACK_PAWN;
		squares[squares::b7] = pieces::BLACK_PAWN;
		squares[squares::c7] = pieces::BLACK_PAWN;
		squares[squares::d7] = pieces::BLACK_PAWN;
		squares[squares::e7] = pieces::BLACK_PAWN;
		squares[squares::f7] = pieces::BLACK_PAWN;
		squares[squares::g7] = pieces::BLACK_PAWN;
		squares[squares::h7] = pieces::BLACK_PAWN;

		squares[squares::a8] = pieces::BLACK_ROOK;
		squares[squares::b8] = pieces::BLACK_KNIGHT;
		squares[squares::c8] = pieces::BLACK_BISHOP;
		squares[squares::d8] = pieces::BLACK_QUEEN;
		squares[squares::e8] = pieces::BLACK_KING;
		squares[squares::f8] = pieces::BLACK_BISHOP;
		squares[squares::g8] = pieces::BLACK_KNIGHT;
		squares[squares::h8] = pieces::BLACK_ROOK;
	}
	
	bitboard Position::GenerateSide(int side) const
    {
        return kings[side]	 | 
			   rooks[side]   | 
			   knights[side] |
               bishops[side] | 
			   queens[side]  |
			   pawns[side];
    }

	void Position::UpdateSides()
	{
		sides[sides::white] = GenerateSide(sides::white);
		sides[sides::black] = GenerateSide(sides::black);
		wholeBoard = sides[sides::white] | sides[sides::black];
	}

	char Position::GetPieceSymbolAtIndex(int index) const 
	{
		bitboard b = util::one << index;
		if (b & kings[sides::white])
			return 'K';
		else if (b & kings[sides::black])
			return 'k';
		else if (b & rooks[sides::white])
			return 'R';
		else if (b & rooks[sides::black])
			return 'r';
		else if (b & knights[sides::white])
			return 'N';
		else if (b & knights[sides::black])
			return 'n';
		else if (b & bishops[sides::white])
			return 'B';
		else if (b & bishops[sides::black])
			return 'b';
		else if (b & queens[sides::white])
			return 'Q';
		else if (b & queens[sides::black])
			return 'q';
		else if (b & pawns[sides::white])
			return 'P';
		else if (b & pawns[sides::black])
			return 'p';
		else
			return 'O';
	}

	//OINK_TODO: this kinda sucks.
	void Position::RemoveCaptured(int sideCapturing, bitboard diff) 
    {
        int s = sideCapturing ^ 1;
        pawns[s]   &= ~diff;
        rooks[s]   &= ~diff;
        knights[s] &= ~diff;
        queens[s]  &= ~diff;
        bishops[s] &= ~diff;
        kings[s]   &= ~diff;
		UpdateSides();
    }

	/*
	void Position::test_rot45()
	{
		//generate rot45 bitboards
		all_rot45_a1h8 = 0;
		bitboard shift_bottom = 0;
		for (int i = 0; i < util::BOARD_SIZE; ++i) { //files/ranks
			bitboard all_shifted_bottom = (wholeBoard & moves::diag_masks_a1h8[i]) >> i;
			bitboard all_shifted_left = 0, this_diag_left = 0;

			bitboard this_diag_bottom = //the diags starting from the bottom of the board
				(all_shifted_bottom | (all_shifted_bottom >> 8) | (all_shifted_bottom >> 16)
				| (all_shifted_bottom >> 24) | (all_shifted_bottom >> 32) 
				| (all_shifted_bottom >> 40) | (all_shifted_bottom >> 48)
				| (all_shifted_bottom >> 56)) & util::fullrank;
			all_rot45_a1h8 |= (this_diag_bottom << shift_bottom);

			if (i > 0) { //..and those starting from the left (dont do long diag twice)
				all_shifted_left = (wholeBoard & moves::diag_masks_a1h8[i<<3]) >> (i<<3);
				this_diag_left = 
					(all_shifted_left | (all_shifted_left >> 8) | (all_shifted_left >> 16)
					| (all_shifted_left >> 24) | (all_shifted_left >> 32) 
					| (all_shifted_left >> 40) | (all_shifted_left >> 48)
					| (all_shifted_left >> 56)) & util::fullrank;
				all_rot45_a1h8 |= (this_diag_left << (shift_bottom + 28));
			}
			if (i == 1) {
				//chess::print_bitboard(moves::diag_masks_a1h8[i]);
				//chess::print_bitboard(all_shifted_bottom);
				//chess::print_bitboard(all_shifted_left);
				//chess::print_bitboard(this_diag_bottom);
				//chess::print_bitboard(this_diag_left);
			}
			shift_bottom += util::BOARD_SIZE - i;
		}
		//chess::print_bitboard(all_rot45_a1h8);
	}
	//ok, so we can generate rot45 bitboards, but doing all the above (and again for a8h1!!)
	//every time a piece moves (i.e. everytime u gen a new pos) is gonna be horrible.
	//=> we need to use the idea from crafty, i.e. when you move something, update all
	//bitboards incrementally (so you make move() a method of board, with probably special
	//cases for captures, etc).
	//of course if it's very inconvenient to do incremental update in a particular case,
	//you can do a full generate. otherwise, the full rot45 gen would only be called at startup..

	*/
}