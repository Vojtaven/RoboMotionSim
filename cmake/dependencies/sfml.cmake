# ============================================================================
# Fetch SFML
# ============================================================================
include_guard(GLOBAL)

message(STATUS "Fetching SFML...")

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG ${sfml_tag_version}
)

# Configure SFML options BEFORE MakeAvailable
set(SFML_STATIC_LIBRARIES ON CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

if(WIN32)
    set_target_properties(sfml-main PROPERTIES FOLDER "Dependencies/SFML")
endif()

# Organize targets in IDE
set_target_properties(
    sfml-system
    sfml-window
    sfml-graphics
    PROPERTIES FOLDER "Dependencies/SFML"
)