#include "ChessConstants.hpp"
#include "BasicOperations.hpp"
#include "Display.hpp"

#include <cassert>

namespace chess
{
	using namespace util;
	using namespace moves;

	namespace util
	{
		bitboard singleBit[BOARD_SIZE];
	}

	namespace pieces
	{
		char PieceSymbols[15];
	}

    namespace moves
    {
        bitboard rook_horiz_moves[util::NUM_SQUARES][256];
        bitboard rook_vert_moves[util::NUM_SQUARES][256];
        bitboard file_masks[util::BOARD_SIZE];
        bitboard rank_masks[util::BOARD_SIZE];
        bitboard knight_moves[util::NUM_SQUARES];
        bitboard diag_moves_a1h8[util::NUM_SQUARES][256];
		bitboard diag_moves_a8h1[util::NUM_SQUARES][256];
        bitboard diagMasks_a1h8[util::NUM_SQUARES];
        bitboard diagMasks_a8h1[util::NUM_SQUARES];
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

	bitboard GenerateDiagonal_a1h8_Moves(bitboard diagonalOcc_a1h8, bitboard bitsOnToUpAndRight, bitboard bitsOnToDownAndLeft)
	{
		bitboard upRightMoves  = diagonalOcc_a1h8 & bitsOnToUpAndRight;
		bitboard downLeftMoves = diagonalOcc_a1h8 & bitsOnToDownAndLeft;

		/*auto foo = (
            ( (upRightMoves << 9) |  (upRightMoves << 18) | (upRightMoves << 27) 
            | (upRightMoves << 36) | (upRightMoves << 45) | (upRightMoves << 54) ));*/

		/*if (diagonalOcc_a1h8 == 1)
		{
			PrintBitboard(foo);
			PrintBitboard(bitsOnToUpAndRight);
			PrintBitboard(bitsOnToDownAndLeft);
		}*/

		upRightMoves = (
            ( (upRightMoves << 9) |  (upRightMoves << 18) | (upRightMoves << 27) 
            | (upRightMoves << 36) | (upRightMoves << 45) | (upRightMoves << 54) )
            & bitsOnToUpAndRight)
            ^ bitsOnToUpAndRight;

		downLeftMoves = (
            ( (downLeftMoves >> 9) |  (downLeftMoves >> 18) | (downLeftMoves >> 27) 
            | (downLeftMoves >> 36) | (downLeftMoves >> 45) | (downLeftMoves >> 54) )
            & bitsOnToDownAndLeft)
            ^ bitsOnToDownAndLeft;

		return upRightMoves | downLeftMoves;
	}

	bitboard GenerateDiagonal_a8h1_Moves(bitboard diagonalOcc_a8h1, bitboard bitsOnToUpAndLeft, bitboard bitsOnToDownAndRight)
	{
		bitboard upAndLeftMoves    = diagonalOcc_a8h1 & bitsOnToUpAndLeft;
		bitboard squaresOccupiedDownAndRight = diagonalOcc_a8h1 & bitsOnToDownAndRight;

		//upAndLeftMoves = (
  //          ( (upAndLeftMoves << 9) |  (upAndLeftMoves << 18) | (upAndLeftMoves << 27) 
  //          | (upAndLeftMoves << 36) | (upAndLeftMoves << 45) | (upAndLeftMoves << 54) )
  //          & bitsOnToUpAndLeft)
  //          ^ bitsOnToUpAndLeft;

		upAndLeftMoves = (
            ( (upAndLeftMoves << 7) |  (upAndLeftMoves << 14) | (upAndLeftMoves << 21) 
            | (upAndLeftMoves << 28) | (upAndLeftMoves << 35) | (upAndLeftMoves << 42) )
            & bitsOnToUpAndLeft)
            ^ bitsOnToUpAndLeft;

		/*if (diagonalOcc_a8h1 ==4194304) {
			PrintBitboard(bitsOnToDownAndRight, "bitsOnToDownAndRight", squares::e5);
			PrintBitboard(diagonalOcc_a8h1, "diagonalOcc_a8h1", squares::e5);
			PrintBitboard(squaresOccupiedDownAndRight, "squaresOccupiedDownAndRight", squares::e5);
			PrintBitboard(squaresOccupiedDownAndRight >> 7, "squaresOccupiedDownAndRight >> 9", squares::e5);
			PrintBitboard(
				( (squaresOccupiedDownAndRight >> 7) |  (squaresOccupiedDownAndRight >> 14) | (squaresOccupiedDownAndRight >> 21) 
				| (squaresOccupiedDownAndRight >> 28) | (squaresOccupiedDownAndRight >> 35) | (squaresOccupiedDownAndRight >> 42) ) );

			PrintBitboard( (
				( (squaresOccupiedDownAndRight >> 7) |  (squaresOccupiedDownAndRight >> 14) | (squaresOccupiedDownAndRight >> 21) 
				| (squaresOccupiedDownAndRight >> 28) | (squaresOccupiedDownAndRight >> 35) | (squaresOccupiedDownAndRight >> 42) ) &   bitsOnToDownAndRight)
            ^ bitsOnToDownAndRight );

		}*/

		bitboard downAndRightMoves = (
            ( (squaresOccupiedDownAndRight >> 7) |  (squaresOccupiedDownAndRight >> 14) | (squaresOccupiedDownAndRight >> 21) 
            | (squaresOccupiedDownAndRight >> 28) | (squaresOccupiedDownAndRight >> 35) | (squaresOccupiedDownAndRight >> 42) )
            & bitsOnToDownAndRight)
            ^ bitsOnToDownAndRight;

		/*if (diagonalOcc_a8h1 ==4194304) {
			PrintBitboard(downAndRightMoves);
		}*/

		return upAndLeftMoves | downAndRightMoves;
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

	const int a1h8_direction = 0, a8h1_direction = 1, rankDiagIndex = 0, fileDiagIndex = 1;

	bitboard RotateOccupanyDiagonal_a1h8(bitboard occupancy /*in low eight bits*/, int diagonalStart[2][2], int diagonalLength[2])
	{
		bitboard diagonalOcc_a1h8 = util::nil;
		int i = 0;
        for (int j = diagonalStart[a1h8_direction][0]; j < diagonalLength[a1h8_direction]; ++j, ++i) //go up + right, rank loop
		{
            bitboard stateOfBit = (occupancy & singleBit[i]) >> i;
            int currentFile = diagonalStart[a1h8_direction][fileDiagIndex] + (j - diagonalStart[a1h8_direction][rankDiagIndex]); //file = starting file on diag + steps
            diagonalOcc_a1h8 |= (stateOfBit << RankFileToIndex(j, currentFile));
        }
		return diagonalOcc_a1h8;
	}

	bitboard RotateOccupanyDiagonal_a8h1(bitboard occupancy /*in low eight bits*/, int diagonalStart[2][2], int diagonalLength[2])
	{
		bitboard diagonalOcc_a8h1 = util::nil;
		int i = 0;
		int x = diagonalLength[a8h1_direction];
		int y = diagonalStart[a8h1_direction][0];
		for (int j = diagonalStart[a8h1_direction][0]; j < diagonalLength[a8h1_direction]; ++j, ++i) //start at same sq, go up + left
		{
			bitboard stateOfBit = (occupancy & singleBit[i]);

			bitboard shiftedUpRanks = stateOfBit << RankFileToIndex(8-i, 0);

			diagonalOcc_a8h1 |= shiftedUpRanks;

			/*bitboard thisbit = (occupancy & singleBit[i]);
			thisbit = thisbit >> i;
			int currentFile = diagonalStart[a8h1_direction][1] - (j - diagonalStart[a8h1_direction][0]);
			diagonalOcc_a8h1 |= (thisbit << RankFileToIndex(j, currentFile));*/

			if (diagonalStart[1][0] == 1 && diagonalStart[1][1] == 7)
			{
				//PrintBitboard(diagonalOcc_a8h1);
			}
		}
		if (diagonalStart[1][0] == 1 && diagonalStart[1][1] == 7)
			{
				//PrintBitboard(diagonalOcc_a8h1, "done");
			}

		return diagonalOcc_a8h1;
	}

	void GenerateDiagonalStartAndLength(int rank, int file, int diagonalStart[2][2], int diagonalLength[2])
	{
		 //OINK_TODO: fix rank>file branch??
		if (rank > file) //square we're considering (i) is in upper/left diagonal half
		{
            diagonalStart[a1h8_direction][rankDiagIndex] = rank - file; //Starting rank of the diag in a1h8 direction
            diagonalStart[a1h8_direction][fileDiagIndex] = 0;           //Starting file is zero, as rank > file
            diagonalStart[a8h1_direction][rankDiagIndex] = 0;           //Starting rank in a8h1 (other) direction is zero too.
            diagonalStart[a8h1_direction][fileDiagIndex] = rank + file; //Starting file in a8h1 direction.
            diagonalLength[a1h8_direction] = util::BOARD_SIZE - diagonalStart[a1h8_direction][rankDiagIndex]; //8 - starting rank.
            diagonalLength[a8h1_direction] = util::BOARD_SIZE - diagonalStart[a8h1_direction][fileDiagIndex];
        } 
		else //See above - symmtery.
		{
            diagonalStart[a1h8_direction][rankDiagIndex] = 0;
            diagonalStart[a1h8_direction][fileDiagIndex] = file - rank;
            diagonalStart[a8h1_direction][rankDiagIndex] = rank - ((util::BOARD_SIZE - 1) - file); //file + rank;
            diagonalStart[a8h1_direction][fileDiagIndex] = util::BOARD_SIZE - 1;
            diagonalLength[a1h8_direction] = util::BOARD_SIZE - diagonalStart[a1h8_direction][fileDiagIndex];
            diagonalLength[a8h1_direction] = util::BOARD_SIZE - diagonalStart[a8h1_direction][rankDiagIndex];
        }
	}

    void InitializeConstants()
    {
		pieces::PieceSymbols[pieces::NONE] = '*';
		pieces::PieceSymbols[pieces::WHITE_PAWN] = 'P';
		pieces::PieceSymbols[pieces::WHITE_KING] = 'K';
		pieces::PieceSymbols[pieces::WHITE_KNIGHT] = 'N';
		pieces::PieceSymbols[pieces::WHITE_BISHOP] = 'B';
		pieces::PieceSymbols[pieces::WHITE_ROOK] = 'R';
		pieces::PieceSymbols[pieces::WHITE_QUEEN] = 'Q';
		pieces::PieceSymbols[pieces::BLACK_PAWN] = 'p';
		pieces::PieceSymbols[pieces::BLACK_KING] = 'k';
		pieces::PieceSymbols[pieces::BLACK_KNIGHT] = 'n';
		pieces::PieceSymbols[pieces::BLACK_BISHOP] = 'b';
		pieces::PieceSymbols[pieces::BLACK_ROOK] = 'r';
		pieces::PieceSymbols[pieces::BLACK_QUEEN] = 'q';

		for (int i = 0; i < util::BOARD_SIZE; ++i)
		{
			singleBit[i] = util::one << i;
		}

        GenerateRankAndFileMasks();

        //===================== Generate moves=======================
        for (int i = 0; i < util::NUM_SQUARES; ++i) //loop over all squares
		{ 
            bitboard currentSquareBit = util::one << i;
            int rank, file;
            IndexToRankAndFile(i, rank, file);
           
			GenerateKnightMoves(i, rank, file, currentSquareBit);

            //============== Generate slider moves (horiz, vert, diag) ==========

			//Generate masks with bits on to the right and to the left of the current square, on its rank.
            const bitboard bitsOnToRight = TurnOnBitsToRight(currentSquareBit, file);
			const bitboard bitsOnToLeft = TurnOnBitsToLeft(currentSquareBit, file);
			bitboard bitsOnToUp = util::nil, bitsOnToDown = util::nil;
			bitboard bitsOnToUpAndRight = util::nil, bitsOnToDownAndLeft = util::nil;
			bitboard bitsOnToUpAndLeft = util::nil, bitsOnToDownAndRight = util::nil;

            //Generate masks with bits 1 on both diagonals intersecting the current square.
            //Also get masks for bitsOnToUp/bitsOnToDown corresponding to bitsOnToRight/bitsOnToLeft in same loop.
            for (int offset = 1; offset < util::BOARD_SIZE - rank; ++offset)
			{
                bitsOnToUp |= (currentSquareBit << RankFileToIndex(offset, 0)); //<< 8, << 16, << 24, ... (shift up ranks)
				
				//Up and right: take bit, shift up along the diagonal (<< 9, << 18, << 27, ...), and mask off other ranks.
				//The shifts are non-intuitive, because to go *right*, we shift 9 (not 7), because a1 is LSB, so going 
				//right on the board means shifting *left*. Basically everything is mirrored along the y-axis in bit-repro.
				//(final masking step is necessary because we loop further than we need here, because we generate the up moves too)
				bitsOnToUpAndRight |= (currentSquareBit << RankFileToIndex(offset, offset)) & rank_masks[rank + offset];
				//Up and left. Same as a1h8, but file offset is negative, so shifts are (<< 7, << 14, << 21, ...)
                bitsOnToUpAndLeft |= (currentSquareBit << RankFileToIndex(offset, -offset)) & rank_masks[rank + offset];
            }

            for (int offset = 1; offset <= rank; ++offset)
			{
                bitsOnToDown |= (currentSquareBit >> (offset*util::BOARD_SIZE)); //>> 8, >> 16, >> 24, ... (shift down ranks)

				//Down and left
                bitsOnToDownAndLeft |= (currentSquareBit >> RankFileToIndex(offset, offset)) & rank_masks[rank - offset];
				//Down and right - file offset is 
                bitsOnToDownAndRight |= (currentSquareBit >> RankFileToIndex(offset, -offset)) & rank_masks[rank - offset];
            }

			diagMasks_a1h8[i] = bitsOnToUpAndRight | bitsOnToDownAndLeft;
			diagMasks_a8h1[i] = bitsOnToUpAndLeft | bitsOnToDownAndRight;

			//for a1h8, we go up + right. for a8h1 we go up + left (not down + right)
            int diagonalStart[2][2]; //{a1h8, a8h1}{rank, file}
            int diagonalLength[2];   //{a1h8, a8h1}
			/*if (rank == 4 && file == 4)
			{
				PrintBitboard(currentSquareBit);
			}*/
			GenerateDiagonalStartAndLength(rank, file, diagonalStart, diagonalLength);

            //loop over all occupancy permutations on ranks/files:
            for (bitboard occ = 0; occ <= util::fullrank; ++occ) 
			{ 
				bitboard thisRankOccupancy = occ << (rank<<3);
                const bitboard rightSliderMoves = GenerateRightSliderMoves(thisRankOccupancy, bitsOnToRight);
                const bitboard leftSliderMoves = GenerateLeftSliderMoves(thisRankOccupancy, bitsOnToLeft);
                rook_horiz_moves[i][occ] = rightSliderMoves | leftSliderMoves;

                bitboard thisFileVerticalOccupancy = RotateOccupancyVertical(occ, file);
                bitboard upSliderMoves = GenerateUpSliderMoves(thisFileVerticalOccupancy, bitsOnToUp); //same principle as above
                bitboard downSliderMoves = GenerateDownSliderMoves(thisFileVerticalOccupancy, bitsOnToDown);
                rook_vert_moves[i][occ] = upSliderMoves | downSliderMoves;

				if (i == squares::e5 && occ== (util::one<<5))
				{
					//PrintBitboard(occ);
				}
                bitboard diagonalOcc_a1h8 = RotateOccupanyDiagonal_a1h8(occ, diagonalStart, diagonalLength);

				
			
                bitboard a1h8_moves = GenerateDiagonal_a1h8_Moves(diagonalOcc_a1h8, bitsOnToUpAndRight, bitsOnToDownAndLeft);

                bitboard diagonalOcc_a8h1 = RotateOccupanyDiagonal_a8h1(occ, diagonalStart, diagonalLength);

                bitboard a8h1_moves = GenerateDiagonal_a8h1_Moves(diagonalOcc_a8h1, bitsOnToUpAndLeft, bitsOnToDownAndRight);
				
				
				if (i == squares::e5 && occ== (util::one<<5))
				{
					//PrintBitboard(diagonalOcc_a8h1, "diagonalOcc_a8h1", squares::e5);
					//PrintBitboard(a8h1_moves, "a8h1_moves", squares::e5);
				}

				diag_moves_a1h8[i][occ] = a1h8_moves;
				diag_moves_a8h1[i][occ] = a8h1_moves;

				/*if (i == squares::e5 && occ == 2)
				{
					PrintBitboard(occ, "occ");
					PrintBitboard(currentSquareBit, "current square", i);
					PrintBitboard(diagMasks_a1h8[i], "diagMasks_a1h8", i);
					PrintBitboard(diagonalOcc_a1h8, "diagonalOcc_a1h8", i);
					PrintBitboard(diagonalOcc_a8h1, "diagonalOcc_a8h1", i);
					PrintBitboard(bitsOnToDownAndRight, "bitsOnToDownAndRight", i);
					PrintBitboard(a1h8_moves, "a1h8_moves", i);
					PrintBitboard(a8h1_moves, "a8h1_moves", i);
				}*/
            }
        }
    }
}
