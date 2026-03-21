gather_module_sources()

set(MODULE_NAME inputManager)
add_library(${MODULE_NAME} STATIC
    ${LIBRARY_SOURCES}
)
# Link required libraries
target_link_libraries(${MODULE_NAME} PRIVATE
    sfml-system
    sfml-window
	asio
    cppzmq-static
)

target_link_libraries(${MODULE_NAME} PUBLIC
    core
)

target_include_directories(${MODULE_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)
finalize_module(${MODULE_NAME})
