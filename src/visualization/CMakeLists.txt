message(STATUS "Making: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB LIBRARY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

set(MODULE_NAME Visualization)

add_library(${MODULE_NAME} STATIC ${LIBRARY_SOURCES})

set_target_properties(${MODULE_NAME} PROPERTIES FOLDER "Project Modules")