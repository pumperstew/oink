#include "Display.hpp"
#include "Position.hpp"
#include "BasicOperations.hpp"

#include <cstdio>

#include <Windows.h>

using namespace std;

namespace chess
{
	void print_position(const Position &position)
	{
        puts("");

        char this_rank_buf[(util::BOARD_SIZE << 1) + 1];
        memset(this_rank_buf, ' ', sizeof(this_rank_buf));

		for (RankFile rank = util::BOARD_SIZE - 1; rank >= 0; --rank)
		{
			for (RankFile file = 0; file < util::BOARD_SIZE; ++file)
			{
                Piece what = position.squares[rank_file_to_square(rank, file)];
                this_rank_buf[file << 1] = pieces::symbols[what];
			}
            this_rank_buf[util::BOARD_SIZE << 1] = 0;
			puts(this_rank_buf);
		}
	}

	namespace console_colours
	{
		enum Values 
		{ 
			DBLUE=1,GREEN,GREY,DRED,DPURP,BROWN,LGREY,DGREY,BLUE,LIMEG,TEAL,RED,PURPLE,YELLOW,WHITE,B_B 
		};
	}

	void print_bitboards(const vector<pair<Bitboard, string>> &boards, Square highlight_square)
	{
#ifndef DISPLAY_DISABLED
		HANDLE std_out_handle = ::GetStdHandle(STD_OUTPUT_HANDLE);

		vector<string> offsets;
		printf("\n");
		for (size_t i = 0; i < boards.size(); ++i)
		{
			int pad_length = 0, title_pad_length = 0;
			if (boards[i].second.size() < 16)
            {
				title_pad_length = 16 - (int)boards[i].second.size() + 1;
            }
			else 
            {
				pad_length = (int)boards[i].second.size() - 16 + 1;
            }

			string title_pad(max(title_pad_length, 1), ' ');
			string pad(max(pad_length, 1), ' ');
			offsets.push_back(pad);
			printf("%s%s", boards[i].second.c_str(), title_pad.c_str());
		}
		printf("\n");

		for (RankFile rank = util::BOARD_SIZE - 1; rank >= 0; --rank)
		{
			for (size_t j = 0; j < boards.size(); ++j)
			{
				for (RankFile file = 0; file < util::BOARD_SIZE; ++file)
				{
					Square index = rank_file_to_square(rank, file);
					Bitboard value = (boards[j].first >> index) & util::one;
					if (index == highlight_square)
					{
						::SetConsoleTextAttribute(std_out_handle, console_colours::RED);
					} 
					else if (value)
					{
						::SetConsoleTextAttribute(std_out_handle, console_colours::BLUE);
					}
					printf("%d ", (int)value);
					::SetConsoleTextAttribute(std_out_handle, console_colours::LGREY);
				}
				printf("%s", offsets[j].c_str());
			}
			printf("\n");
		}
#endif
	}
	
	void print_bitboard(Bitboard board, const char* title, Square highlight_square)
	{
		vector<pair<Bitboard, string>> bitboards(1, make_pair(board, title ? title : ""));
		print_bitboards(bitboards, highlight_square);
	}

    void print_bitboard(const Bitboard b, const char* title)
    {
        print_bitboard(b, title, squares::NO_SQUARE);
    }

    void print_bitboard(const Bitboard b)
    {
        print_bitboard(b, "", squares::NO_SQUARE);
    }

    void print_bitboard(const Bitboard b, Square highlight_square)
    {
        print_bitboard(b, nullptr, highlight_square);
    }

    void print_move(Move move, int move_num)
    {
        printf("\n%d. ", move_num);

        if (move.get_castling() != pieces::NONE)
        {
            if (move.get_destination() == squares::c1 || move.get_destination() == squares::c8)
            {
                puts("O-O-O");
            }
            else
            {
                puts("O-O");
            }
            return;

        }

        string algebraic;

        Piece moving_piece = move.get_piece();
        if (moving_piece != pieces::WHITE_PAWN && moving_piece != pieces::BLACK_PAWN)
        {
            algebraic = pieces::symbols[moving_piece];
        }

        Square source = move.get_source();
        RankFile rank, file;
        square_to_rank_file(source, rank, file);

        algebraic += 'a' + file;
        algebraic += '1' + rank;

        algebraic += move.get_captured_piece() != pieces::NONE ? "x" : "-";

        Square dest = move.get_destination();
        square_to_rank_file(dest, rank, file);

        algebraic += 'a' + file;
        algebraic += '1' + rank;

        if (move.get_en_passant() != pieces::NONE)
        {
            algebraic += "ep";
        }
        else if (move.get_promotion_piece() != pieces::NONE)
        {
            algebraic += "=";
            algebraic += pieces::symbols[move.get_promotion_piece()];
        }

        puts(algebraic.c_str());
    }
}