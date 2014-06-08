#include "Display.hpp"
#include "ChessConstants.hpp"
#include "BasicOperations.hpp"

#include <cstdio>
#include <Windows.h>

#include <boost/assign.hpp>

using namespace std;
namespace ba = boost::assign;

namespace chess
{
	void PrintPosition(const Position &position)
	{
		printf("\n");
		for (RankFile rank = 7; rank >= 0; --rank)
		{
			for (RankFile file = 0; file < 8; ++file)
			{
				printf("%c ", pieces::PieceSymbols[position.GetSquare(RankFileToIndex(rank, file))]);
			}
			printf("\n");
		}
	}

	//void print_boards(const position_list &brds)
	//{
	//	printf("printing %d positions:\n", brds.size());
	//	for (int r = 7; r >= 0; --r) {
	//		for (position_list::const_iterator b = brds.begin(); b != brds.end(); ++b) {
	//			for (int c = 0; c < 8; ++c) {
	//				printf("%c ", b->get_at(r,c));
	//			}
	//			printf(" ");
	//		}
	//		printf("\n");
	//	}
	//}

	namespace ConsoleColours
	{
		enum Values 
		{ 
			DBLUE=1,GREEN,GREY,DRED,DPURP,BROWN,LGREY,DGREY,BLUE,LIMEG,TEAL,RED,PURPLE,YELLOW,WHITE,B_B 
		};
	}
//#define DISPLAY_DISABLED
	void PrintBitboards(const vector<pair<bitboard, string>> &bitboards, Square highlightSquare)
	{
#ifndef DISPLAY_DISABLED
		auto stdOutHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);

		vector<string> offsets;
		printf("\n");
		for (size_t i = 0; i < bitboards.size(); ++i)
		{
			int padLength = 0, titlePadLength = 0;
			if (bitboards[i].second.size() < 16) 
				titlePadLength = 16 - bitboards[i].second.size() + 1;
			else 
				padLength = bitboards[i].second.size() - 16 + 1;

			string titlePad(max(titlePadLength, 1), ' ');
			string pad(max(padLength, 1), ' ');
			offsets.push_back(pad);
			printf("%s%s", bitboards[i].second.c_str(), titlePad.c_str());
		}
		printf("\n");

		for (RankFile rank = 7; rank >= 0; --rank)
		{
			for (size_t j = 0; j < bitboards.size(); ++j)
			{
				for (RankFile file = 0; file < util::BOARD_SIZE; ++file)
				{
					Square index = RankFileToIndex(rank, file);
					auto value = (bitboards[j].first >> index) & util::one;
					if (index == highlightSquare)
					{
						::SetConsoleTextAttribute(stdOutHandle, ConsoleColours::RED);
					} 
					else if (value)
					{
						::SetConsoleTextAttribute(stdOutHandle, ConsoleColours::BLUE);
					}
					printf("%d ", (int)value);
					::SetConsoleTextAttribute(stdOutHandle, ConsoleColours::LGREY);
				}
				printf("%s", offsets[j].c_str());
			}
			printf("\n");
		}
#endif
	}
	
	void PrintBitboard(bitboard board, const char* title, Square highlightSquare)
	{
		vector<pair<bitboard, string>> bitboards = ba::list_of(make_pair(board, title));
		PrintBitboards(bitboards, highlightSquare);
	}

	/*char file2letter[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

	std::ostream& print_square::operator()(std::ostream &stream) const
	{
		int rank = 1 + sq / 8;
		int file = sq % 8;
		stream << file2letter[file] << rank;
		return stream;
	}

	std::ostream& operator<<(std::ostream &stream, const print_square &sqp)
	{
		return sqp(stream);
	}*/
}