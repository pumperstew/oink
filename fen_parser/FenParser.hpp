#ifndef FENPARSER_HPP
#define FENPARSER_HPP

#include <engine/Position.hpp>

#include <string>
#include <vector>

namespace chess
{
    namespace fen
    {
	    chess::Position parse_fen(std::string fen);

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