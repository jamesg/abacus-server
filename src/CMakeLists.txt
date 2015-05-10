file(GLOB server_sources *.cpp)
add_library(abacus-server-lib ${server_sources})
target_link_libraries(
    abacus-server-lib
    apollo-server-lib
    helios-server-lib
    )

