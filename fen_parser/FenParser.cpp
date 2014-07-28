#include "FenParser.hpp"

#include <engine/ChessConstants.hpp>

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_container.hpp>

using namespace boost::spirit;
using namespace boost;
using namespace chess;

namespace chess { namespace fen 
{
	struct pieces_ : qi::symbols<char, Piece>
	{
		pieces_()
		{
			add
				("r"   , pieces::BLACK_ROOK)
				("n"   , pieces::BLACK_KNIGHT)
				("b"   , pieces::BLACK_BISHOP)
				("q"   , pieces::BLACK_QUEEN)
				("k"   , pieces::BLACK_KING)
				("p"   , pieces::BLACK_PAWN)
				("P"   , pieces::WHITE_PAWN)
				("R"   , pieces::WHITE_ROOK)
				("N"   , pieces::WHITE_KNIGHT)
				("B"   , pieces::WHITE_BISHOP)
				("Q"   , pieces::WHITE_QUEEN)
				("K"   , pieces::WHITE_KING)
			;
		}

	} qi_pieces;

	template <typename Iterator>
	struct fen : qi::grammar<Iterator, std::vector<int>()>
	{
		fen() : fen<Iterator>::base_type(fenRule)
		{
			using qi::int_;
			using qi::eps;
			using qi::lit;
			using qi::_val;
			using qi::_1;
			using qi::repeat;
			using ascii::char_;
			using phoenix::end;
			using phoenix::push_back;
			using phoenix::insert;
			using phoenix::size;
			//TODO: use a list parser to simplify?
			//TODO: missing trailing '/' - causes failure

			rankElement =
				qi_pieces		[push_back(_val, _1)]
				| int_		    [insert(_val, end(_val), _1, 0)]
			;

			rank = +rankElement;

			newRank = eps(size(_val) % 8 == 0) >> lit('/');

			fenRule = repeat(8)[rank > newRank];

			/*BOOST_SPIRIT_DEBUG_NODE(rankElement);
			debug(rankElement);
			BOOST_SPIRIT_DEBUG_NODE(rank);
			debug(rank);
			BOOST_SPIRIT_DEBUG_NODE(newRank);
			debug(newRank);
			BOOST_SPIRIT_DEBUG_NODE(fenRule);
			debug(fenRule);*/
		}

		qi::rule<Iterator, std::vector<int>()> fenRule, rank, rankElement, newRank;
	};

/*
start =
			repeat(8)[
			+(
                pieces		    [push_back(_val, _1)]
                |  int_		    [insert(_val, end(_val), _1, 0)]
            )
			> 
			(eps((size(_val)  % 8) == 0) >> char_('/'))
			]
        ;
*/
   
	Position parse_fen(std::string fenString)
	{
		std::vector<Piece> fenResult;
		fen<std::string::iterator> fenParser;
		bool ok;
		try
		{
			ok = qi::parse(fenString.begin(), fenString.end(), fenParser, fenResult);
		}
		catch (const std::exception &ex)
		{
			throw FenParseFailure(ex.what());
		}
		
		if (!ok)
			throw FenParseFailure("Fen failed to parse (non-throwing)");

        Position pos;
        int i = 0;
        for (RankFile rank = util::BOARD_SIZE - 1; rank >= 0; --rank)
        {
            for (RankFile file = 0; file < util::BOARD_SIZE; ++file)
            {
                Square square = rank_file_to_square(rank, file);
                pos.squares[square] = fenResult[i];

                pos.piece_bbs[fenResult[i]] |= util::one << square;
                ++i;
            }
        }
        pos.update_sides();
		return pos;
	}
}}