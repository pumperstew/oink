
set(ENGINE_SRC
    BasicTypes.hpp
    ChessConstants.hpp
    ChessConstants.cpp
    BasicOperations.hpp
    BasicOperations.cpp
    Position.hpp
    Position.cpp
    Move.hpp
    MoveGenerator.hpp
    MoveGenerator.cpp
    Display.hpp
    Display.cpp
)
    
add_library(engine ${ENGINE_SRC})

add_subdirectory(tests)
add_subdirectory(test_harness)