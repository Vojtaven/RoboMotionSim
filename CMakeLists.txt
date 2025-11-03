cmake_minimum_required(VERSION 3.24)
project(RoboMotionSim VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Libraries versions
set(sfml_tag_version 3.0.2)
set(imgui_tag_version v1.91.9)
set(imgui_sfml_tag_version v3.0)
set(asio_tag_version asio-1-36-0)

# Project Sources
set(project_sources
    src/RoboMotion.cpp
)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Enable FetchContent
include(FetchContent)
# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Platform-specific settings
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# ============================================================================
# Fetch SFML
# ============================================================================
message(STATUS "Fetching SFML...")
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG ${sfml_tag_version}
)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
set(SFML_BUILD_GRAPHICS TRUE)
FetchContent_MakeAvailable(SFML)
set_target_properties(sfml-main sfml-system sfml-window sfml-graphics PROPERTIES FOLDER "Dependencies/SFML")
# ============================================================================
# Fetch ImGui
# ============================================================================
message(STATUS "Fetching ImGui...")
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG ${imgui_tag_version}
)
FetchContent_MakeAvailable(imgui)

# Create ImGui library
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
set_target_properties(imgui PROPERTIES FOLDER "Dependencies/ImGui")
# ============================================================================
# Fetch ImGui-SFML
# ============================================================================
message(STATUS "Fetching ImGui-SFML...")
FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG ${imgui_sfml_tag_version}
)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)
FetchContent_MakeAvailable(imgui-sfml)
set_target_properties(ImGui-SFML PROPERTIES FOLDER "Dependencies/ImGui-SFML")
# ============================================================================
# Fetch Boost (Asio header-only)
# ============================================================================
message(STATUS "Fetching Boost.Asio...")
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG ${asio_tag_version}
    SOURCE_SUBDIR asio
)
FetchContent_MakeAvailable(asio)
# Create header-only Asio interface library
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)

# Platform-specific definitions for Asio
if(WIN32)
    target_compile_definitions(asio INTERFACE _WIN32_WINNT=0x0601)
endif()

set_target_properties(asio PROPERTIES FOLDER "Dependencies/Asio")
# ============================================================================
# Create executable
# ============================================================================
add_executable(${PROJECT_NAME} ${project_sources})
add_subdirectory(src/libraryIntegration)
add_subdirectory(src/visualization)


# ============================================================================
# Link libraries
# ============================================================================
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    ImGui-SFML::ImGui-SFML
    imgui
    asio
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
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
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