
include(${CMAKE_HOME_DIRECTORY}/cmake/AddGTestTest.cmake)

set(OINK_ENGINE_TESTS_SRC
	BasicOperationsTests.cpp
	PositionTests.cpp
	MoveGeneratorTests.cpp
	SearchTests.cpp
	PerftBasedTests.cpp
)
set(OINK_ENGINE_TESTS_LIBS OinkEngine OinkDisplay OinkFenParser)

ADD_GTEST_TEST(OinkEngineTests "${OINK_ENGINE_TESTS_SRC}" "${OINK_ENGINE_TESTS_LIBS}")