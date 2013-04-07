#include "ChessConstants.hpp"
#include "BasicOperations.hpp"

using namespace chess::util;

namespace chess
{
	namespace squares
	{
		std::vector<bitboard> GetAllSquares()
		{
			std::vector<bitboard> allSquares;
			allSquares.reserve(NUM_SQUARES);
			bitboard square = util::one;
			for (int i = 0; i < util::NUM_SQUARES; ++i)
			{
				allSquares.push_back(square);
				square <<= util::one;
			}
			return allSquares;
		}
	}

    namespace moves
    {
        bitboard rook_horiz_moves[util::NUM_SQUARES][256];
        bitboard rook_vert_moves[util::NUM_SQUARES][256];
        bitboard file_masks[util::BOARD_SIZE];
        bitboard rank_masks[util::BOARD_SIZE];
        bitboard knight_moves[util::NUM_SQUARES];
        bitboard diag_moves[util::NUM_SQUARES][256];
        bitboard diag_masks_a1h8[util::NUM_SQUARES];
        bitboard diag_masks_a8h1[util::NUM_SQUARES];
    }

    void InitializeConstants()
    {
        using namespace moves;
        //Masks for each rank and file
        for (int i = 0; i < util::BOARD_SIZE; ++i) { //rank or file loop
            file_masks[i] = 0;
            rank_masks[i] = util::fullrank << (i << 3);
            for (int j = 0; j < util::BOARD_SIZE; ++j) { //rank look for file masks
                file_masks[i] |= util::one << (j*util::BOARD_SIZE + i);
            }
        }

        //===================== Generate moves=======================
        for (int i = 0; i < util::NUM_SQUARES; ++i) { //loop over all squares
            bitboard this_sq = util::one << i;
            int rank, file;
            IndexToRankAndFile(i, rank, file);
            //============== Knight moves
            knight_moves[i] = 0;
            if (rank >= 2) //2 down, 1 left; 2 down, 1 right
                knight_moves[i] |= ((this_sq >> 17) | (this_sq >> 15)) & rank_masks[rank-2];
            if (rank >= 1) //1 down, 2 left; 1 down, 2 right
                knight_moves[i] |= ((this_sq >> 10) | (this_sq >> 6))  & rank_masks[rank-1];
            if (rank <= 6) //1 up, 2 left; 1 up, 2 right                         
                knight_moves[i] |= ((this_sq << 6)  | (this_sq << 10)) & rank_masks[rank+1];
            if (rank <= 5) //2 up, 1 left; 2 up, 1 right
                knight_moves[i] |= ((this_sq << 15) | (this_sq << 17)) & rank_masks[rank+2];

            //============== Generate slider moves (horiz, vert, diag) ==========
            bitboard to_right = 0, to_left = 0, to_up = 0, to_down = 0;
           
            diag_masks_a1h8[i] = 0, diag_masks_a8h1[i] = 0;

            //Gen masks with bits 1 to the right (left) of current square.
            for (int j = 1; j < util::BOARD_SIZE - file; ++j) to_right |= (this_sq << j);
            for (int j = 1; j <= file;                   ++j) to_left  |= (this_sq >> j);

            //Gen masks with bits 1 on both diagonals intersecting the current square,
            //also get masks for up/down corresponding to to_right/left in same loop.
            for (int j = 0; j < util::BOARD_SIZE - rank; ++j) {
                to_up |= (this_sq << ((j+1)*util::BOARD_SIZE)); 
                diag_masks_a1h8[i] |= (this_sq << (j*util::BOARD_SIZE + j)) & rank_masks[rank+j]; //up and right
                diag_masks_a8h1[i] |= (this_sq << (j*util::BOARD_SIZE - j)) & rank_masks[rank+j]; //up and left
            }
            for (int j = 0; j <= rank; ++j) {
                to_down |= (this_sq >> ((j+1)*util::BOARD_SIZE));
                diag_masks_a1h8[i] |= (this_sq >> (j*util::BOARD_SIZE + j)) & rank_masks[rank-j]; //down and left
                diag_masks_a8h1[i] |= (this_sq >> (j*util::BOARD_SIZE - j)) & rank_masks[rank-j]; //down and right
            }

            //std::cout << print_square(i) << std::endl;
            //print_bitboard(diag_a1h8);

            //loop over all occupancy permutations on ranks/files:
            for (bitboard occ = 0; occ <= util::fullrank; ++occ) 
			{ 
                bitboard right_moves = (occ << (rank<<3)) & to_right; //1s where pieces sit, to right of sq
                right_moves = (( //first fill in 1s to right of first blocking piece
                    ( (right_moves << 1) | (right_moves << 2) | (right_moves << 3)
                    | (right_moves << 4) | (right_moves << 5) | (right_moves << 6) )
                    & to_right)  //eliminate overflows to next rank
                    ^ to_right); //flip to get squares we *can* go to

                bitboard left_moves = (occ << (rank<<3)) & to_left;
                left_moves = ((
                    ( (left_moves >> 1) | (left_moves >> 2) | (left_moves >> 3)
                    | (left_moves >> 4) | (left_moves >> 5) | (left_moves >> 6) )
                    & to_left)
                    ^ to_left);

                rook_horiz_moves[i][occ] = right_moves | left_moves;

                bitboard vert_occ = 0; //vertical occupancy is more tricky, as bits not contiguous
                for (int j = 0; j < util::BOARD_SIZE; ++j) {
                    bitboard thisbit = (occ & (util::one << j)) >> j;
                    vert_occ |= (thisbit << (file + j*util::BOARD_SIZE));
                }

                bitboard up_moves = vert_occ & to_up; //same principle as above
                up_moves = ((
                    ( (up_moves << 8)  | (up_moves << 16) | (up_moves << 24)
                    | (up_moves << 32) | (up_moves << 40) | (up_moves << 48) )
                    & to_up)
                    ^ to_up);

                bitboard down_moves = vert_occ & to_down;
                down_moves = ((
                    ( (down_moves >> 8)  | (down_moves >> 16) | (down_moves >> 24)
                    | (down_moves >> 32) | (down_moves >> 40) | (down_moves >> 48) )
                    & to_down)
                    ^ to_down);

                rook_vert_moves[i][occ] = up_moves | down_moves;

                //for a1h8, we go up + right. for a8h1 we go up + left (not down + right)
                int diag_begin[2][2]; //{a1h8, a8h1}{rank, file}
                int diag_length[2]; //{a1h8, a8h1}
                if (rank > file) {
                    diag_begin[0][0] = rank - file; //starting rank of the diag in a1h8 dir
                    diag_begin[0][1] = 0;           //starting file is zero as rank > file
                    diag_begin[1][0] = 0;           //starting rank in other dir
                    diag_begin[1][1] = rank + file; 
                    diag_length[0] = util::BOARD_SIZE - diag_begin[0][0]; //8 - starting rank
                    diag_length[1] = diag_begin[1][1] + 1;
                } else {
                    diag_begin[0][0] = 0;
                    diag_begin[0][1] = file - rank;
                    diag_begin[1][0] = file + rank;
                    diag_begin[1][1] = 0;
                    diag_length[0] = util::BOARD_SIZE - diag_begin[0][1];
                    diag_length[1] = diag_begin[1][1] + 1;
                }

                bitboard diag_a1h8_occ = 0;
                for (int j = diag_begin[0][0]; j < diag_length[0]; ++j) { //go up + right, rank loop
                    bitboard thisbit = (occ & (util::one << j)) >> j;
                    int cur_file = diag_begin[0][1] + (j - diag_begin[0][0]); //file = starting file on diag + steps
                    diag_a1h8_occ |= (thisbit << (j*util::BOARD_SIZE + cur_file));
                }        
                bitboard a1h8_moves = diag_a1h8_occ & diag_masks_a1h8[i];

                bitboard diag_a8h1_occ = 0;
                for (int j = diag_begin[1][0]; j < diag_length[1]; ++j) { //start at same sq, go up + left
                    bitboard thisbit = (occ & (util::one << j)) >> j;
                    int cur_file = diag_begin[1][1] - (j - diag_begin[1][0]);
                    diag_a8h1_occ |= (thisbit << (j*util::BOARD_SIZE + cur_file));
                }        
                bitboard a8h1_moves = diag_a8h1_occ & diag_masks_a8h1[i];

                a1h8_moves = ((
                    ( (a1h8_moves << 9) |  (a1h8_moves << 18) | (a1h8_moves << 27) 
                    | (a1h8_moves << 36) | (a1h8_moves << 45) | (a1h8_moves << 54) )
                    & diag_masks_a1h8[i])
                    ^ diag_masks_a1h8[i]);

                /*if (i == 0 && occ == 0) {
                    print_bitboard(diag_masks_a1h8[i]);
                    print_bitboard(diag_a1h8_occ);
                    print_bitboard(a1h8_moves);
                }*/
                diag_moves[i][occ] = a1h8_moves; //todo
            }
        }
    }
}
