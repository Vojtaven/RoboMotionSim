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
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

# Organize targets in IDE
set_target_properties(
    sfml-main
    sfml-system
    sfml-window
    sfml-graphics
    PROPERTIES FOLDER "Dependencies/SFML"
)