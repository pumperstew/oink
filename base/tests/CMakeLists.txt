
set(BASE_TESTS_SRC
	BasicOperationsTests.cpp
	PositionTests.cpp
	MoveGeneratorTests.cpp
)

include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIBRARY_DIR})
add_executable(BaseTests ${BASE_TESTS_SRC})
target_link_libraries(BaseTests base gtest.lib)
add_dependencies(BaseTests gtest)
add_test(BaseTests BaseTests)