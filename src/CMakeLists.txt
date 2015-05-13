file(GLOB server_sources *.cpp)
add_library(abacus-server-lib ${server_sources})
target_link_libraries(
    abacus-server-lib
    ${CMAKE_CURRENT_BINARY_DIR}/../web/static.a
    apollo-server-lib
    chronos-server-lib
    helios-server-lib
    )

