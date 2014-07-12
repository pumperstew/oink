#include "ChessConstants.hpp"
#include "BasicOperations.hpp"

#ifdef OINK_MOVEGEN_DIAGNOSTICS
    #include "Display.hpp"
#endif

#include <cassert>

namespace chess
{
	using namespace util;
	using namespace moves;

	namespace pieces
	{
		char symbols[15];
	}

    namespace moves
    {
        Bitboard horiz_slider_moves[util::NUM_SQUARES][util::FULL_6BITOCC + 1];
        Bitboard vert_slider_moves[util::NUM_SQUARES][util::FULL_6BITOCC + 1];
        Bitboard sixbit_file_masks[util::BOARD_SIZE];
        Bitboard eightbit_rank_masks[util::BOARD_SIZE];
        Bitboard knight_moves[util::NUM_SQUARES];
        Bitboard diag_moves_a1h8[util::NUM_SQUARES][util::FULL_6BITOCC + 1];
		Bitboard diag_moves_a8h1[util::NUM_SQUARES][util::FULL_6BITOCC + 1];
        Bitboard sixbit_diag_masks_a1h8[NUM_DIAGS];
        Bitboard sixbit_diag_masks_a8h1[NUM_DIAGS];
    }

    const int A1H8_SELECT  = 0;
    const int A8H1_SELECT  = 1;
    const int RANK_SELECT  = 0;
    const int FILE_SELECT  = 1;

    static void init_piece_symbols()
    {
        pieces::symbols[pieces::NONE]         = '*';
		pieces::symbols[pieces::WHITE_PAWN]   = 'P';
		pieces::symbols[pieces::WHITE_KING]   = 'K';
		pieces::symbols[pieces::WHITE_KNIGHT] = 'N';
		pieces::symbols[pieces::WHITE_BISHOP] = 'B';
		pieces::symbols[pieces::WHITE_ROOK]   = 'R';
		pieces::symbols[pieces::WHITE_QUEEN]  = 'Q';
		pieces::symbols[pieces::BLACK_PAWN]   = 'p';
		pieces::symbols[pieces::BLACK_KING]   = 'k';
		pieces::symbols[pieces::BLACK_KNIGHT] = 'n';
		pieces::symbols[pieces::BLACK_BISHOP] = 'b';
		pieces::symbols[pieces::BLACK_ROOK]   = 'r';
		pieces::symbols[pieces::BLACK_QUEEN]  = 'q';
    }

	static void generate_rank_file_masks()
	{
		for (RankFile i = 0; i < util::BOARD_SIZE; ++i)  //rank or file loop
        {
            sixbit_file_masks[i] = util::nil;
            eightbit_rank_masks[i] = util::fullrank << calc_rank_shift(i);

            for (RankFile j = 1; j < util::BOARD_SIZE - 1; ++j) //rank loop for file masks
            { 
                sixbit_file_masks[i] |= util::one << rank_file_to_square(j, i);
            }
        }
	}

    static void generate_diag_masks()
    {
        for (RankFile i = 0; i < moves::NUM_DIAGS; ++i)
        {
            // Starts at top left and walks down and around.
            RankFile diag_rank_start = 7 - i >= 0 ? 7 - i : 0;
            RankFile diag_file_start = 7 - i >= 0 ? 0     : -(7 - i);

            RankFile diag_length     = i < 8 ? i + 1 : moves::NUM_DIAGS - i;

            Bitboard diag_start_bit = util::one << rank_file_to_square(diag_rank_start, diag_file_start);
            Bitboard mask = util::nil;
            for (int offset = 1; offset < diag_length - 1; ++offset)
            {
                RankFile shift = 9 * offset;
                mask |= (diag_start_bit << shift);
            }

            sixbit_diag_masks_a1h8[i] = mask;
        }

        for (RankFile i = 0; i < moves::NUM_DIAGS; ++i)
        {
            // Starts at bottom left and walks up and around:
            // a1, a2b1, a3b2c1, ..., a8h1, ..., g8h7, h8
            RankFile diag_rank_start = 7 - i >= 0 ? i : 7;
            RankFile diag_file_start = 7 - i >= 0 ? 0 : -(7 - i);

            RankFile diag_length     = i < 8 ? i + 1 : moves::NUM_DIAGS - i;

            Bitboard diag_start_bit = util::one << rank_file_to_square(diag_rank_start, diag_file_start);
            Bitboard mask = util::nil;
            for (int offset = 1; offset < diag_length - 1; ++offset)
            {
                RankFile shift = 7 * offset;
                mask |= (diag_start_bit >> shift);
            }

            sixbit_diag_masks_a8h1[i] = mask;
        }
    }

	static void generate_knight_moves(Square square, RankFile rank, RankFile file)
	{
        Bitboard set_on_this_square = util::one << square;

		knight_moves[square] = 0;
        if (rank >= 2) //2 down, 1 left; 2 down, 1 right
            knight_moves[square] |= ((set_on_this_square >> 17) | (set_on_this_square >> 15)) & eightbit_rank_masks[rank-2];
        if (rank >= 1) //1 down, 2 left; 1 down, 2 right
            knight_moves[square] |= ((set_on_this_square >> 10) | (set_on_this_square >> 6))  & eightbit_rank_masks[rank-1];
        if (rank <= 6) //1 up, 2 left; 1 up, 2 right                         
            knight_moves[square] |= ((set_on_this_square << 6)  | (set_on_this_square << 10)) & eightbit_rank_masks[rank+1];
        if (rank <= 5) //2 up, 1 left; 2 up, 1 right
            knight_moves[square] |= ((set_on_this_square << 15) | (set_on_this_square << 17)) & eightbit_rank_masks[rank+2];
	}

	// Turn on bits to the right of the given square, which is assumed to be on the given rank.
	static Bitboard turn_on_bits_to_right(Bitboard square_bit, RankFile rank)
	{
        return ((square_bit << 1) | (square_bit << 2) | 
                (square_bit << 3) | (square_bit << 4) | 
                (square_bit << 5) | (square_bit << 6) | (square_bit << 7)) & eightbit_rank_masks[rank];
	}

    // Turn on bits to the left of the given square, which is assumed to be on the given rank.
	static Bitboard turn_on_bits_to_left(Bitboard square_bit, RankFile rank)
	{
        return ((square_bit >> 1) | (square_bit >> 2) | 
                (square_bit >> 3) | (square_bit >> 4) | 
                (square_bit >> 5) | (square_bit >> 6) | (square_bit >> 7)) & eightbit_rank_masks[rank];
	}

    static void turn_on_bits_up(Bitboard square_bit, RankFile rank, Bitboard &up, Bitboard &up_right, Bitboard &up_left)
    {
        // Generate masks with bits 1 on both diagonals intersecting the current square.
        // Also get masks for bits_on_to_up in same loop.
        for (RankFile offset = 1; offset < util::BOARD_SIZE - rank; ++offset)
		{
            up |= (square_bit << rank_file_to_square(offset, 0)); //<< 8, << 16, << 24, ... (shift up ranks)
				
			//Up and right: take bit, shift up along the diagonal (<< 9, << 18, << 27, ...), and mask off other ranks.
			//The shifts are non-intuitive, because to go *right*, we shift 9 (not 7), because a1 is LSB, so going 
			//right on the board means shifting *left*. Basically everything is mirrored along the y-axis in bit-repro.
			//(final masking step is necessary because we loop further than we need here, because we generate the up moves too)
			up_right |= (square_bit << rank_file_to_square(offset, offset)) & eightbit_rank_masks[rank + offset];
			//Up and left. Same as a1h8, but file offset is negative, so shifts are (<< 7, << 14, << 21, ...)
            up_left |= (square_bit << rank_file_to_square(offset, -offset)) & eightbit_rank_masks[rank + offset];
        }
    }

    static void turn_on_bits_down(Bitboard square_bit, RankFile rank, Bitboard &down, Bitboard &down_right, Bitboard &down_left)
    {
        for (RankFile offset = 1; offset <= rank; ++offset)
		{
            down |= (square_bit >> (offset*util::BOARD_SIZE)); //>> 8, >> 16, >> 24, ... (shift down ranks)
			
            down_left |= (square_bit >> rank_file_to_square(offset, offset)) & eightbit_rank_masks[rank - offset];
			
            down_right |= (square_bit >> rank_file_to_square(offset, -offset)) & eightbit_rank_masks[rank - offset];
        }
    }

    static Bitboard sixbit_to_eightbit_occ(Bitboard sixbit_rank_occ)
    {
        return (sixbit_rank_occ << 1)          |    // shift up to make room
               ((sixbit_rank_occ & 0x20) << 1) |    // copy last bit to last bit of shifted version
               (sixbit_rank_occ & 1);               // copy second bit to first bit of shifted version
    }

	static Bitboard generate_right_slider_moves(Bitboard sixbit_rank_occ, Bitboard bits_on_to_right)
	{
        // Build a full eight-bit occupancy. The first and last files are determined by their neighbours on this rank.
        // This isn't necessary for right moves (I think), but it is(?) for the left ones..
        Bitboard eightbit_rank_occ = sixbit_to_eightbit_occ(sixbit_rank_occ);

		Bitboard moves = eightbit_rank_occ & bits_on_to_right; // Ones where pieces sit, to right of square.
        moves = ( 
            ( (moves << 1) | (moves << 2) | (moves << 3) // First fill in 1s to right of first blocking piece..
            | (moves << 4) | (moves << 5) | (moves << 6) )
            & bits_on_to_right)  // ...eliminate overflows to next rank
            ^ bits_on_to_right;  // ...and finally flip to get squares we *can* go to
		return moves;
	}

	static Bitboard generate_left_slider_moves(Bitboard sixbit_rank_occ, Bitboard bits_on_to_left)
	{
        Bitboard eightbit_rank_occ = sixbit_to_eightbit_occ(sixbit_rank_occ);

		Bitboard moves = eightbit_rank_occ & bits_on_to_left;
        moves = (
            ( (moves >> 1) | (moves >> 2) | (moves >> 3)
            | (moves >> 4) | (moves >> 5) | (moves >> 6) )
            & bits_on_to_left)
            ^ bits_on_to_left;
		return moves;
	}

    /*
    Rotate a 6-bit occupancy up onto the vertical, extend it to an 8-bit occupancy, and reverse the bits.
    This reversal is done because when projecting an occupancy down during movegen we will produce a bit-reversed pattern.
    Therefore we want to index the moves arrays in a bit-reversed manner -- so here we need to produce an occupancy for 
    the reversed state to generate moves from.
    */
	static Bitboard rotate_occupancy_to_vertical_reverse_and_extend(Bitboard sixbit_rank_occ /* in low six bits */, RankFile desired_file)
	{
        Bitboard eightbit_rank_occ = sixbit_to_eightbit_occ(sixbit_rank_occ);

		Bitboard rotated_occ = util::nil;
        // Reverse bits by iterating backwards.
        for (RankFile rank = util::BOARD_SIZE - 1; rank >= 0; --rank)
		{
            Bitboard thisbit = eightbit_rank_occ & util::one;
            eightbit_rank_occ >>= 1;

            rotated_occ |= thisbit << rank_file_to_square(rank, desired_file); // shift back up to correct point.
        }

		return rotated_occ;
	}

	static Bitboard generate_up_slider_moves(Bitboard eightbit_file_occupancy, Bitboard bits_on_to_up)
	{
		Bitboard moves = eightbit_file_occupancy & bits_on_to_up;
        moves = (
            ( (moves << 8)  | (moves << 16) | (moves << 24)
            | (moves << 32) | (moves << 40) | (moves << 48) )
            & bits_on_to_up)
            ^ bits_on_to_up;
		return moves;
	}

	static Bitboard generate_down_slider_moves(Bitboard eightbit_file_occupancy, Bitboard bits_on_to_down)
	{
		Bitboard moves = eightbit_file_occupancy & bits_on_to_down;
        moves = (
            ( (moves >> 8)  | (moves >> 16) | (moves >> 24)
            | (moves >> 32) | (moves >> 40) | (moves >> 48) )
            & bits_on_to_down)
            ^ bits_on_to_down;
		return moves;
	}

    /*
    Rotate an 8-bit occupancy up onto an a1h8-oriented diagonal.
    Travel up and right from the start of the diagonal (the bottomleftmost square of it)
    */
	static Bitboard rotate_occupancy_onto_a1h8_diagonal_and_extend(Bitboard sixbit_rank_occ /* in low six bits */, RankFile diag_start[2][2], RankFile diag_length[2])
	{
        Bitboard eightbit_rank_occ = sixbit_to_eightbit_occ(sixbit_rank_occ);

        assert(eightbit_rank_occ == (eightbit_rank_occ & 0xff)); // low eight bits only

		Bitboard rotated_occ = util::nil;
        RankFile start_rank = diag_start[A1H8_SELECT][RANK_SELECT];
        RankFile start_file = diag_start[A1H8_SELECT][FILE_SELECT];

        for (RankFile distance = 0; (distance < diag_length[A1H8_SELECT]) && eightbit_rank_occ; ++distance)
		{
            Bitboard thisbit = eightbit_rank_occ & util::one;
            eightbit_rank_occ >>= 1;

            rotated_occ |= thisbit << rank_file_to_square(start_rank + distance, start_file + distance);
        }

		return rotated_occ;
	}

	static Bitboard generate_a1h8_diagonal_moves(Bitboard rotated_occ_a1h8, Bitboard bits_set_up_and_right, Bitboard bits_set_down_and_left)
	{
		Bitboard up_right_moves  = rotated_occ_a1h8 & bits_set_up_and_right;
		Bitboard down_left_moves = rotated_occ_a1h8 & bits_set_down_and_left;

		up_right_moves = (
            ( (up_right_moves << 9) |  (up_right_moves << 18) | (up_right_moves << 27) 
            | (up_right_moves << 36) | (up_right_moves << 45) | (up_right_moves << 54) )
            & bits_set_up_and_right)
            ^ bits_set_up_and_right;

		down_left_moves = (
            ( (down_left_moves >> 9) |  (down_left_moves >> 18) | (down_left_moves >> 27) 
            | (down_left_moves >> 36) | (down_left_moves >> 45) | (down_left_moves >> 54) )
            & bits_set_down_and_left)
            ^ bits_set_down_and_left;

		return up_right_moves | down_left_moves;
	}

    // For example, an occupancy of 0x2 for the diagonal starting at a4 means:
    // where @ == a4, ^ == occupied.
    // * * * * * * * *
    // * * * * * * * *
    // * * * * * * * *
    // * * * * * * * *
    // @ * * * * * * *
    // * ^ * * * * * *
    // * * * * * * * *
    // * * * * * * * *
	static Bitboard rotate_occupancy_onto_a8h1_diagonal_and_extend(Bitboard sixbit_rank_occ /* in low six bits */, RankFile diag_start[2][2], RankFile diag_length[2])
	{
        Bitboard eightbit_rank_occ = sixbit_to_eightbit_occ(sixbit_rank_occ);

        assert(eightbit_rank_occ == (eightbit_rank_occ & 0xff)); // low eight bits only

		Bitboard rotated_occ = util::nil;
        RankFile start_rank = diag_start[A8H1_SELECT][RANK_SELECT];
        RankFile start_file = diag_start[A8H1_SELECT][FILE_SELECT];

		for (RankFile distance = 0; (distance < diag_length[A8H1_SELECT]) && eightbit_rank_occ; ++distance)
		{
			Bitboard thisbit = eightbit_rank_occ & util::one;
            eightbit_rank_occ >>= 1;
        
            // In the above example, imagine we're dealing with b3, so distance == 1, thisbit == 1.
            // Shift should be equal to the index of b3, i.e. 17.
            // start_rank = 3. start_file = 0 => RFTI(2, 1) = 17. Yay.
			rotated_occ |= thisbit << rank_file_to_square(start_rank - distance, start_file + distance);
		}

		return rotated_occ;
	}

	static Bitboard generate_a8h1_diagonal_moves(Bitboard rotated_occ_a8h1, Bitboard bits_set_up_and_left, Bitboard bits_set_down_and_right)
	{
		Bitboard up_left_moves    = rotated_occ_a8h1 & bits_set_up_and_left;
		Bitboard down_right_moves = rotated_occ_a8h1 & bits_set_down_and_right;

		up_left_moves = (
            ( (up_left_moves << 7)  |  (up_left_moves << 14) | (up_left_moves << 21) 
            | (up_left_moves << 28) | (up_left_moves << 35)  | (up_left_moves << 42) )
            & bits_set_up_and_left)
            ^ bits_set_up_and_left;

		down_right_moves = (
            ( (down_right_moves >> 7)  |  (down_right_moves >> 14) | (down_right_moves >> 21) 
            | (down_right_moves >> 28) | (down_right_moves >> 35)  | (down_right_moves >> 42) )
            & bits_set_down_and_right)
            ^ bits_set_down_and_right;

		return up_left_moves | down_right_moves;
	}

	static void generate_diagonal_vector(RankFile rank, RankFile file, RankFile diag_start[2][2] /* out */, RankFile diag_length[2] /* out */)
	{
        /*
        1. For the a1-h8 direction, there are two cases are differentiated by whether rank > file. 
        
        Consider '%' (e6). This is in the upper left diagonally cut half.
        rank = 5, file = 4. 
        a) Intercept on "a" file comes at rank (5 - 4) = 1   => diagonalStart[a1h8_direction][rankDiagIndex] = rank - file = 1.
        b) Intercept is with "a" file,                       => diagonalStart[a1h8_direction][fileDiagIndex] = 0.
        c) Length of diagonal is limited by intercept with "a" file. It's equal to 8 - (rank - file).
        
        Consider '@' (e3). This is in the lower right.
        rank = 2, file = 4.
        a) We intercept at rank zero.
        b) The above intercept is at file (file - rank) == 2 (c1).
        c) Length is limited by the file intercept with rank zero, so it's 8 - 2 == 6.
        
        2. a8-h1 direction. The other diagonal is the differentiator in this case: rank + file >= 8.
        e6 is an example of this (rank + file == 9).
        a) Diagonal starts on eighth rank. Since r + f = const C, solve for r = 7. => 7 + f = C => f = r0 + f0 - 7.
        b) Length is then 8 - the file intercept above.

        Finally consider e3 again (rank + file == 6).
        a) Intercept at "a" file.
        b) Rank intercept will be r = r0 + f0 - 0
        c) Length is file intercept + 1 == 7.
        */

        // * * * * * * * *
        // * * * * * * * *
        // * * * * % * * *
        // * * * * * * * *
        // * * * * * * * *
        // * * * * @ * * &
        // & * * * * * * *
        // * * & * * * & *

        // a1-h8
		if (rank >= file)
		{
            diag_start[A1H8_SELECT][RANK_SELECT] = rank - file;
            diag_start[A1H8_SELECT][FILE_SELECT] = files::a;          
            diag_length[A1H8_SELECT]             = util::BOARD_SIZE - (rank - file);
        } 
		else
		{
            diag_start[A1H8_SELECT][RANK_SELECT] = ranks::first;
            diag_start[A1H8_SELECT][FILE_SELECT] = file - rank;
            diag_length[A1H8_SELECT]             = util::BOARD_SIZE - (file - rank);
        }

        // a8-h1
        if (rank + file >= util::BOARD_SIZE)
        {
            RankFile file_intercept_at_eighth = rank + file - (util::BOARD_SIZE - 1);
            diag_start[A8H1_SELECT][RANK_SELECT] = ranks::eighth;
            diag_start[A8H1_SELECT][FILE_SELECT] = file_intercept_at_eighth;
            diag_length[A8H1_SELECT]             = util::BOARD_SIZE - file_intercept_at_eighth;
        }
        else
        {
            diag_start[A8H1_SELECT][RANK_SELECT] = file + rank;
            diag_start[A8H1_SELECT][FILE_SELECT] = files::a;
            diag_length[A8H1_SELECT]             = file + rank; 
        }

        assert(diag_start[A1H8_SELECT][RANK_SELECT]  >= 0);
        assert(diag_start[A1H8_SELECT][FILE_SELECT]  >= 0);
        assert(diag_start[A8H1_SELECT][RANK_SELECT]  >= 0);
        assert(diag_start[A8H1_SELECT][FILE_SELECT] >= 0);
        assert(diag_length[A1H8_SELECT] >= 0 && diag_length[A1H8_SELECT] <= util::BOARD_SIZE);
        assert(diag_length[A8H1_SELECT] >= 0 && diag_length[A8H1_SELECT] <= util::BOARD_SIZE);
	}

    void constants_initialize()
    {
        init_piece_symbols();

        generate_rank_file_masks();

        generate_diag_masks();

        //===================== Generate moves=======================
        for (int i = 0; i < util::NUM_SQUARES; ++i) //loop over all squares
		{ 
            Bitboard cur_square_bit = util::one << i;
            RankFile rank, file;
            square_to_rank_file(i, rank, file);
           
			generate_knight_moves(i, rank, file);

            //============== Generate slider moves (horiz, vert, diag) ==========

			//Generate masks with bits on to the right and to the left of the current square, on its rank.
            const Bitboard bits_on_to_right = turn_on_bits_to_right(cur_square_bit, rank);
			const Bitboard bits_on_to_left  = turn_on_bits_to_left(cur_square_bit, rank);

			Bitboard bits_on_to_up           = util::nil;
			Bitboard bits_on_to_up_and_right = util::nil;
			Bitboard bits_on_to_up_and_left  = util::nil;
            turn_on_bits_up(cur_square_bit, rank, bits_on_to_up, bits_on_to_up_and_right, bits_on_to_up_and_left);

            Bitboard bits_on_to_down           = util::nil;
            Bitboard bits_on_to_down_and_right = util::nil;
            Bitboard bits_on_to_down_and_left  = util::nil;
            turn_on_bits_down(cur_square_bit, rank, bits_on_to_down, bits_on_to_down_and_right, bits_on_to_down_and_left);

            RankFile diag_start[2][2]; // {a1h8, a8h1}{rank, file}
            RankFile diag_length[2];   // {a1h8, a8h1}
		
			generate_diagonal_vector(rank, file, diag_start, diag_length);

            // Loop over all reduced 6-bit occupancy permutations on ranks/files/diagonals:
            for (Bitboard sixbit_occ = 0; sixbit_occ <= util::FULL_6BITOCC; ++sixbit_occ)
            {
                // OINK_TODO: can we use just 5 bits? The sixth is by definition set by the square we're considering.
                // But it might be one of the edge bits..

                Bitboard this_rank_sixbit_occ = sixbit_occ << calc_rank_shift(rank);
                Bitboard right_slider_moves   = generate_right_slider_moves(this_rank_sixbit_occ, bits_on_to_right);
                Bitboard left_slider_moves    = generate_left_slider_moves(this_rank_sixbit_occ,  bits_on_to_left);
                horiz_slider_moves[i][sixbit_occ] = right_slider_moves | left_slider_moves;

                Bitboard this_file_occupancy = rotate_occupancy_to_vertical_reverse_and_extend(sixbit_occ, file);
                Bitboard up_slider_moves     = generate_up_slider_moves(this_file_occupancy,   bits_on_to_up);
                Bitboard down_slider_moves   = generate_down_slider_moves(this_file_occupancy, bits_on_to_down);
                vert_slider_moves[i][sixbit_occ] = up_slider_moves | down_slider_moves;

                Bitboard rotated_occ_a1h8 = rotate_occupancy_onto_a1h8_diagonal_and_extend(sixbit_occ, diag_start, diag_length);
                Bitboard moves_a1h8       = generate_a1h8_diagonal_moves(rotated_occ_a1h8, bits_on_to_up_and_right, bits_on_to_down_and_left);
                diag_moves_a1h8[i][sixbit_occ] = moves_a1h8;

                Bitboard rotated_occ_a8h1 = rotate_occupancy_onto_a8h1_diagonal_and_extend(sixbit_occ, diag_start, diag_length);
                Bitboard moves_a8h1       = generate_a8h1_diagonal_moves(rotated_occ_a8h1, bits_on_to_up_and_left, bits_on_to_down_and_right);
				diag_moves_a8h1[i][sixbit_occ] = moves_a8h1;
            }
        }
    }
}
