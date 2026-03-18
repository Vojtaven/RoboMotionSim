cmake_minimum_required(VERSION 3.24)
set(APP_NAME "RoboMotionSim")
set(APP_VERSION "0.6.0")

project(${APP_NAME} VERSION 1.0.0 LANGUAGES CXX)

# Set startup project for Visual Studio
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
    add_compile_options(/bigobj)
endif()
# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Useful variables
add_compile_definitions(
    APP_NAME="${APP_NAME}"
    APP_VERSION="${APP_VERSION}"
)

# Project Sources

file(GLOB project_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/src/app/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/app/*.hpp"
)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Platform-specific settings
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# ============================================================================
# Dependencies
# ============================================================================

include (${CMAKE_CURRENT_LIST_DIR}/cmake/dependencies.cmake)

# ============================================================================
# Create executable
# ============================================================================
add_executable(${PROJECT_NAME} ${project_sources})
add_subdirectory(src/configManager)
add_subdirectory(src/visualization)
add_subdirectory(src/physicsEngine)
add_subdirectory(src/core)
add_subdirectory(src/inputManager)

# ============================================================================
# Link libraries
# ============================================================================
target_link_libraries(${PROJECT_NAME} PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
    ImGui-SFML::ImGui-SFML
    imgui
    asio
    nfd
    cppzmq-static
    configManager
    visualization
    physicsEngine
    inputManager
    core
)

# Windows needs ws2_32 and wsock32 for Asio
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 wsock32)
endif()

# ============================================================================
# Platform-specific configurations
# ============================================================================
if(WIN32)
    # Copy SFML DLLs to output directory on Windows
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    if(NOT MSVC)
        target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
    endif()
endif()

# Linux-specific settings
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()

# ============================================================================
# Compiler warnings
# ============================================================================
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
endif()
# ============================================================================
# Installation
# ============================================================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

message(STATUS "Configuration complete!")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Serial communication: Asio serial_port")