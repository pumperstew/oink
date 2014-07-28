#include "Position.hpp"

namespace chess
{
	Position::Position()
	{
        clear();
    }

	void Position::clear()
	{
        memset(piece_bbs, util::nil,    sizeof(piece_bbs));
        memset(sides,     util::nil,    sizeof(sides));
        memset(squares,   pieces::NONE, sizeof(squares));

        castling_rights  = sides::CASTLING_RIGHTS_ANY_WHITE | sides::CASTLING_RIGHTS_ANY_BLACK;
        ep_target_square = squares::NO_SQUARE;
        fifty_move_count = 0;
        material         = 0;
	}

	void Position::setup_starting_position()
	{
        castling_rights  = sides::CASTLING_RIGHTS_ANY_WHITE | sides::CASTLING_RIGHTS_ANY_BLACK;
        ep_target_square = squares::NO_SQUARE;
        fifty_move_count = 0;
        material         = 0;

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

    static inline Bitboard make_castling_mask(Bitboard input)
    {
        Bitboard mask;
        // Shift the set bit, if any, down to the right position.
        mask  = (input & squarebits::h1) >> 7;  /* CASTLING_RIGHTS_WHITE_KINGSIDE  == 0x1 */
        mask |= (input & squarebits::a1) << 1;  /* CASTLING_RIGHTS_WHITE_QUEENSIDE == 0x2 */
        mask |= (input & squarebits::h8) >> 61; /* CASTLING_RIGHTS_BLACK_KINGSIDE  == 0x4 */
        mask |= (input & squarebits::a8) >> 53; /* CASTLING_RIGHTS_BLACK_QUEENSIDE == 0x8 */

        return mask;
    }

    void Position::move_common_second_stage(Piece captured_piece, Side side_capturing, Bitboard dest_bitboard, Bitboard source_bitboard, Bitboard source_and_dest_bitboard)
    {
        if (captured_piece != pieces::NONE)
        {
            piece_bbs[captured_piece]           ^= dest_bitboard;
            sides[swap_side(side_capturing)]    ^= dest_bitboard;
            whole_board                         ^= source_bitboard; // Not source_and_dest_bitboard, as dest is occupied before and after.
            fifty_move_count = 0;
            // e.g. if white's moving, then material goes up by the value of the piece he captured (positive)
            material += evals::PIECE_CAPTURE_VALUES[captured_piece]; 

            // Anything captured on the corner squares must remove castling rights, because either the rook has already moved, or we're capturing it.
            // h1 => map to CASTLING_RIGHTS_WHITE_KINGSIDE, etc.
            castling_rights &= ~make_castling_mask(dest_bitboard); // If the bits in the mask are set, this turns off the relevant bits.
        }
        else
        {
            whole_board ^= source_and_dest_bitboard;
            ++fifty_move_count;
        }
    }

    bool Position::square_attacked(Square square, Side side_on_square) const
    {
        RankFile rank, file;
        square_to_rank_file(square, rank, file);
            
        Side other_side = swap_side(side_on_square);

        // Place imaginary pawn where the king is, of the same colour. Can it capture any pawns of the other colour? If so, the king is in check from a pawn.
        if (pawns[other_side] & moves::pawn_captures[side_on_square][square]) 
            return true;
        // Similar for knights
        if (knights[other_side] & moves::knight_moves[square])
            return true;
        // Kings too (pseudo-check, i.e. for the purposes of legality checking).
        if (kings[other_side] & moves::king_moves[square])
            return true;

        // Rank / file sliders: similar idea -- try to attack the other side's rooks.
        Bitboard attackers = queens[other_side] | rooks[other_side];
        Bitboard rotated_occ = get_6bit_rank_occupancy(whole_board, rank);
        if (attackers & moves::horiz_slider_moves[square][rotated_occ])
            return true;

        rotated_occ = project_occupancy_from_file_to6bit(whole_board, file);
        if (attackers & moves::vert_slider_moves[square][rotated_occ])
            return true;

        // Diagonal sliders
        attackers = queens[other_side] | bishops[other_side];
        rotated_occ = project_occupancy_from_a1h8_to6bit(whole_board, rank, file);
        if (attackers & moves::diag_moves_a1h8[square][rotated_occ])
            return true;

        rotated_occ = project_occupancy_from_a8h1_to6bit(whole_board, rank, file);
        if (attackers & moves::diag_moves_a8h1[square][rotated_occ])
            return true;

        return false;
    }

    bool Position::detect_check(Side king_side) const
    {
        Square king_square = get_first_occ_square(kings[king_side]);
        return square_attacked(king_square, king_side);
    }

    bool Position::make_move(Move move)
    {
        const Piece    moving_piece             = move.get_piece();
        const Piece    captured_piece           = move.get_captured_piece();
        const Square   source                   = move.get_source();
        const Square   dest                     = move.get_destination();
        const Side     side                     = get_piece_side(moving_piece);
        const RankFile source_rank              = square_to_rank(source);
        const RankFile dest_rank                = square_to_rank(dest);
        const Bitboard source_bitboard          = util::one << source;
        const Bitboard dest_bitboard            = util::one << dest;
        const Bitboard source_and_dest_bitboard = source_bitboard | dest_bitboard;

        switch (moving_piece)
        {
        case pieces::WHITE_PAWN:
        case pieces::BLACK_PAWN:

            move_common_first_stage(moving_piece, side, source, dest, source_and_dest_bitboard);

            // EP square is on third/sixth rank, if applicable:
            ep_target_square = abs(dest_rank - source_rank) == 2 ? source + sides::NEXT_RANK_OFFSET[side] : squares::NO_SQUARE;

            if (move.get_en_passant() != pieces::NONE)
            {
                Bitboard pawn_captured_ep_mask    = util::one << (dest - sides::NEXT_RANK_OFFSET[side]);  // the square below or above dest. Less shift for white, more for black
                pawns[swap_side(side)]           ^= pawn_captured_ep_mask;
                sides[swap_side(side)]           ^= pawn_captured_ep_mask;
                squares[dest - sides::NEXT_RANK_OFFSET[side]] = pieces::NONE;
                whole_board                      ^= (source_and_dest_bitboard | pawn_captured_ep_mask);

                material -= evals::PAWN_CAPTURE_VALUES[side];
            }
            else
            {
                move_common_second_stage(captured_piece, side, dest_bitboard, source_bitboard, source_and_dest_bitboard);

                Piece promotion_piece = move.get_promotion_piece();
                if (promotion_piece != pieces::NONE)
                {
                    pawns[side]                 ^= dest_bitboard; // Turn back off pawn at dest
                    piece_bbs[promotion_piece]  ^= dest_bitboard; // Turn on new piece at dest
                    squares[dest]               = promotion_piece;

                    material -= evals::PIECE_CAPTURE_VALUES[promotion_piece]; // "-=", as we're adding it
                    material += evals::PAWN_CAPTURE_VALUES[side]; // we "lost" the pawn.
                }
            }

            fifty_move_count = 0;

            break;

        case pieces::WHITE_KING:
        case pieces::BLACK_KING:

            if (move.get_castling() != pieces::NONE)
            {
                assert(!captured_piece);

                // canna castle through check
                if (detect_check(side))
                    return false;

                Bitboard rook_mask;

                if (dest == squares::g1)
                {
                    if (square_attacked(squares::f1, sides::white))
                        return false;

                    rook_mask = squarebits::h1 | squarebits::f1;
                    squares[squares::h1] = pieces::NONE;
                    squares[squares::f1] = pieces::WHITE_ROOK;
                }
                else if (dest == squares::c1)
                {
                    if (square_attacked(squares::d1, sides::white))
                        return false;

                    rook_mask = squarebits::a1 | squarebits::d1;
                    squares[squares::a1] = pieces::NONE;
                    squares[squares::d1] = pieces::WHITE_ROOK;
                }
                else if (dest == squares::g8)
                {
                    if (square_attacked(squares::f8, sides::black))
                        return false;

                    rook_mask = squarebits::h8 | squarebits::f8;
                    squares[squares::h8] = pieces::NONE;
                    squares[squares::f8] = pieces::BLACK_ROOK;
                }
                else if (dest == squares::c8)
                {
                    if (square_attacked(squares::d8, sides::black))
                        return false;

                    rook_mask = squarebits::a8 | squarebits::d8;
                    squares[squares::a8] = pieces::NONE;
                    squares[squares::d8] = pieces::BLACK_ROOK;
                }

                move_common_first_stage(moving_piece, side, source, dest, source_and_dest_bitboard);
                move_common_second_stage(captured_piece, side, dest_bitboard, source_bitboard, source_and_dest_bitboard);

                castling_rights &= ~sides::CASTLING_RIGHTS_ANY[side];

                rooks[side] ^= rook_mask;
                sides[side] ^= rook_mask;
                whole_board ^= rook_mask;
            }
            else
            {
                move_common_first_stage(moving_piece, side, source, dest, source_and_dest_bitboard);
                move_common_second_stage(captured_piece, side, dest_bitboard, source_bitboard, source_and_dest_bitboard);

                castling_rights &= ~sides::CASTLING_RIGHTS_ANY[side];
            }
            break;

        case pieces::WHITE_ROOK:
        case pieces::BLACK_ROOK:

            move_common_first_stage(moving_piece, side, source, dest, source_and_dest_bitboard);
            move_common_second_stage(captured_piece, side, dest_bitboard, source_bitboard, source_and_dest_bitboard);

            // See notes in move_common_second_stage(), capture branch, and make_castling_mask(), for this logic to avoid branching 
            // when removing castling rights.
            // The difference is that source_bitboard is used here, rather than dest_bitboard in move_common_second_stage(); 
            // note that it's ok to do all four cases, as if e.g. we're moving from h8 as a white rook, black doesn't have castling 
            // rights there anyway; turning them off again is OK.
            castling_rights &= ~make_castling_mask(source_bitboard); // If the bits in the mask are set, this turns off the relevant bits.

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

        // If we're in check, it wasn't legal
        return !detect_check(side);
    }
}