
##########################################################################################
###################################### Boost #############################################

set(BOOST_ROOT "C:/code/libraries/boost")
find_package(Boost 1.52.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(OINK_FENPARSER_SRC
	FenParser.hpp
	FenParser.cpp
)
	
add_library(OinkFenParser ${OINK_FENPARSER_SRC})

add_subdirectory(tests)