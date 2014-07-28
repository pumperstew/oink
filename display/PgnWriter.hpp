#ifndef PGN_WRITER_HPP
#define PGN_WRITER_HPP

#include <engine/ChessConstants.hpp>
#include <engine/Move.hpp>

#include <ostream>

namespace chess
{
    void pgn_out_move(std::ostream &stream, Move move, int move_num, Side side, util::PositionType pos_characteristics);
}

#endif // PGN_WRITER_HPP
