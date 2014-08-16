#include "ConsoleDisplay.hpp"

#include <engine/Position.hpp>
#include <engine/BasicOperations.hpp>

#include <cstdio>
#include <tuple>

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

    // ifdef windows, etc..

    typedef HANDLE OSHandleType;

    static OSHandleType get_std_handle()
    {
        return ::GetStdHandle(STD_OUTPUT_HANDLE);
    }

    static void colour_stdout(OSHandleType std_out_handle, console_colours::Values colour)
    {
        ::SetConsoleTextAttribute(std_out_handle, colour);
    }

	void print_bitboards(const vector<pair<Bitboard, string>> &boards, Square highlight_square)
	{
		auto std_out_handle = get_std_handle();

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
						colour_stdout(std_out_handle, console_colours::RED);
					} 
					else if (value)
					{
						colour_stdout(std_out_handle, console_colours::BLUE);
					}
					printf("%d ", (int)value);
					colour_stdout(std_out_handle, console_colours::LGREY);
				}
				printf("%s", offsets[j].c_str());
			}
			printf("\n");
		}
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

    static std::tuple<RankFile, RankFile, RankFile, RankFile> get_source_dest_rank_and_file(Move move)
    {
        RankFile rs, fs, rd, fd;
        Square source = move.get_source();
        square_to_rank_file(source, rs, fs);

        Square dest = move.get_destination();
        square_to_rank_file(dest, rd, fd);

        return make_tuple(rs, fs, rd, fd);
    }

    void print_move(Move move, int move_num, Side side, util::PositionType pos_characteristics, PosEvaluationFrac eval)
    {
        const char * const suffixes[] = { "", "+", "#", " 1/2-1/2 (stalemate)", " 1/2-1/2 (insufficient material)" };
        
        const char * const prefix = side == sides::black ? ".." : "";
        const char * const suffix = suffixes[pos_characteristics];

        if (move.is_queenside_castle())
        {
            printf("\n%d.%s O-O-O%s (%+.2f)\n", move_num, prefix, suffix, eval);
            return;
        }
        else if (move.is_kingside_castle())
        {
            printf("\n%d.%s O-O%s (%+.2f)\n", move_num, prefix, suffix, eval);
            return;
        }

        char algebraic[9];
        char *c = algebraic;

        Piece moving_piece = move.get_piece();
        if (moving_piece != pieces::WHITE_PAWN && moving_piece != pieces::BLACK_PAWN)
        {
            *c++ = pieces::symbols[moving_piece];
        }

        auto rank_file_info = get_source_dest_rank_and_file(move);

        *c++ = (char)('a' + get<1>(rank_file_info));
        *c++ = (char)('1' + get<0>(rank_file_info));
        *c++ = move.get_captured_piece() != pieces::NONE ? 'x' : '-';
        *c++ = (char)('a' + get<3>(rank_file_info));
        *c++ = (char)('1' + get<2>(rank_file_info));

        if (move.get_en_passant() != pieces::NONE)
        {
            *c++ = 'e';
            *c++ = 'p';
        }
        else if (move.get_promotion_piece() != pieces::NONE)
        {
            *c++ = '=';
            *c++ = pieces::symbols[move.get_promotion_piece()];
        }

        *c = 0;

        printf("\n%d.%s %s%s (%+.2f)\n", move_num, prefix, algebraic, suffix, eval);
    }

    // Outputs a minimal coord-string, like e2e4, a7a8q, etc.
    string move_to_coordtext(Move move)
    {
        if (move.is_queenside_castle())
            return "O-O-O";
        else if (move.is_kingside_castle())
            return "O-O";

        char result[6] = {0};
        auto rank_file_info = get_source_dest_rank_and_file(move);

        result[0] = (char)('a' + get<1>(rank_file_info));
        result[1] = (char)('1' + get<0>(rank_file_info));
        result[2] = (char)('a' + get<3>(rank_file_info));
        result[3] = (char)('1' + get<2>(rank_file_info));

        if (move.get_promotion_piece() != pieces::NONE)
            result[4] = (char)tolower(pieces::symbols[move.get_promotion_piece()]);

        return result;
    }
}