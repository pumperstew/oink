#include "ChessConstants.hpp"
#include "BasicOperations.hpp"

namespace chess
{
	using namespace util;
	using namespace moves;

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

	void GenerateRankAndFileMasks()
	{
		for (int i = 0; i < util::BOARD_SIZE; ++i) { //rank or file loop
            file_masks[i] = util::nil;
            rank_masks[i] = util::fullrank << (i << 3);
            for (int j = 0; j < util::BOARD_SIZE; ++j) { //rank loop for file masks
                file_masks[i] |= util::one << (j*util::BOARD_SIZE + i);
            }
        }
	}

	void GenerateKnightMoves(int square, int rank, int file, bitboard squareBitboard)
	{
		knight_moves[square] = 0;
        if (rank >= 2) //2 down, 1 left; 2 down, 1 right
            knight_moves[square] |= ((squareBitboard >> 17) | (squareBitboard >> 15)) & rank_masks[rank-2];
        if (rank >= 1) //1 down, 2 left; 1 down, 2 right
            knight_moves[square] |= ((squareBitboard >> 10) | (squareBitboard >> 6))  & rank_masks[rank-1];
        if (rank <= 6) //1 up, 2 left; 1 up, 2 right                         
            knight_moves[square] |= ((squareBitboard << 6)  | (squareBitboard << 10)) & rank_masks[rank+1];
        if (rank <= 5) //2 up, 1 left; 2 up, 1 right
            knight_moves[square] |= ((squareBitboard << 15) | (squareBitboard << 17)) & rank_masks[rank+2];
	}

	//Turn on bits to the right of the given square, which is assumed to be on the given file.
	bitboard TurnOnBitsToRight(bitboard squareBit, int file)
	{
		bitboard result = util::nil;
		for (int j = 1; j < util::BOARD_SIZE - file; ++j) 
			result |= (squareBit << j);

		return result;
	}

	bitboard TurnOnBitsToLeft(bitboard squareBit, int file)
	{
		bitboard result = util::nil;
		for (int j = 1; j <= file; ++j) 
			result |= (squareBit >> j);

		return result;
	}

	bitboard GenerateRightSliderMoves(bitboard rankOccupancy, bitboard bitsOnToRight)
	{
		bitboard rightMoves = rankOccupancy & bitsOnToRight; //1s where pieces sit, to right of sq
        rightMoves = ( 
            ( (rightMoves << 1) | (rightMoves << 2) | (rightMoves << 3) //first fill in 1s to right of first blocking piece..
            | (rightMoves << 4) | (rightMoves << 5) | (rightMoves << 6) )
            & bitsOnToRight)  //..eliminate overflows to next rank
            ^ bitsOnToRight; //and finally flip to get squares we *can* go to
		return rightMoves;
	}

	bitboard GenerateLeftSliderMoves(bitboard rankOccupancy, bitboard bitsOnToLeft)
	{
		bitboard leftMoves = rankOccupancy & bitsOnToLeft;
        leftMoves = (
            ( (leftMoves >> 1) | (leftMoves >> 2) | (leftMoves >> 3)
            | (leftMoves >> 4) | (leftMoves >> 5) | (leftMoves >> 6) )
            & bitsOnToLeft)
            ^ bitsOnToLeft;
		return leftMoves;
	}

	bitboard GenerateUpSliderMoves(bitboard verticalOccupancy, bitboard bitsOnToUp)
	{
		bitboard upMoves = verticalOccupancy & bitsOnToUp; //same principle as above
        upMoves = (
            ( (upMoves << 8)  | (upMoves << 16) | (upMoves << 24)
            | (upMoves << 32) | (upMoves << 40) | (upMoves << 48) )
            & bitsOnToUp)
            ^ bitsOnToUp;
		return upMoves;
	}

	bitboard GenerateDownSliderMoves(bitboard verticalOccupancy, bitboard bitsOnToDown)
	{
		bitboard downMoves = verticalOccupancy & bitsOnToDown;
        downMoves = (
            ( (downMoves >> 8)  | (downMoves >> 16) | (downMoves >> 24)
            | (downMoves >> 32) | (downMoves >> 40) | (downMoves >> 48) )
            & bitsOnToDown)
            ^ bitsOnToDown;
		return downMoves;
	}

	bitboard RotateOccupancyVertical(bitboard occupancy /*in low eight bits*/, int desiredFile)
	{
		bitboard vertOcc = util::nil; //vertical occupancy is a bit tricky, as bits not contiguous
        for (int j = 0; j < util::BOARD_SIZE; ++j)
		{
            bitboard stateOfBit = (occupancy & singleBit[j]) >> j; //get the state of the bit at j (here, j = file), and shift down to LSB.
            vertOcc |= (stateOfBit << RankFileToIndex(j, desiredFile)); //shift back up to correct point. Here, j = rank.
        }
		return vertOcc;
	}

    void InitializeConstants()
    {
		for (int i = 0; i < util::BOARD_SIZE; ++i)
		{
			singleBit[i] = util::one << i;
		}

        GenerateRankAndFileMasks();

        //===================== Generate moves=======================
        for (int i = 0; i < util::NUM_SQUARES; ++i) //loop over all squares
		{ 
            bitboard this_sq = util::one << i;
            int rank, file;
            IndexToRankAndFile(i, rank, file);
           
			GenerateKnightMoves(i, rank, file, this_sq);

            //============== Generate slider moves (horiz, vert, diag) ==========

			//Generate masks with bits on to the right and to the left of the current square, on its rank.
            const bitboard to_right = TurnOnBitsToRight(this_sq, file);
			const bitboard to_left = TurnOnBitsToLeft(this_sq, file);
			bitboard to_up = 0, to_down = 0;
           
            diag_masks_a1h8[i] = 0, diag_masks_a8h1[i] = 0;

            //Gen masks with bits 1 on both diagonals intersecting the current square,
            //also get masks for up/down corresponding to to_right/left in same loop.
            for (int j = 0; j < util::BOARD_SIZE - rank; ++j) {
                to_up |= (this_sq << ((j+1)*util::BOARD_SIZE));
				//OINK_TODO: (j*util::BOARD_SIZE + j) same as:
				//int offset = 9 * j; since 8n + n = 9n.
                diag_masks_a1h8[i] |= (this_sq << (j*util::BOARD_SIZE + j)) & rank_masks[rank+j]; //up and right //
                diag_masks_a8h1[i] |= (this_sq << (j*util::BOARD_SIZE - j)) & rank_masks[rank+j]; //up and left
            }

			auto foo = diag_masks_a1h8[i];

            for (int j = 0; j <= rank; ++j) {
                to_down |= (this_sq >> ((j+1)*util::BOARD_SIZE));
                diag_masks_a1h8[i] |= (this_sq >> (j*util::BOARD_SIZE + j)) & rank_masks[rank-j]; //down and left
                diag_masks_a8h1[i] |= (this_sq >> (j*util::BOARD_SIZE - j)) & rank_masks[rank-j]; //down and right
            }

			auto bar = diag_masks_a1h8[i];
            //std::cout << print_square(i) << std::endl;
            //print_bitboard(diag_a1h8);

            //loop over all occupancy permutations on ranks/files:
            for (bitboard occ = 0; occ <= util::fullrank; ++occ) 
			{ 
				bitboard thisRankOccupancy = occ << (rank<<3);
                const bitboard rightSliderMoves = GenerateRightSliderMoves(thisRankOccupancy, to_right);
                const bitboard leftSliderMoves = GenerateLeftSliderMoves(thisRankOccupancy, to_left);

                rook_horiz_moves[i][occ] = rightSliderMoves | leftSliderMoves;

                bitboard thisFileVerticalOccupancy = RotateOccupancyVertical(occ, file);
                bitboard upMoves = GenerateUpSliderMoves(thisFileVerticalOccupancy, to_up); //same principle as above
                bitboard downMoves = GenerateDownSliderMoves(thisFileVerticalOccupancy, to_down);

                rook_vert_moves[i][occ] = upMoves | downMoves;

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

				bitboard wtf = diag_masks_a1h8[i];

				auto moo = 
                    ( (a1h8_moves << 9) |  (a1h8_moves << 18) | (a1h8_moves << 27) 
                    | (a1h8_moves << 36) | (a1h8_moves << 45) | (a1h8_moves << 54) );

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
