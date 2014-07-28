
include(${CMAKE_HOME_DIRECTORY}/cmake/AddGTestTest.cmake)

set(OINK_FENPARSER_TESTS_SRC
	FenParserTests.cpp
)
set(OINK_FENPARSER_TESTS_LIBS OinkFenParser OinkEngine OinkDisplay)

ADD_GTEST_TEST(OinkFenParserTests "${OINK_FENPARSER_TESTS_SRC}" "${OINK_FENPARSER_TESTS_LIBS}")