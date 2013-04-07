cmake_minimum_required(VERSION 2.8)

project(oink)

add_definitions(/D _VARIADIC_MAX=10) #VS2012 hack for Google test compilation. It's also needed on code compiling *AGAINST* Google test.

##################################### Google test ##########################################

include(ExternalProject)
ExternalProject_Add(
   gtest
   PREFIX gtest
   GIT_REPOSITORY git://github.com/narvi/googletest.git
   GIT_TAG vc11
   CMAKE_ARGS -Dgtest_force_shared_crt:BOOL=ON
   INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gtest SOURCE_DIR)
set(GTEST_INCLUDE_DIR ${SOURCE_DIR}/include)
message("${GTEST_INCLUDE_DIR}") 
ExternalProject_Get_Property(gtest BINARY_DIR)
set(GTEST_LIBRARY_DIR ${BINARY_DIR})
message("${GTEST_LIBRARY_DIR}") 

##########################################################################################
###################################### Boost #############################################

set(BOOST_ROOT "C:/code/libraries/boost")
find_package(Boost 1.52.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

##########################################################################################

enable_testing()

add_subdirectory(base)