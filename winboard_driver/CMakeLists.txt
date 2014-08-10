
set(OINK_WINBOARD_DRIVER_SRC
    OinkWinboardDriver.hpp
    OinkWinboardDriver.cpp
)

add_executable(OinkWinboardDriver ${OINK_WINBOARD_DRIVER_SRC})
target_link_libraries(OinkWinboardDriver OinkEngine OinkDisplay OinkFenParser)
