#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "BasicTypes.hpp"
#include "ChessConstants.hpp"

#include <vector>
#include <utility>
#include <string>

namespace chess
{
    class Position;

	void print_position(const Position &position);

    void print_bitboard(const Bitboard bitboard, Square highlight_square);
    void print_bitboard(const Bitboard bitboard, const char* title);
    void print_bitboard(const Bitboard bitboard, const char* title, Square highlight_square);

	void print_bitboards(const std::vector<std::pair<Bitboard, std::string>> &bitboards, Square highlight_square = squares::NO_SQUARE);
}

#endif
