
set(BASE_SRC
	BasicTypes.hpp
	ChessConstants.hpp
	ChessConstants.cpp
	BasicOperations.hpp
	BasicOperations.cpp
	Position.hpp
	Position.cpp
	Move.hpp
	Move.cpp
	MoveGenerator.hpp
	MoveGenerator.cpp
)
	
add_library(base ${BASE_SRC})

add_subdirectory(tests)