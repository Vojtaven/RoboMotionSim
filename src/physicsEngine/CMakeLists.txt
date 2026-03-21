gather_module_sources()

set(MODULE_NAME physicsEngine)
add_library(${MODULE_NAME} STATIC ${LIBRARY_SOURCES} )

target_link_libraries(${MODULE_NAME} PUBLIC core)

target_include_directories(${MODULE_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
finalize_module(${MODULE_NAME})
