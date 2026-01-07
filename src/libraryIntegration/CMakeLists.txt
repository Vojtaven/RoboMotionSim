message(STATUS "Making: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB LIBRARY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

set(CONFIG_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/outTestConfig.ini"
    "${CMAKE_CURRENT_SOURCE_DIR}/OmniConfig.ini"
    "${CMAKE_CURRENT_SOURCE_DIR}/DifferentialConfig.ini"
    "${CMAKE_CURRENT_SOURCE_DIR}/MecanumConfig.ini"
)

set(MODULE_NAME libraryIntegration)

add_library(${MODULE_NAME} STATIC ${LIBRARY_SOURCES} ${CONFIG_FILES})

target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${MODULE_NAME} PUBLIC exprtk)
target_link_libraries(${MODULE_NAME} PUBLIC core)
set_target_properties(${MODULE_NAME} PROPERTIES FOLDER "Project Modules")

source_group("Config Files" FILES ${CONFIG_FILES})