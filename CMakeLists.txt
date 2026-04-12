cmake_minimum_required(VERSION 3.24)
# Ensure MSVC runtime library is controlled per-target (required for static runtime)
cmake_policy(SET CMP0091 NEW)
set(APP_NAME "RoboMotionSim")
set(APP_VERSION "0.6.0")

project(${APP_NAME} VERSION ${APP_VERSION} LANGUAGES CXX)

# Configure git hooks path so pre-commit hook runs from the tracked git-hooks/ directory
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} config core.hooksPath git-hooks
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

# Set startup project for Visual Studio
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
    add_compile_options(/bigobj)
endif()
# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Static linking - produce a single .exe with no DLL dependencies
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)

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

# ============================================================================
# Dependencies
# ============================================================================

include (${CMAKE_CURRENT_LIST_DIR}/cmake/dependencies.cmake)

# ============================================================================
# Create executable
# ============================================================================
if(WIN32)
    set(APP_ICON_RC "${CMAKE_CURRENT_SOURCE_DIR}/src/assets/app.rc")
endif()
add_executable(${PROJECT_NAME} ${project_sources} ${APP_ICON_RC})
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
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
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
    if(WIN32)
        target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
    endif()
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