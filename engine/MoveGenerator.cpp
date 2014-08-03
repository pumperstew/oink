#include "MoveGenerator.hpp"
#include "Position.hpp"
#include "BasicOperations.hpp"

//#define OINK_MOVEGEN_DIAGNOSTICS

#ifdef OINK_MOVEGEN_DIAGNOSTICS
    #include <display/ConsoleDisplay.hpp>
#endif

namespace chess
{
	static void generate_moves_from_destinations(Bitboard destinations, Move move, MoveVector &moves, const Position &position)
	{
        Square dest_square;
		while (destinations)
        {
            destinations = get_and_clear_first_occ_square(destinations, &dest_square);
			move.set_destination(dest_square);
			move.set_captured_piece(position.squares[dest_square]);
            moves.push_back(move);
        }
	}

    static void generate_moves_from_destinations_with_promotion(Bitboard destinations, Move move, MoveVector &moves, const Position &position, Side side)
	{
        Square dest_square;
		while (destinations)
        {
			destinations = get_and_clear_first_occ_square(destinations, &dest_square);
			move.set_destination(dest_square);
			move.set_captured_piece(position.squares[dest_square]);
			
			move.set_promotion_piece(pieces::QUEENS[side]);
			moves.push_back(move);
			
            move.set_promotion_piece(pieces::ROOKS[side]);
			moves.push_back(move);
			
            move.set_promotion_piece(pieces::KNIGHTS[side]);
			moves.push_back(move);

			move.set_promotion_piece(pieces::BISHOPS[side]);
            moves.push_back(move);
        }
	}

    static void generate_ep_move(Bitboard destinations, Move move, MoveVector &moves, const Position &position, Side side)
	{
        if (destinations)
        {
            Square dest_square;
		    destinations = get_and_clear_first_occ_square(destinations, &dest_square);
		    move.set_destination(dest_square);
		    move.set_captured_piece(pieces::PAWNS[swap_side(side)]);
            move.set_en_passant(pieces::PAWNS[side]);
            moves.push_back(move);
        }

        // There is a maximum of one EP capture available in a given position for a given pawn.
        assert(!destinations);
	}

	void generate_king_moves(MoveVector &moves, const Position &position, Side side)
    {
		Move move;
		move.set_piece(pieces::KINGS[side]);

		Bitboard king = position.kings[side];
        assert(king);

		Square square = get_first_occ_square(king);
		move.set_source(square);

        Bitboard destinations = moves::king_moves[square] & ~position.sides[side];
		generate_moves_from_destinations(destinations, move, moves, position);

        if (side == sides::white && square == squares::e1)
        {
            if (position.castling_rights & sides::CASTLING_RIGHTS_WHITE_KINGSIDE)
            {
                assert(position.squares[squares::h1] == pieces::WHITE_ROOK);

                if (!(position.whole_board & moves::white_kingside_castling_mask))
                {
                    move.set_destination(squares::g1);
                    move.set_castling(pieces::WHITE_KING);
                    moves.push_back(move);
                }
            }

            if (position.castling_rights & sides::CASTLING_RIGHTS_WHITE_QUEENSIDE)
            {
                assert(position.squares[squares::a1] == pieces::WHITE_ROOK);

                if (!(position.whole_board & moves::white_queenside_castling_mask))
                {
                    move.set_destination(squares::c1);
                    move.set_castling(pieces::WHITE_KING);
                    moves.push_back(move);
                }
            }
        }
        else if (side == sides::black && square == squares::e8)
        {
            if (position.castling_rights & sides::CASTLING_RIGHTS_BLACK_KINGSIDE)
            {
                assert(position.squares[squares::h8] == pieces::BLACK_ROOK);

                if (!(position.whole_board & moves::black_kingside_castling_mask))
                {
                    move.set_destination(squares::g8);
                    move.set_castling(pieces::BLACK_KING);
                    moves.push_back(move);
                }
            }

            if (position.castling_rights & sides::CASTLING_RIGHTS_BLACK_QUEENSIDE)
            {
                assert(position.squares[squares::a8] == pieces::BLACK_ROOK);

                if (!(position.whole_board & moves::black_queenside_castling_mask))
                {
                    move.set_destination(squares::c8);
                    move.set_castling(pieces::BLACK_KING);
                    moves.push_back(move);
                }
            }
        }
    }

    void generate_knight_moves(MoveVector &moves, const Position &position, Side side)
    {
		Move move;
		move.set_piece(pieces::KNIGHTS[side]);

        Bitboard knights        = position.knights[side];
        Bitboard not_other_king = ~position.kings[swap_side(side)];
        Bitboard not_my_side    = ~position.sides[side];

        Square source_sq;
        while (knights)
        {
            knights = get_and_clear_first_occ_square(knights, &source_sq);
			move.set_source(source_sq);

            Bitboard destinations = moves::knight_moves[source_sq] & not_my_side & not_other_king;
			generate_moves_from_destinations(destinations, move, moves, position);
        }
    }

	void generate_pawn_moves(MoveVector &moves, const Position &position, Side side)
    {
		Move move;
		move.set_piece(pieces::PAWNS[side]);

        Bitboard pawns          = position.pawns[side];
        Bitboard other_side     = position.sides[swap_side(side)];
        Bitboard not_other_king = ~position.kings[swap_side(side)];

        Square source_sq;
        while (pawns)
        {
            pawns = get_and_clear_first_occ_square(pawns, &source_sq);
			move.set_source(source_sq);

			RankFile rank = square_to_rank(source_sq);
			Bitboard whole_board = position.whole_board;
			if (rank == sides::STARTING_PAWN_RANKS[side])
            {
				whole_board = exclude_fourth_or_fifth_rank_if_third_or_sixth_occupied(whole_board, side);
            }
			// Since for pawns we're doing captures separately, we use whole_board here. We also exclude 4th(5th) rank if 3rd(6th) is occupied.
            // Normal moves and promotions.
			Bitboard destinations = moves::pawn_moves[side][source_sq] & ~whole_board;

            // Normal captures
			destinations |= moves::pawn_captures[side][source_sq] & other_side & not_other_king;
            bool promoting = (rank == sides::ABOUT_TO_PROMOTE[side]); //if we're on the 7th or 2nd ranks, we're gonna promote.
			
            if (promoting)
            {
                generate_moves_from_destinations_with_promotion(destinations, move, moves, position, side);
            }
            else
            {
                generate_moves_from_destinations(destinations, move, moves, position);
                
                // EP captures are never promotions.
                // EP captures: ep_target_square is set if there is a valid target for an EP capture. 
                // The actual captured pawn will have to be fixed up later. 
                // OINK_TODO: cleaner way?
                Bitboard ep_bb = (position.ep_target_square == squares::NO_SQUARE) ? util::nil : util::one << position.ep_target_square;
                destinations = moves::pawn_captures[side][source_sq] & ep_bb;
                // There must be a maximum of one destination.
                generate_ep_move(destinations, move, moves, position, side);
            }
        }
    }

	static void generate_rank_file_slider_moves(MoveVector &moves, const Position &position, Side side, Move &move, Bitboard moving_piece_bitboard)
	{
        Bitboard not_other_king = ~position.kings[swap_side(side)];
        Bitboard not_my_side    = ~position.sides[side];

		while (moving_piece_bitboard)
		{
            Square source_sq;
            moving_piece_bitboard = get_and_clear_first_occ_square(moving_piece_bitboard, &source_sq);
			move.set_source(source_sq);
			
            RankFile rank, file;
			square_to_rank_file(source_sq, rank, file);

			Bitboard rank_occ_6bit = get_6bit_rank_occupancy(position.whole_board, rank);
			Bitboard file_occ_6bit = project_occupancy_from_file_to6bit(position.whole_board, file);
			Bitboard destinations  = (moves::horiz_slider_moves[source_sq][rank_occ_6bit] | moves::vert_slider_moves[source_sq][file_occ_6bit])
									  & not_my_side
                                      & not_other_king;

			generate_moves_from_destinations(destinations, move, moves, position);
		}
	}

    void generate_rook_moves(MoveVector &moves, const Position &position, Side side)
    {
		Move move;
		move.set_piece(pieces::ROOKS[side]);
		generate_rank_file_slider_moves(moves, position, side, move, position.rooks[side]);
    }

	static void generate_diagonal_slider_moves(MoveVector &moves, const Position &position, Side side, Move &move, Bitboard moving_piece_bitboard)
	{
        Bitboard not_other_king = ~position.kings[swap_side(side)];
        Bitboard not_my_side    = ~position.sides[side];

		while (moving_piece_bitboard)
		{
            Square source_sq;
			moving_piece_bitboard = get_and_clear_first_occ_square(moving_piece_bitboard, &source_sq);
			move.set_source(source_sq);

#ifdef OINK_MOVEGEN_DIAGNOSTICS
            print_bitboards(
            {
                { position.whole_board,                     "whole board"                  },
                { moves::sixbit_diag_masks_a1h8[source_sq], "a1h8 diagmasks for source_sq" },
                { moves::sixbit_diag_masks_a8h1[source_sq], "a8h1 diagmasks for source_sq" }
            },
            source_sq);
#endif
            RankFile rank, file;
            square_to_rank_file(source_sq, rank, file);

			Bitboard projected_a1h8_occ_6bit = project_occupancy_from_a1h8_to6bit(position.whole_board, rank, file);
			Bitboard projected_a8h1_occ_6bit = project_occupancy_from_a8h1_to6bit(position.whole_board, rank, file);
			Bitboard destinations = (moves::diag_moves_a1h8[source_sq][projected_a1h8_occ_6bit] | moves::diag_moves_a8h1[source_sq][projected_a8h1_occ_6bit])
				                     & not_my_side
                                     & not_other_king;

            assert(projected_a1h8_occ_6bit <= util::FULL_6BITOCC);
			assert(projected_a8h1_occ_6bit <= util::FULL_6BITOCC);

#ifdef OINK_MOVEGEN_DIAGNOSTICS
            print_bitboard(projected_a1h8_occ_6bit, "projected_a1h8_occ");
            print_bitboard(projected_a8h1_occ_6bit, "projected_a8h1_occ");
			print_bitboards(
            {
                { moves::diag_moves_a1h8[source_sq][projected_a1h8_occ_6bit], "a1h8 moves"   },
                { moves::diag_moves_a8h1[source_sq][projected_a8h1_occ_6bit], "a8h1 moves"   },
                { destinations,                                               "destinations" },
                { position.sides[sides::black],                               "black"        }
            },
            source_sq);
#endif
			generate_moves_from_destinations(destinations, move, moves, position);
		}
	}

    void generate_bishop_moves(MoveVector &moves, const Position &position, Side side)
    {
		Move move;
		move.set_piece(pieces::BISHOPS[side]);
		generate_diagonal_slider_moves(moves, position, side, move, position.bishops[side]);
    }

	void generate_queen_moves(MoveVector &moves, const Position &position, Side side)
	{
		Move rf_move;
		rf_move.set_piece(pieces::QUEENS[side]);
		generate_rank_file_slider_moves(moves, position, side, rf_move, position.queens[side]);

		Move diag_move;
		diag_move.set_piece(pieces::QUEENS[side]);
		generate_diagonal_slider_moves(moves, position, side, diag_move, position.queens[side]);
	}
    
	void generate_all_moves(MoveVector &moves, const Position &position, Side side)
    {
        generate_pawn_moves(moves,   position, side);
        generate_queen_moves(moves,  position, side);
        generate_bishop_moves(moves, position, side);
        generate_rook_moves(moves,   position, side);
        generate_knight_moves(moves, position, side);
        generate_king_moves(moves,   position, side);
    }
}