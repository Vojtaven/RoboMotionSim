gather_module_sources()

add_library(core STATIC ${LIBRARY_SOURCES})

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
finalize_module(core)
