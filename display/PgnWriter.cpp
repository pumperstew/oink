#include "PgnWriter.hpp"

#include <engine/BasicOperations.hpp>

namespace chess
{
    void pgn_out_move(std::ostream &stream, Move move, int move_num, Side side, util::PositionType pos_characteristics)
    {
        if (side == sides::white)
            stream << move_num << ". ";

        const char *suffixes[] = { "", "+", "#", " 1/2-1/2", " 1/2-1/2" };
        const char *suffix     = suffixes[pos_characteristics];

        if (move.get_castling() != pieces::NONE)
        {
            if (move.get_destination() == squares::c1 || move.get_destination() == squares::c8)
                stream << "O-O-O";
            else
                stream << "O-O";
        }
        else
        {
            Piece moving_piece = move.get_piece();
            if (moving_piece != pieces::WHITE_PAWN && moving_piece != pieces::BLACK_PAWN)
            {
                stream << (char)toupper(pieces::symbols[moving_piece]);
            }

            RankFile rank, file;
            Square source = move.get_source();
            square_to_rank_file(source, rank, file);

            stream << (char)('a' + file);
            stream << (char)('1' + rank);

            stream << (move.get_captured_piece() != pieces::NONE ? "x" : "-");

            Square dest = move.get_destination();
            square_to_rank_file(dest, rank, file);

            stream << (char)('a' + file);
            stream << (char)('1' + rank);

            if (move.get_promotion_piece() != pieces::NONE)
            {
                stream << "=" << (char)toupper(pieces::symbols[move.get_promotion_piece()]);
            }
        }

        stream << suffix << ' ';
        if (side == sides::black)
            stream << '\n';
    }
}