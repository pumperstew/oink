set(OINK_FENPARSER_SRC
	FenParser.hpp
	FenParser.cpp
)
	
add_library(OinkFenParser ${OINK_FENPARSER_SRC})

add_subdirectory(tests)