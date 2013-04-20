#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "Position.hpp"
#include <iostream>
#include <vector>
#include <utility>
#include <string>

namespace chess
{
	void PrintPosition(const Position &position);
	void PrintBitboard(const bitboard bitboard, const char* title = nullptr, int highlightSquare = -1);
	void PrintBitboards(const std::vector<std::pair<bitboard, std::string>> &bitboards, int highlightSquare = -1);

	struct print_square
	{
		int sq;
		print_square(int sq_) : sq(sq_) {}
		std::ostream& operator()(std::ostream &stream) const;
	};
	std::ostream& operator<<(std::ostream &stream, const print_square &sqp);
}

#endif
