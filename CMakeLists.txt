cmake_minimum_required(VERSION 2.8)

project(oink)

##################################### Google test ##########################################

include(ExternalProject)
ExternalProject_Add(
   gtest
   PREFIX gtest
   GIT_REPOSITORY https://github.com/google/googletest.git
   GIT_TAG release-1.8.1
   CMAKE_ARGS -Dgtest_force_shared_crt:BOOL=ON
   INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gtest SOURCE_DIR)
set(GTEST_INCLUDE_DIR ${SOURCE_DIR}/googletest/include)
message("${GTEST_INCLUDE_DIR}") 
ExternalProject_Get_Property(gtest BINARY_DIR)
set(GTEST_LIBRARY_DIR ${BINARY_DIR}/googlemock/gtest)
message("${GTEST_LIBRARY_DIR}")
# OINK_TODO: Windows specific and a bit ugly..older versions of gtest didn't 
# differentiate debug/release lib names, on Windows at least.
set(GTEST_LIBRARY gtest.lib)
set(GTEST_LIBRARY_DEBUG gtestd.lib)

##########################################################################################

enable_testing()

include_directories(.)

add_subdirectory(engine)
add_subdirectory(display)
add_subdirectory(test_harness)
add_subdirectory(magic_gen)
add_subdirectory(fen_parser)
add_subdirectory(winboard_driver)