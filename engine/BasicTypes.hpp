#ifndef BASICTYPES_HPP
#define BASICTYPES_HPP

#include <cinttypes>

#define OINK_INLINE inline

//#define OINK_MOVEGEN_DIAGNOSTICS

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

#ifdef OINK_USE_NARROWEST_TYPES
    typedef unsigned char Piece;
    typedef char          Square;
    typedef char          RankFile;
    typedef char          Side;
#else
	typedef unsigned char Piece; // This should be narrow, as we have 64 of them in a Position.
    typedef int_fast32_t  Square;
    typedef char          SquareLeastT;
    typedef int_fast32_t  RankFile;
    typedef int_fast32_t  Side;
#endif
}

#endif // BASICTYPES_HPP