# ============================================================================
# Fetch ImGui-SFML
# ============================================================================
include_guard(GLOBAL)
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
set_target_properties(ImGui-SFML
 PROPERTIES FOLDER "Dependencies/ImGui-SFML"
 )