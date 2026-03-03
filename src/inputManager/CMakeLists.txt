message(STATUS "Making: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE LIBRARY_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)


set(MODULE_NAME inputManager)
add_library(${MODULE_NAME} STATIC 
    ${LIBRARY_SOURCES}
)
# Link required libraries
target_link_libraries(${MODULE_NAME} PUBLIC
    sfml-system
    sfml-window
	asio
    core
)
 
target_include_directories(${MODULE_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIBRARY_SOURCES})
source_group("Generated font headers" FILES ${FONT_OUT_FILE})
set_target_properties(${MODULE_NAME} PROPERTIES FOLDER "Project Modules")