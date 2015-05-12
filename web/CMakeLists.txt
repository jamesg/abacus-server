atlas_add_web_library(
    PREFIX abacus
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/static.a
    SOURCES
        index.html
        )

add_custom_target(abacus-web ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/static.a)

