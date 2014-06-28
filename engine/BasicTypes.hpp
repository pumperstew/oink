#ifndef BASICTYPES_HPP
#define BASICTYPES_HPP

#include <cinttypes>

#define OINK_INLINE inline

#ifdef _MSC_VER
    #ifdef _WIN64
        #define OINK_MSVC_64
    #else
        #define OINK_MSVC_32
    #endif
#endif

namespace chess
{
	typedef uint64_t      Bitboard;
	typedef unsigned char Piece;
    typedef char          Square;
    typedef char          RankFile;
    typedef char          Side;
}

#endif