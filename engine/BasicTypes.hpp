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

#ifdef OINK_USE_NARROWEST_TYPES
    typedef unsigned char Piece;
    typedef char          Square;
    typedef char          RankFile;
    typedef char          Side;
    typedef char          PieceValue;
#else
	typedef unsigned char Piece; // This should be narrow, as we have 64 of them in a Position.
    typedef int_fast32_t  Square;
    typedef char          SquareLeastT;
    typedef int_fast32_t  RankFile;
    typedef int_fast32_t  Side;
    typedef int_fast32_t  PieceValue;
#endif

    typedef int PosEvaluation;
    typedef double PosEvaluationFrac;
}

#endif // BASICTYPES_HPP