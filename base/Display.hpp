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
	void PrintBitboard(const bitboard bitboard, const char* title = nullptr, Square highlightSquare = squares::NO_SQUARE);
	void PrintBitboards(const std::vector<std::pair<bitboard, std::string>> &bitboards, Square highlightSquare = squares::NO_SQUARE);

	struct print_square
	{
		Square sq;
		print_square(Square sq_) : sq(sq_) {}
		std::ostream& operator()(std::ostream &stream) const;
	};
	std::ostream& operator<<(std::ostream &stream, const print_square &sqp);
}

#endif
