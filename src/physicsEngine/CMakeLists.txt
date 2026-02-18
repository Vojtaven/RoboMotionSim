message(STATUS "Making: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE LIBRARY_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

set(MODULE_NAME physicsEngine)
add_library(${MODULE_NAME} STATIC ${LIBRARY_SOURCES} )

target_link_libraries(${MODULE_NAME} PUBLIC core)

target_include_directories(${MODULE_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIBRARY_SOURCES})
set_target_properties(${MODULE_NAME} PROPERTIES FOLDER "Project Modules")