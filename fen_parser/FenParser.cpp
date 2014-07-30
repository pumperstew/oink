#include "FenParser.hpp"

using namespace chess;

namespace chess { namespace fen 
{
    static void bad_fen(char what)
    {
        throw FenParseFailure(std::string("Bad FEN: '") + what + "' appeared in the wrong place");
    }

    static unsigned char move_count_to_int(const std::string &fen, int index, int count)
    {
        char buf[3] = { fen[index], 0, 0 };
        if (count == 2) 
            buf[1] = fen[index+1];
        return atoi(buf);
    }

	Position parse_fen(const std::string &fen, int *fullmove_count, Side *side_to_move)
	{
        Position pos;
        pos.castling_rights = 0;
        int halfmove_clock_char_count = 0;
        int fullmove_char_count = 0;
        RankFile ep_file = -1, ep_rank = -1;
        int move_num = 1;
        Side to_move = sides::white;

        enum ParseState
        {
            ExpectPiecePositions,
            ExpectToMove,
            ExpectCastlingRights,
            ExpectEPSquare,
            ExpectHalfMoveClock,
            ExpectFullMoveCounter,
            ExpectEnd,
        };
        
        ParseState state = ExpectPiecePositions;
        Square square = squares::a8;
        for (int i = 0; i < fen.size(); ++i)
        {
            char c = fen[i];
            Square square_step = 1;

            switch (c)
            {
            case 'r':
                if (state != ExpectPiecePositions)
                    bad_fen(c);
                else
                    pos.place_piece(pieces::BLACK_ROOK, square);
                break;
            case 'n':
                if (state != ExpectPiecePositions)
                    bad_fen(c);
                else
                    pos.place_piece(pieces::BLACK_KNIGHT, square);
                break;
            case 'b':
                if (state == ExpectToMove)
                    to_move = sides::black;
                else if (state == ExpectEPSquare)
                {
                    if (ep_file >= 0)
                        bad_fen(c);
                    ep_file = files::b;
                }
                else if (state == ExpectPiecePositions)
                    pos.place_piece(pieces::BLACK_BISHOP, square);
                else
                    bad_fen(c);
                break;
            case 'q':
                if (state == ExpectCastlingRights)
                    pos.castling_rights |= sides::CASTLING_RIGHTS_BLACK_QUEENSIDE;
                else if (state == ExpectPiecePositions)
                    pos.place_piece(pieces::BLACK_QUEEN, square);
                else
                    bad_fen(c);
                break;
            case 'k':
                if (state == ExpectCastlingRights)
                    pos.castling_rights |= sides::CASTLING_RIGHTS_BLACK_KINGSIDE;
                else if (state == ExpectPiecePositions)
                    pos.place_piece(pieces::BLACK_KING, square);
                else
                     bad_fen(c);
                break;
            case 'p':
                if (state != ExpectPiecePositions)
                    bad_fen(c);
                else
                    pos.place_piece(pieces::BLACK_PAWN, square);
                break;
            case 'R':
                if (state != ExpectPiecePositions)
                    bad_fen(c);
                else
                    pos.place_piece(pieces::WHITE_ROOK, square);
                break;
            case 'N':
                if (state != ExpectPiecePositions)
                    bad_fen(c);
                else
                    pos.place_piece(pieces::WHITE_KNIGHT, square);
                break;
            case 'B':
                if (state != ExpectPiecePositions)
                    bad_fen(c);
                else
                    pos.place_piece(pieces::WHITE_BISHOP, square);
                break;
            case 'Q':
                if (state == ExpectCastlingRights)
                    pos.castling_rights |= sides::CASTLING_RIGHTS_WHITE_QUEENSIDE;
                else if (state == ExpectPiecePositions)
                    pos.place_piece(pieces::WHITE_QUEEN, square);
                else
                    bad_fen(c);
                break;
            case 'K':
                if (state == ExpectCastlingRights)
                    pos.castling_rights |= sides::CASTLING_RIGHTS_WHITE_KINGSIDE;
                else if (state == ExpectPiecePositions)
                    pos.place_piece(pieces::WHITE_KING, square);
                else
                    bad_fen(c);
                break;
            case 'P':
                if (state != ExpectPiecePositions)
                    bad_fen(c);
                else
                    pos.place_piece(pieces::WHITE_PAWN, square);
                break;
            case '0':
                if (state == ExpectHalfMoveClock)
                    ++halfmove_clock_char_count;
                else if (state == ExpectFullMoveCounter)
                {
                    if (fullmove_char_count == 0) // Zero cannot be first.
                        bad_fen(c);
                    ++fullmove_char_count;
                }
                else
                    bad_fen(c);
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                if (state == ExpectHalfMoveClock)
                    ++halfmove_clock_char_count;
                else if (state == ExpectFullMoveCounter)
                     ++fullmove_char_count;
                else if (state == ExpectPiecePositions)
                    square_step = 1 + (c - '1');
                else if (state == ExpectEPSquare)
                {
                    if (ep_file < 0)
                        bad_fen(c);
                    ep_rank = ranks::first + (c - '1');
                    if (ep_rank != ranks::third && ep_rank != ranks::sixth)
                        bad_fen(c);
                }
                else
                    bad_fen(c);
                break;
            case '/':
                if (square % 8)
                    throw FenParseFailure("Bad FEN: incomplete rank");
                square_step = -16;
                break;
            case ' ':
                if (state == ExpectPiecePositions && square != squares::a2) // h1 + 1
                    throw FenParseFailure("Bad FEN: incomplete piece placements");
                if (state == ExpectHalfMoveClock)
                    pos.fifty_move_count = move_count_to_int(fen, i - halfmove_clock_char_count, halfmove_clock_char_count);
                else if (state == ExpectFullMoveCounter)
                    move_num = move_count_to_int(fen, i - fullmove_char_count, fullmove_char_count);
                else if (state == ExpectEPSquare)
                {
                    if (ep_file >= 0 && ep_rank < 0)
                        bad_fen(c); 
                    else if (ep_file >= 0 && ep_rank >= 0)
                        pos.ep_target_square = rank_file_to_square(ep_rank, ep_file);
                }
                state = (ParseState)((int)state + 1);
                break;
            case 'w':
                if (state != ExpectToMove)
                    bad_fen(c);
                to_move = sides::white;
                break;
            case 'a':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
                if (state != ExpectEPSquare)
                    bad_fen(c);
                if (ep_file >= 0)
                    bad_fen(c);
                ep_file = files::a + (c - 'a');
                break;
            case '-':
                if (state == ExpectCastlingRights)
                {
                    if (pos.castling_rights)
                        bad_fen(c);
                }
                else if (state == ExpectEPSquare)
                    pos.ep_target_square = squares::NO_SQUARE;
                else
                    bad_fen(c);
                break;
            default:
                throw FenParseFailure("Unexpected character: " + c);
            }

            if (state == ExpectPiecePositions)
                square += square_step;
            if (state == ExpectEnd)
                break;
        }

        if (square != squares::a2)
            throw FenParseFailure("Bad FEN: incomplete board");

        if (state == ExpectHalfMoveClock && halfmove_clock_char_count)
            pos.fifty_move_count = move_count_to_int(fen, (int)fen.size() - halfmove_clock_char_count, halfmove_clock_char_count);

        if (state == ExpectFullMoveCounter && fullmove_char_count)
            move_num = move_count_to_int(fen, (int)fen.size() - fullmove_char_count, fullmove_char_count);

        if (fullmove_count)
            *fullmove_count = move_num;

        if (side_to_move)
            *side_to_move = to_move;

        pos.update_sides();
		return pos;
	}
}}