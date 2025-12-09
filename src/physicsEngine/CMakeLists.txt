message(STATUS "Making: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB LIBRARY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

set(MODULE_NAME physicsEngine)

add_library(${MODULE_NAME} STATIC ${LIBRARY_SOURCES} )
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${MODULE_NAME} PROPERTIES FOLDER "Project Modules")