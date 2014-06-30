#ifndef CHESSCONSTANTS_HPP
#define CHESSCONSTANTS_HPP

#include "BasicTypes.hpp"

namespace chess
{
    void constants_initialize();

    namespace starting
    {
        const Bitboard white_king	 = 0x0000000000000010; //e1 = bit 4 = 2^4 = 16
        const Bitboard black_king	 = 0x1000000000000000;
        const Bitboard white_rooks	 = 0x0000000000000081;
        const Bitboard black_rooks	 = 0x8100000000000000;
        const Bitboard white_knights = 0x0000000000000042;
        const Bitboard black_knights = 0x4200000000000000;
        const Bitboard white_bishops = 0x0000000000000024;
        const Bitboard black_bishops = 0x2400000000000000;
        const Bitboard white_queen	 = 0x0000000000000008;
        const Bitboard black_queen	 = 0x0800000000000000;
        const Bitboard white_pawns	 = 0x000000000000FF00;
        const Bitboard black_pawns	 = 0x00FF000000000000;
    }

    namespace util
    {
        const Bitboard one         = 0x0000000000000001;
        const Bitboard full        = 0xffffffffffffffff;
        const Bitboard nil         = 0x0000000000000000;
        const Bitboard fullrank    = 0x00000000000000ff;
        const Square   NUM_SQUARES = 64;
        const RankFile BOARD_SIZE  = 8;
    }

	namespace ranks
	{
		const RankFile first   = 0;
		const RankFile second  = 1;
		const RankFile third   = 2;
		const RankFile fourth  = 3;
		const RankFile fifth   = 4;
		const RankFile sixth   = 5;
		const RankFile seventh = 6;
		const RankFile eighth  = 7;
	}

    namespace files
	{
		const RankFile a = 0;
		const RankFile b = 1;
		const RankFile c = 2;
		const RankFile d = 3;
		const RankFile e = 4;
		const RankFile f = 5;
		const RankFile g = 6;
		const RankFile h = 7;
	}

	namespace pieces
	{
		const Piece NONE         = 0;
		const Piece WHITE_PAWN   = 0x1;
        const Piece BLACK_PAWN   = 0x2;
		const Piece WHITE_KING   = 0x3;
        const Piece BLACK_KING   = 0x4;
        const Piece WHITE_ROOK   = 0x5;
        const Piece BLACK_ROOK   = 0x6;
        const Piece WHITE_KNIGHT = 0x7;
        const Piece BLACK_KNIGHT = 0x8;
        const Piece WHITE_BISHOP = 0x9;
        const Piece BLACK_BISHOP = 0xa;
        const Piece WHITE_QUEEN  = 0xb;
        const Piece BLACK_QUEEN  = 0xc;

		const Piece KNIGHTS[2] = { 0x7, 0x8 };
		const Piece BISHOPS[2] = { 0x9, 0xa };
		const Piece KINGS[2]   = { 0x3, 0x4 };
		const Piece PAWNS[2]   = { 0x1, 0x2 };
		const Piece ROOKS[2]   = { 0x5, 0x6 };
		const Piece QUEENS[2]  = { 0xb, 0xc };

		extern char symbols[15];
	}

	namespace squares
	{
        const Square NO_SQUARE = 64;

		const Square a1 = 0;
		const Square b1 = 1;
		const Square c1 = 2;
		const Square d1 = 3;
		const Square e1 = 4;
		const Square f1 = 5;
		const Square g1 = 6;
		const Square h1 = 7;

		const Square a2 = 8;
		const Square b2 = 9;
		const Square c2 = 10;
		const Square d2 = 11;
		const Square e2 = 12;
		const Square f2 = 13;
		const Square g2 = 14;
		const Square h2 = 15;

		const Square a3 = 16;
		const Square b3 = 17;
		const Square c3 = 18;
		const Square d3 = 19;
		const Square e3 = 20;
		const Square f3 = 21;
		const Square g3 = 22;
		const Square h3 = 23;

		const Square a4 = 24;
		const Square b4 = 25;
		const Square c4 = 26;
		const Square d4 = 27;
		const Square e4 = 28;
		const Square f4 = 29;
		const Square g4 = 30;
		const Square h4 = 31;

		const Square a5 = 32;
		const Square b5 = 33;
		const Square c5 = 34;
		const Square d5 = 35;
		const Square e5 = 36;
		const Square f5 = 37;
		const Square g5 = 38;
		const Square h5 = 39;

		const Square a6 = 40;
		const Square b6 = 41;
		const Square c6 = 42;
		const Square d6 = 43;
		const Square e6 = 44;
		const Square f6 = 45;
		const Square g6 = 46;
		const Square h6 = 47;

		const Square a7 = 48;
		const Square b7 = 49;
		const Square c7 = 50;
		const Square d7 = 51;
		const Square e7 = 52;
		const Square f7 = 53;
		const Square g7 = 54;
		const Square h7 = 55;

		const Square a8 = 56;
		const Square b8 = 57;
		const Square c8 = 58;
		const Square d8 = 59;
		const Square e8 = 60;
		const Square f8 = 61;
		const Square g8 = 62;
		const Square h8 = 63;
	}

    namespace sides
    {
        const Side white = 0;
        const Side black = 1;

        const RankFile STARTING_PAWN_RANKS[] = { ranks::second,    ranks::seventh    };
		const RankFile ABOUT_TO_PROMOTE[]    = { ranks::seventh,   ranks::second     };
        const RankFile NEXT_RANK_OFFSET[]    = { util::BOARD_SIZE, -util::BOARD_SIZE };

        const unsigned char CASTLING_RIGHTS_WHITE_KINGSIDE  = 0x1;
        const unsigned char CASTLING_RIGHTS_WHITE_QUEENSIDE = 0x2;
        const unsigned char CASTLING_RIGHTS_BLACK_KINGSIDE  = 0x4;
        const unsigned char CASTLING_RIGHTS_BLACK_QUEENSIDE = 0x8;

        const unsigned char CASTLING_RIGHTS_ANY_WHITE = CASTLING_RIGHTS_WHITE_KINGSIDE | CASTLING_RIGHTS_WHITE_QUEENSIDE;
        const unsigned char CASTLING_RIGHTS_ANY_BLACK = CASTLING_RIGHTS_BLACK_KINGSIDE | CASTLING_RIGHTS_BLACK_QUEENSIDE;
        const unsigned char CASTLING_RIGHTS_KINGSIDE[2]  = { CASTLING_RIGHTS_WHITE_KINGSIDE, CASTLING_RIGHTS_WHITE_QUEENSIDE };
        const unsigned char CASTLING_RIGHTS_QUEENSIDE[2] = { CASTLING_RIGHTS_BLACK_KINGSIDE, CASTLING_RIGHTS_BLACK_QUEENSIDE };
        const unsigned char CASTLING_RIGHTS_ANY[2]       = { CASTLING_RIGHTS_ANY_WHITE,      CASTLING_RIGHTS_ANY_BLACK };

        const Square KING_ROOK_START[2]  = { squares::h1, squares::h8 };
        const Square QUEEN_ROOK_START[2] = { squares::a1, squares::a8 };
    }

    namespace squarebits
    {
        const Bitboard a1 = 0x0000000000000001;
        const Bitboard b1 = 0x0000000000000002;
        const Bitboard c1 = 0x0000000000000004;
        const Bitboard d1 = 0x0000000000000008;
        const Bitboard e1 = 0x0000000000000010;
        const Bitboard f1 = 0x0000000000000020;
        const Bitboard g1 = 0x0000000000000040;
        const Bitboard h1 = 0x0000000000000080;
        const Bitboard a2 = 0x0000000000000100;
        const Bitboard b2 = 0x0000000000000200;
        const Bitboard c2 = 0x0000000000000400;
        const Bitboard d2 = 0x0000000000000800;
        const Bitboard e2 = 0x0000000000001000;
        const Bitboard f2 = 0x0000000000002000;
        const Bitboard g2 = 0x0000000000004000;
        const Bitboard h2 = 0x0000000000008000;
        const Bitboard a3 = 0x0000000000010000;
        const Bitboard b3 = 0x0000000000020000;
        const Bitboard c3 = 0x0000000000040000;
        const Bitboard d3 = 0x0000000000080000;
        const Bitboard e3 = 0x0000000000100000;
        const Bitboard f3 = 0x0000000000200000;
        const Bitboard g3 = 0x0000000000400000;
        const Bitboard h3 = 0x0000000000800000;
        const Bitboard a4 = 0x0000000001000000;
        const Bitboard b4 = 0x0000000002000000;
        const Bitboard c4 = 0x0000000004000000;
        const Bitboard d4 = 0x0000000008000000;
        const Bitboard e4 = 0x0000000010000000;
        const Bitboard f4 = 0x0000000020000000;
        const Bitboard g4 = 0x0000000040000000;
        const Bitboard h4 = 0x0000000080000000;
        const Bitboard a5 = 0x0000000100000000;
        const Bitboard b5 = 0x0000000200000000;
        const Bitboard c5 = 0x0000000400000000;
        const Bitboard d5 = 0x0000000800000000;
        const Bitboard e5 = 0x0000001000000000;
        const Bitboard f5 = 0x0000002000000000;
        const Bitboard g5 = 0x0000004000000000;
        const Bitboard h5 = 0x0000008000000000;
        const Bitboard a6 = 0x0000010000000000;
        const Bitboard b6 = 0x0000020000000000;
        const Bitboard c6 = 0x0000040000000000;
        const Bitboard d6 = 0x0000080000000000;
        const Bitboard e6 = 0x0000100000000000;
        const Bitboard f6 = 0x0000200000000000;
        const Bitboard g6 = 0x0000400000000000;
        const Bitboard h6 = 0x0000800000000000;
        const Bitboard a7 = 0x0001000000000000;
        const Bitboard b7 = 0x0002000000000000;
        const Bitboard c7 = 0x0004000000000000;
        const Bitboard d7 = 0x0008000000000000;
        const Bitboard e7 = 0x0010000000000000;
        const Bitboard f7 = 0x0020000000000000;
        const Bitboard g7 = 0x0040000000000000;
        const Bitboard h7 = 0x0080000000000000;
        const Bitboard a8 = 0x0100000000000000;
        const Bitboard b8 = 0x0200000000000000;
        const Bitboard c8 = 0x0400000000000000;
        const Bitboard d8 = 0x0800000000000000;
        const Bitboard e8 = 0x1000000000000000;
        const Bitboard f8 = 0x2000000000000000;
        const Bitboard g8 = 0x4000000000000000;
        const Bitboard h8 = 0x8000000000000000;

		const Bitboard indexed[util::NUM_SQUARES] =
		{
			a1, b1, c1, d1, e1, f1, g1, h1,
			a2, b2, c2, d2, e2, f2, g2, h2,
			a3, b3, c3, d3, e3, f3, g3, h3,
			a4, b4, c4, d4, e4, f4, g4, h4,
			a5, b5, c5, d5, e5, f5, g5, h5,
			a6, b6, c6, d6, e6, f6, g6, h6,
			a7, b7, c7, d7, e7, f7, g7, h7,
			a8, b8, c8, d8, e8, f8, g8, h8
		};
    }

    namespace moves
    {
        const Bitboard king_moves[util::NUM_SQUARES] = 
        {
            0x0000000000000302, //a1
            0x0000000000000705,
            0x0000000000000e0a,
            0x0000000000001c14,
            0x0000000000003828,
            0x0000000000007050,
            0x000000000000e0a0,
            0x000000000000c040, //h1
            0x0000000000030203, //a2
            0x0000000000070507,
            0x00000000000e0a0e,
            0x00000000001c141c,
            0x0000000000382838,
            0x0000000000705070,
            0x0000000000e0a0e0,
            0x0000000000c040c0, //h2
            0x0000000003020300, //a3
            0x0000000007050700,
            0x000000000e0a0e00,
            0x000000001c141c00,
            0x0000000038283800,
            0x0000000070507000,
            0x00000000e0a0e000,
            0x00000000c040c000, //h3
            0x0000000302030000,
            0x0000000705070000,
            0x0000000e0a0e0000,
            0x0000001c141c0000,
            0x0000003828380000,
            0x0000007050700000,
            0x000000e0a0e00000,
            0x000000c040c00000,
            0x0000030203000000,
            0x0000070507000000,
            0x00000e0a0e000000,
            0x00001c141c000000,
            0x0000382838000000,
            0x0000705070000000,
            0x0000e0a0e0000000,
            0x0000c040c0000000,
            0x0003020300000000,
            0x0007050700000000,
            0x000e0a0e00000000,
            0x001c141c00000000,
            0x0038283800000000,
            0x0070507000000000,
            0x00e0a0e000000000,
            0x00c040c000000000,
            0x0302030000000000,
            0x0705070000000000,
            0x0e0a0e0000000000,
            0x1c141c0000000000,
            0x3828380000000000,
            0x7050700000000000,
            0xe0a0e00000000000,
            0xc040c00000000000,
            0x0203000000000000,
            0x0507000000000000,
            0x0a0e000000000000,
            0x141c000000000000,
            0x2838000000000000,
            0x5070000000000000,
            0xa0e0000000000000,
            0x40c0000000000000
        };

        const Bitboard pawn_moves[2][util::NUM_SQUARES] =
        {
            {   // White
                0x0000000000000100,                                                                               
                0x0000000000000200,                                                                               
                0x0000000000000400,                                                                               
                0x0000000000000800,                                                                               
                0x0000000000001000,                                                                               
                0x0000000000002000,
                0x0000000000004000,
                0x0000000000008000,
                0x0000000001010000,
                0x0000000002020000,
                0x0000000004040000,
                0x0000000008080000,
                0x0000000010100000,
                0x0000000020200000,
                0x0000000040400000,
                0x0000000080800000,
                0x0000000001000000,
                0x0000000002000000,
                0x0000000004000000,
                0x0000000008000000,
                0x0000000010000000,
                0x0000000020000000,
                0x0000000040000000,
                0x0000000080000000,
                0x0000000100000000,
                0x0000000200000000,
                0x0000000400000000,
                0x0000000800000000,
                0x0000001000000000,
                0x0000002000000000,
                0x0000004000000000,
                0x0000008000000000,
                0x0000010000000000,
                0x0000020000000000,
                0x0000040000000000,
                0x0000080000000000,
                0x0000100000000000,
                0x0000200000000000,
                0x0000400000000000,
                0x0000800000000000,
                0x0001000000000000,
                0x0002000000000000,
                0x0004000000000000,
                0x0008000000000000,
                0x0010000000000000,
                0x0020000000000000,
                0x0040000000000000,
                0x0080000000000000,
                0x0100000000000000,
                0x0200000000000000,
                0x0400000000000000,
                0x0800000000000000,
                0x1000000000000000,
                0x2000000000000000,
                0x4000000000000000,
                0x8000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000
            },
            {   // Black
                0x0000000000000000,                                                                               
                0x0000000000000000,                                                                               
                0x0000000000000000,                                                                               
                0x0000000000000000,                                                                               
                0x0000000000000000,                                                                               
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000001,
                0x0000000000000002,
                0x0000000000000004,
                0x0000000000000008,
                0x0000000000000010,
                0x0000000000000020,
                0x0000000000000040,
                0x0000000000000080,
                0x0000000000000100,
                0x0000000000000200,
                0x0000000000000400,
                0x0000000000000800,
                0x0000000000001000,
                0x0000000000002000,
                0x0000000000004000,
                0x0000000000008000,
                0x0000000000010000,
                0x0000000000020000,
                0x0000000000040000,
                0x0000000000080000,
                0x0000000000100000,
                0x0000000000200000,
                0x0000000000400000,
                0x0000000000800000,
                0x0000000001000000,
                0x0000000002000000,
                0x0000000004000000,
                0x0000000008000000,
                0x0000000010000000,
                0x0000000020000000,
                0x0000000040000000,
                0x0000000080000000,
                0x0000000100000000,
                0x0000000200000000,
                0x0000000400000000,
                0x0000000800000000,
                0x0000001000000000,
                0x0000002000000000,
                0x0000004000000000,
                0x0000008000000000,
                0x0000010100000000,
                0x0000020200000000,
                0x0000040400000000,
                0x0000080800000000,
                0x0000101000000000,
                0x0000202000000000,
                0x0000404000000000,
                0x0000808000000000,
                0x0001000000000000,
                0x0002000000000000,
                0x0004000000000000,
                0x0008000000000000,
                0x0010000000000000,
                0x0020000000000000,
                0x0040000000000000,
                0x0080000000000000
            }
        };

        const Bitboard pawn_captures[2][util::NUM_SQUARES] =
        {
            {
                0x0000000000000200,                                                                               
                0x0000000000000500,                                                                               
                0x0000000000000a00,                                                                               
                0x0000000000001400,                                                                               
                0x0000000000002800,                                                                               
                0x0000000000005000,
                0x000000000000a000,
                0x0000000000004000,
                0x0000000000020000,
                0x0000000000050000,
                0x00000000000a0000,
                0x0000000000140000,
                0x0000000000280000,
                0x0000000000500000,
                0x0000000000a00000,
                0x0000000000400000,
                0x0000000002000000,
                0x0000000005000000,
                0x000000000a000000,
                0x0000000014000000,
                0x0000000028000000,
                0x0000000050000000,
                0x00000000a0000000,
                0x0000000040000000,
                0x0000000200000000,
                0x0000000500000000,
                0x0000000a00000000,
                0x0000001400000000,
                0x0000002800000000,
                0x0000005000000000,
                0x000000a000000000,
                0x0000004000000000,
                0x0000020000000000,
                0x0000050000000000,
                0x00000a0000000000,
                0x0000140000000000,
                0x0000280000000000,
                0x0000500000000000,
                0x0000a00000000000,
                0x0000400000000000,
                0x0002000000000000,
                0x0005000000000000,
                0x000a000000000000,
                0x0014000000000000,
                0x0028000000000000,
                0x0050000000000000,
                0x00a0000000000000,
                0x0040000000000000,
                0x0200000000000000,
                0x0500000000000000,
                0x0a00000000000000,
                0x1400000000000000,
                0x2800000000000000,
                0x5000000000000000,
                0xa000000000000000,
                0x4000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000
            },
            {
                0x0000000000000000,                                                                               
                0x0000000000000000,                                                                               
                0x0000000000000000,                                                                               
                0x0000000000000000,                                                                               
                0x0000000000000000,                                                                               
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000000,
                0x0000000000000002,
                0x0000000000000005,
                0x000000000000000a,
                0x0000000000000014,
                0x0000000000000028,
                0x0000000000000050,
                0x00000000000000a0,
                0x0000000000000040,
                0x0000000000000200,
                0x0000000000000500,
                0x0000000000000a00,
                0x0000000000001400,
                0x0000000000002800,
                0x0000000000005000,
                0x000000000000a000,
                0x0000000000004000,
                0x0000000000020000,
                0x0000000000050000,
                0x00000000000a0000,
                0x0000000000140000,
                0x0000000000280000,
                0x0000000000500000,
                0x0000000000a00000,
                0x0000000000400000,
                0x0000000002000000,
                0x0000000005000000,
                0x000000000a000000,
                0x0000000014000000,
                0x0000000028000000,
                0x0000000050000000,
                0x00000000a0000000,
                0x0000000040000000,
                0x0000000200000000,
                0x0000000500000000,
                0x0000000a00000000,
                0x0000001400000000,
                0x0000002800000000,
                0x0000005000000000,
                0x000000a000000000,
                0x0000004000000000,
                0x0000020000000000,
                0x0000050000000000,
                0x00000a0000000000,
                0x0000140000000000,
                0x0000280000000000,
                0x0000500000000000,
                0x0000a00000000000,
                0x0000400000000000,
                0x0002000000000000,
                0x0005000000000000,
                0x000a000000000000,
                0x0014000000000000,
                0x0028000000000000,
                0x0050000000000000,
                0x00a0000000000000,
                0x0040000000000000
            }
        };
        
        //const Bitboard magic_file_rotators

		extern Bitboard file_masks[util::BOARD_SIZE];
        extern Bitboard rank_masks[util::BOARD_SIZE];
		extern Bitboard diag_masks_a1h8[util::NUM_SQUARES];
        extern Bitboard diag_masks_a8h1[util::NUM_SQUARES];
		extern Bitboard knight_moves[util::NUM_SQUARES];
        extern Bitboard rook_horiz_moves[util::NUM_SQUARES][256];
        extern Bitboard rook_vert_moves[util::NUM_SQUARES][256];
        extern Bitboard diag_moves_a1h8[util::NUM_SQUARES][256];
		extern Bitboard diag_moves_a8h1[util::NUM_SQUARES][256];

        // in order to quickly disallow castling through stuff.
        const Bitboard white_kingside_castling_mask  = 0x0000000000000060;
        const Bitboard white_queenside_castling_mask = 0x000000000000000e;
        const Bitboard black_kingside_castling_mask  = 0x6000000000000000;
        const Bitboard black_queenside_castling_mask = 0x0e00000000000000;
        
        /*const Bitboard right_of[util::NUM_SQUARES] =
        {
        0x00000000000000fe,                                                 
        0x00000000000000fc,                                                 
        0x00000000000000f8,                                                 
        0x00000000000000f0,                                                 
        0x00000000000000e0,                                                 
        0x00000000000000c0,                                                 
        0x0000000000000080,
        0x0000000000000000,
        0x000000000000fe00,
        0x000000000000fc00,
        0x000000000000f800,
        0x000000000000f000,
        0x000000000000e000,
        0x000000000000c000,
        0x0000000000008000,
        0x0000000000000000,
        0x0000000000fe0000,
        0x0000000000fc0000,
        0x0000000000f80000,
        0x0000000000f00000,
        0x0000000000e00000,
        0x0000000000c00000,
        0x0000000000800000,
        0x0000000000000000,
        0x00000000fe000000,
        0x00000000fc000000,
        0x00000000f8000000,
        0x00000000f0000000,
        0x00000000e0000000,
        0x00000000c0000000,
        0x0000000080000000,
        0x0000000000000000,
        0x000000fe00000000,
        0x000000fc00000000,
        0x000000f800000000,
        0x000000f000000000,
        0x000000e000000000,
        0x000000c000000000,
        0x0000008000000000,
        0x0000000000000000,
        0x0000fe0000000000,
        0x0000fc0000000000,
        0x0000f80000000000,
        0x0000f00000000000,
        0x0000e00000000000,
        0x0000c00000000000,
        0x0000800000000000,
        0x0000000000000000,
        0x00fe000000000000,
        0x00fc000000000000,
        0x00f8000000000000,
        0x00f0000000000000,
        0x00e0000000000000,
        0x00c0000000000000,
        0x0080000000000000,
        0x0000000000000000,
        0xfe00000000000000,
        0xfc00000000000000,
        0xf800000000000000,
        0xf000000000000000,
        0xe000000000000000,
        0xc000000000000000,
        0x8000000000000000,
        0x0000000000000000
        };*/

        /*const Bitboard left_of[util::NUM_SQUARES] =
        {
        0x0000000000000000,                                                          
        0x0000000000000001,                                                          
        0x0000000000000003,                                                          
        0x0000000000000007,                                                          
        0x000000000000000e,                                                          
        0x0000000000000018,
        0x0000000000000020,
        0x0000000000000000,
        0x00000000000000fe,
        0x00000000000001f8,
        0x00000000000003e0,
        0x0000000000000780,
        0x0000000000000e00,
        0x0000000000001800,
        0x0000000000002000,
        0x0000000000000000,
        0x000000000000fe00,
        0x000000000001f800,
        0x000000000003e000,
        0x0000000000078000,
        0x00000000000e0000,
        0x0000000000180000,
        0x0000000000200000,
        0x0000000000000000,
        0x0000000000fe0000,
        0x0000000001f80000,
        0x0000000003e00000,
        0x0000000007800000,
        0x000000000e000000,
        0x0000000018000000,
        0x0000000020000000,
        0x0000000000000000,
        0x00000000fe000000,
        0x00000001f8000000,
        0x00000003e0000000,
        0x0000000780000000,
        0x0000000e00000000,
        0x0000001800000000,
        0x0000002000000000,
        0x0000000000000000,
        0x000000fe00000000,
        0x000001f800000000,
        0x000003e000000000,
        0x0000078000000000,
        0x00000e0000000000,
        0x0000180000000000,
        0x0000200000000000,
        0x0000000000000000,
        0x0000fe0000000000,
        0x0001f80000000000,
        0x0003e00000000000,
        0x0007800000000000,
        0x000e000000000000,
        0x0018000000000000,
        0x0020000000000000,
        0x0000000000000000,
        0x00fe000000000000,
        0x01f8000000000000,
        0x03e0000000000000,
        0x0780000000000000,
        0x0e00000000000000,
        0x1800000000000000,
        0x2000000000000000,
        0x0000000000000000,
        };

        const Bitboard up_from[util::NUM_SQUARES] =
        {
        0x0101010101010100,                          
        0x0202020202020200,                          
        0x0404040404040400,                          
        0x0808080808080800,                          
        0x1010101010101000,                          
        0x2020202020202000,
        0x4040404040404000,
        0x8080808080808000,
        0x0101010101010000,
        0x0202020202020000,
        0x0404040404040000,
        0x0808080808080000,
        0x1010101010100000,
        0x2020202020200000,
        0x4040404040400000,
        0x8080808080800000,
        0x0101010101000000,
        0x0202020202000000,
        0x0404040404000000,
        0x0808080808000000,
        0x1010101010000000,
        0x2020202020000000,
        0x4040404040000000,
        0x8080808080000000,
        0x0101010100000000,
        0x0202020200000000,
        0x0404040400000000,
        0x0808080800000000,
        0x1010101000000000,
        0x2020202000000000,
        0x4040404000000000,
        0x8080808000000000,
        0x0101010000000000,
        0x0202020000000000,
        0x0404040000000000,
        0x0808080000000000,
        0x1010100000000000,
        0x2020200000000000,
        0x4040400000000000,
        0x8080800000000000,
        0x0101000000000000,
        0x0202000000000000,
        0x0404000000000000,
        0x0808000000000000,
        0x1010000000000000,
        0x2020000000000000,
        0x4040000000000000,
        0x8080000000000000,
        0x0100000000000000,
        0x0200000000000000,
        0x0400000000000000,
        0x0800000000000000,
        0x1000000000000000,
        0x2000000000000000,
        0x4000000000000000,
        0x8000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000
        };

        const Bitboard down_from[util::NUM_SQUARES] =
        {
        0x0000000000000000,                          
        0x0000000000000000,                          
        0x0000000000000000,                          
        0x0000000000000000,                          
        0x0000000000000000,                          
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000001,
        0x0000000000000002,
        0x0000000000000004,
        0x0000000000000008,
        0x0000000000000010,
        0x0000000000000020,
        0x0000000000000040,
        0x0000000000000080,
        0x0000000000000101,
        0x0000000000000202,
        0x0000000000000404,
        0x0000000000000808,
        0x0000000000001010,
        0x0000000000002020,
        0x0000000000004040,
        0x0000000000008080,
        0x0000000000010101,
        0x0000000000020202,
        0x0000000000040404,
        0x0000000000080808,
        0x0000000000101010,
        0x0000000000202020,
        0x0000000000404040,
        0x0000000000808080,
        0x0000000001010100,
        0x0000000002020200,
        0x0000000004040400,
        0x0000000008080800,
        0x0000000010101000,
        0x0000000020202000,
        0x0000000040404000,
        0x0000000080808000,
        0x0000000101000000,
        0x0000000202000000,
        0x0000000404000000,
        0x0000000808000000,
        0x0000001010000000,
        0x0000002020000000,
        0x0000004040000000,
        0x0000008080000000,
        0x0000010000000000,
        0x0000020000000000,
        0x0000040000000000,
        0x0000080000000000,
        0x0000100000000000,
        0x0000200000000000,
        0x0000400000000000,
        0x0000800000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000
        };*/
    }
}
#endif