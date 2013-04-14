#include "Display.hpp"
#include "ChessConstants.hpp"
#include "BasicOperations.hpp"

#include <cstdio>
#include <Windows.h>

namespace chess
{
	//void print_board(const board &b)
	//{
	//	printf("\n");
	//	for (int r = 7; r >= 0; --r) {
	//		for (int c = 0; c < 8; ++c) {
	//			printf("%c ", b.get_at(r,c));
	//		}
	//		printf("\n");
	//	}
	//}

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
	void PrintBitboard(const bitboard bitboard, const char* title, int highlightSquare)
	{
#ifndef DISPLAY_DISABLED
		auto stdOutHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);

		printf("\n");
		if (title) printf("%s\n", title);

		for (int rank = 7; rank >= 0; --rank)
		{
			for (int file = 0; file < util::BOARD_SIZE; ++file)
			{
				int index = RankFileToIndex(rank, file);
				auto value = (bitboard >> index) & util::one;
				if (index == highlightSquare)
				{
					::SetConsoleTextAttribute(stdOutHandle, ConsoleColours::RED);
				} 
				else if (value)
				{
					::SetConsoleTextAttribute(stdOutHandle, ConsoleColours::BLUE);
				}
				printf("%d ", (bitboard >> index) & util::one);
				::SetConsoleTextAttribute(stdOutHandle, ConsoleColours::LGREY);
			}
			printf("\n");
		}
#endif
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