#include "Position.hpp"

#ifdef OINK_MOVEGEN_DIAGNOSTICS
    #include "Display.hpp"
#endif

namespace chess
{
	Position::Position()
	{
        clear();
    }

	void Position::clear()
	{
        memset(piece_bbs, util::nil, sizeof(piece_bbs));
        memset(sides, util::nil, sizeof(sides));
        memset(squares, pieces::NONE, sizeof(squares));

        castling_rights = sides::CASTLING_RIGHTS_ANY_WHITE | sides::CASTLING_RIGHTS_ANY_BLACK;
        ep_target_square = squares::NO_SQUARE;
        fifty_move_count = 0;
	}

	void Position::setup_starting_position()
	{
        castling_rights  = sides::CASTLING_RIGHTS_ANY_WHITE | sides::CASTLING_RIGHTS_ANY_BLACK;
        ep_target_square = squares::NO_SQUARE;
        fifty_move_count = 0;

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

		update_sides();

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
	
	Bitboard Position::generate_side(Side side) const
    {
        return kings[side]	 | 
			   rooks[side]   | 
			   knights[side] |
               bishops[side] | 
			   queens[side]  |
			   pawns[side];
    }

	void Position::update_sides()
	{
		sides[sides::white] = generate_side(sides::white);
		sides[sides::black] = generate_side(sides::black);
		whole_board = sides[sides::white] | sides[sides::black];
	}

    void Position::move_common_first_stage(Piece moving_piece, Side side, Square source, Square dest, Bitboard source_and_dest_bitboard)
    {
        piece_bbs[moving_piece]  ^= source_and_dest_bitboard;
        sides[side]              ^= source_and_dest_bitboard;
        squares[source]           = pieces::NONE;
        squares[dest]             = moving_piece;
        ep_target_square          = squares::NO_SQUARE;
    }

    void Position::move_common_second_stage(Piece captured_piece, Side side_capturing, Bitboard dest_bitboard, Bitboard source_bitboard, Bitboard source_and_dest_bitboard)
    {
        if (captured_piece != pieces::NONE)
        {
            piece_bbs[captured_piece]           ^= dest_bitboard;
            sides[swap_side(side_capturing)]    ^= dest_bitboard;
            whole_board                         ^= source_bitboard; // Not source_and_dest_bitboard, as dest is occupied before and after.
            fifty_move_count = 0;

            // Anything captured on the corner squares must remove castling rights, because either the rook has already moved, or we're capturing it.
            // h1 => map to CASTLING_RIGHTS_WHITE_KINGSIDE, etc.
            Bitboard mask;
            // Shift the set bit, if any, down to the right position (e.g. >> 7 and << CASTLING_RIGHTS_WHITE_KINGSIDE ==> >> 6)
            mask  = (dest_bitboard & squarebits::h1) >> 6;  /* CASTLING_RIGHTS_WHITE_KINGSIDE  == 0x1 */
            mask |= (dest_bitboard & squarebits::a1) << 1;  /* CASTLING_RIGHTS_WHITE_QUEENSIDE == 0x2 */
            mask |= (dest_bitboard & squarebits::h8) << 61; /* CASTLING_RIGHTS_BLACK_KINGSIDE  == 0x4 */
            mask |= (dest_bitboard & squarebits::a8) << 53; /* CASTLING_RIGHTS_BLACK_QUEENSIDE == 0x8 */

            castling_rights &= ~mask; // If the bits in the mask are set, this turns off the relevant bits.
        }
        else
        {
            whole_board ^= source_and_dest_bitboard;
            ++fifty_move_count;
        }
    }

    bool Position::detect_check(Side king_side) const
    {
        Square king_square = get_first_occ_square(kings[king_side]);
        RankFile king_rank, king_file;
        square_to_rank_file(king_square, king_rank, king_file);
            
        Side other_side = swap_side(king_side);

        // Place imaginary pawn where the king is, of the same colour. Can it capture any pawns of the other colour? If so, the king is in check from a pawn.
        if (pawns[other_side] & moves::pawn_captures[king_side][king_square]) 
            return true;
        // Similar for knights
        if (knights[other_side] & moves::knight_moves[king_square])
            return true;
        // Kings too
        if (kings[other_side] & moves::king_moves[king_square])
            return true;

        // Rank / file sliders: similar idea -- try to attack the other side's rooks.
        Bitboard attackers = queens[other_side] | rooks[other_side];
        Bitboard rotated_occ = get_6bit_rank_occupancy(whole_board, king_rank);
        if (attackers & moves::horiz_slider_moves[king_square][rotated_occ])
            return true;

        rotated_occ = get_6bit_file_occupancy(whole_board, king_file);
        if (attackers & moves::vert_slider_moves[king_square][rotated_occ])
            return true;

        // Diagonal sliders
        attackers = queens[other_side] | bishops[other_side];
        rotated_occ = project_occupancy_from_a1h8_to6bit(whole_board, king_square);
        if (attackers & moves::diag_moves_a1h8[king_square][rotated_occ])
            return true;

        rotated_occ = project_occupancy_from_a8h1_to6bit(whole_board, king_square);
        if (attackers & moves::diag_moves_a8h1[king_square][rotated_occ])
            return true;

        return false;
    }

    bool Position::make_move(Move move)
    {
        Position backup = Position(*this);

        Piece moving_piece   = move.get_piece();
        Side  side           = get_piece_side(moving_piece);
        Piece captured_piece = move.get_captured_piece();

        Square source        = move.get_source();
        RankFile source_rank = square_to_rank(source);

        Square   dest      = move.get_destination();
        RankFile dest_rank = square_to_rank(dest);

        Bitboard source_bitboard          = util::one << source;
        Bitboard dest_bitboard            = util::one << dest;
        Bitboard source_and_dest_bitboard = source_bitboard | dest_bitboard;

        switch (moving_piece)
        {
        case pieces::WHITE_PAWN:
        case pieces::BLACK_PAWN:

            move_common_first_stage(moving_piece, side, source, dest, source_and_dest_bitboard);

            // EP square is on third/sixth rank, if applicable:
            ep_target_square = abs(dest_rank - source_rank) == 2 ? source + sides::NEXT_RANK_OFFSET[side] : squares::NO_SQUARE;

            if (move.get_en_passant() != pieces::NONE)
            {
#ifdef OINK_MOVEGEN_DIAGNOSTICS
                if (squares[dest - sides::NEXT_RANK_OFFSET[side]] != pieces::WHITE_PAWN &&
                    squares[dest - sides::NEXT_RANK_OFFSET[side]] != pieces::BLACK_PAWN)
                {
                    print_bitboard(source_bitboard, "source");
                    print_bitboard(dest_bitboard, "dest");
                    print_bitboard(util::one << backup.ep_target_square, "old ep sq");
                    print_position(*this);
                    print_position(backup);
                }
#endif
                Bitboard pawn_captured_ep_mask    = util::one << (dest - sides::NEXT_RANK_OFFSET[side]);  // the square below or above dest. Less shift for white, more for black
                pawns[swap_side(side)]           ^= pawn_captured_ep_mask;
                sides[swap_side(side)]           ^= pawn_captured_ep_mask;
                squares[dest - sides::NEXT_RANK_OFFSET[side]] = pieces::NONE;
                whole_board                      ^= (source_and_dest_bitboard | pawn_captured_ep_mask);
            }
            else
            {
                move_common_second_stage(captured_piece, side, dest_bitboard, source_bitboard, source_and_dest_bitboard);

                Piece promotion_piece = move.get_promotion_piece();
                if (promotion_piece != pieces::NONE)
                {
                    pawns[sides::white]         ^= dest_bitboard; // Turn back off pawn at dest
                    piece_bbs[promotion_piece]  ^= dest_bitboard; // Turn on new piece at dest
                    squares[dest]               = promotion_piece;
                }
            }

            fifty_move_count = 0;

            break;

        case pieces::WHITE_KING:
        case pieces::BLACK_KING:

            move_common_first_stage(moving_piece, side, source, dest, source_and_dest_bitboard);
            move_common_second_stage(captured_piece, side, dest_bitboard, source_bitboard, source_and_dest_bitboard);

            castling_rights &= ~sides::CASTLING_RIGHTS_ANY[side];

            if (move.get_castling() != pieces::NONE)
            {
                Bitboard rook_mask;

                if (dest == squares::g1)
                {
                    rook_mask = squarebits::h1 | squarebits::f1;
                    squares[squares::h1] = pieces::NONE;
                    squares[squares::f1] = pieces::WHITE_ROOK;
                }
                else if (dest == squares::c1)
                {
                    rook_mask = squarebits::a1 | squarebits::c1;
                    squares[squares::a1] = pieces::NONE;
                    squares[squares::c1] = pieces::BLACK_ROOK;
                }
                else if (dest == squares::g8)
                {
                    rook_mask = squarebits::h8 | squarebits::f8;
                    squares[squares::h8] = pieces::NONE;
                    squares[squares::g8] = pieces::BLACK_ROOK;
                }
                else if (dest == squares::c8)
                {
                    rook_mask = squarebits::a8 | squarebits::c8;
                    squares[squares::a8] = pieces::NONE;
                    squares[squares::c8] = pieces::BLACK_ROOK;
                }

                rooks[side] ^= rook_mask;
                sides[side] ^= rook_mask;
                whole_board ^= rook_mask;
            }

            break;

        case pieces::WHITE_ROOK:
        case pieces::BLACK_ROOK:

            move_common_first_stage(moving_piece, side, source, dest, source_and_dest_bitboard);
            move_common_second_stage(captured_piece, side, dest_bitboard, source_bitboard, source_and_dest_bitboard);

            //if (source == sides::KING_ROOK_START[side])  castling_rights &= ~sides::CASTLING_RIGHTS_KINGSIDE[side];
            //if (source == sides::QUEEN_ROOK_START[side]) castling_rights &= ~sides::CASTLING_RIGHTS_QUEENSIDE[side];

            // See notes in move_common_second_stage(), capture branch for this logic to avoid branching when removing castling rights.
            // The difference is that source_bitboard is used here, rather than dest_bitboard in move_common_second_stage(); 
            // note that it's ok to do all four cases, as if e.g. we're moving from h8 as a white rook, black doesn't have castling 
            // rights there anyway; turning them off again is OK.
            Bitboard mask;
            mask  = (source_bitboard & squarebits::h1) >> 6;  /* CASTLING_RIGHTS_WHITE_KINGSIDE  == 0x1 */
            mask |= (source_bitboard & squarebits::a1) << 1;  /* CASTLING_RIGHTS_WHITE_QUEENSIDE == 0x2 */
            mask |= (source_bitboard & squarebits::h8) << 61; /* CASTLING_RIGHTS_BLACK_KINGSIDE  == 0x4 */
            mask |= (source_bitboard & squarebits::a8) << 53; /* CASTLING_RIGHTS_BLACK_QUEENSIDE == 0x8 */

            castling_rights &= ~mask; // If the bits in the mask are set, this turns off the relevant bits.

            break;

        case pieces::WHITE_KNIGHT:
        case pieces::BLACK_KNIGHT:
        case pieces::WHITE_BISHOP:
        case pieces::BLACK_BISHOP:
        case pieces::WHITE_QUEEN:
        case pieces::BLACK_QUEEN:

            move_common_first_stage(moving_piece, side, source, dest, source_and_dest_bitboard);
            move_common_second_stage(captured_piece, side, dest_bitboard, source_bitboard, source_and_dest_bitboard);
            break;
        }

        // If we're in check, undo the move
        if (detect_check(side))
        {
            *this = backup;
            return false;
        }
        return true;
    }


	/*
	void Position::test_rot45()
	{
		//generate rot45 Bitboards
		all_rot45_a1h8 = 0;
		Bitboard shift_bottom = 0;
		for (int i = 0; i < util::BOARD_SIZE; ++i) { //files/ranks
			Bitboard all_shifted_bottom = (wholeBoard & moves::diag_masks_a1h8[i]) >> i;
			Bitboard all_shifted_left = 0, this_diag_left = 0;

			Bitboard this_diag_bottom = //the diags starting from the bottom of the board
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
				//chess::print_Bitboard(moves::diag_masks_a1h8[i]);
				//chess::print_Bitboard(all_shifted_bottom);
				//chess::print_Bitboard(all_shifted_left);
				//chess::print_Bitboard(this_diag_bottom);
				//chess::print_Bitboard(this_diag_left);
			}
			shift_bottom += util::BOARD_SIZE - i;
		}
		//chess::print_Bitboard(all_rot45_a1h8);
	}
	//ok, so we can generate rot45 Bitboards, but doing all the above (and again for a8h1!!)
	//every time a piece moves (i.e. everytime u gen a new pos) is gonna be horrible.
	//=> we need to use the idea from crafty, i.e. when you move something, update all
	//Bitboards incrementally (so you make move() a method of board, with probably special
	//cases for captures, etc).
	//of course if it's very inconvenient to do incremental update in a particular case,
	//you can do a full generate. otherwise, the full rot45 gen would only be called at startup..

	*/
}