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

        char this_rank_buf[util::BOARD_SIZE + 1];
		for (RankFile rank = util::BOARD_SIZE - 1; rank >= 0; --rank)
		{
			for (RankFile file = 0; file < util::BOARD_SIZE; ++file)
			{
                Piece what = position.squares[rank_file_to_square(rank, file)];
                this_rank_buf[file] = pieces::symbols[what];
			}
            this_rank_buf[util::BOARD_SIZE] = 0;
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

    void print_bitboard(const Bitboard b, Square highlight_square)
    {
        print_bitboard(b, nullptr, highlight_square);
    }
}