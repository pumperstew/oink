#ifndef FENPARSER_HPP
#define FENPARSER_HPP

#include <engine/Position.hpp>

#include <string>

namespace chess
{
    namespace fen
    {
	    chess::Position parse_fen(const std::string &fen, int *fullmove_count = nullptr, Side *side_to_move = nullptr);

	    class FenParseFailure : public std::runtime_error
	    {
	    public:
		    FenParseFailure(const std::string &message)
			    : std::runtime_error(message)
		    {}
	    };
    }
}

#endif