
MACRO(ADD_GTEST_TEST name sources link_libs)
	include_directories(${GTEST_INCLUDE_DIR})
	link_directories(${GTEST_LIBRARY_DIR})
	add_executable(${name} ${sources})
	target_link_libraries(${name} ${link_libs} ${GTEST_LIBRARY})
	add_dependencies(${name} gtest)
	add_test(${name} ${name})
ENDMACRO()
