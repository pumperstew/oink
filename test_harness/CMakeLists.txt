
set(OINK_TEST_HARNESS_SRC
    OinkTestHarness.hpp
    OinkTestHarness.cpp
)

add_executable(OinkTestHarness ${OINK_TEST_HARNESS_SRC})
target_link_libraries(OinkTestHarness OinkEngine OinkDisplay OinkFenParser)
