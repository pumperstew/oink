
set(OINK_ENGINE_SRC
    BasicTypes.hpp
    ChessConstants.hpp
    ChessConstants.cpp
    BasicOperations.hpp
    Position.hpp
    Position.cpp
    Move.hpp
    MoveGenerator.hpp
    MoveGenerator.cpp
	Evaluator.hpp
	Evaluator.cpp
	Search.hpp
	Search.cpp
	Perft.hpp
	Perft.cpp
)


add_library(OinkEngine ${OINK_ENGINE_SRC})
# target_compile_options(engine PRIVATE $<$<CONFIG:Release>:/arch:AVX>)

add_subdirectory(tests)
