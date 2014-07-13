
set(ENGINE_TESTS_SRC
	BasicOperationsTests.cpp
	PositionTests.cpp
	MoveGeneratorTests.cpp
)

include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIBRARY_DIR})
add_executable(EngineTests ${ENGINE_TESTS_SRC})
target_link_libraries(EngineTests engine console_display gtest.lib)
add_dependencies(EngineTests gtest)
add_test(EngineTests EngineTests)