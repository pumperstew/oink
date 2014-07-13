
set(OINK_MAGIC_GEN_SRC
    MagicGen.cpp
)

add_executable(OinkMagicGen ${OINK_MAGIC_GEN_SRC})
target_link_libraries(OinkMagicGen engine console_display)
