cmake_minimum_required(VERSION 2.8)

project(oink)

##################################### Google test ##########################################

include(ExternalProject)
ExternalProject_Add(
   gtest
   PREFIX gtest
   GIT_REPOSITORY git://github.com/narvi/googletest.git
   GIT_TAG oink
   CMAKE_ARGS -Dgtest_force_shared_crt:BOOL=ON
   INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gtest SOURCE_DIR)
set(GTEST_INCLUDE_DIR ${SOURCE_DIR}/include)
message("${GTEST_INCLUDE_DIR}") 
ExternalProject_Get_Property(gtest BINARY_DIR)
set(GTEST_LIBRARY_DIR ${BINARY_DIR})
message("${GTEST_LIBRARY_DIR}")
set(GTEST_LIBRARY gtest.lib)

##########################################################################################

enable_testing()

include_directories(.)

add_subdirectory(engine)
add_subdirectory(display)
add_subdirectory(test_harness)
add_subdirectory(magic_gen)
add_subdirectory(fen_parser)