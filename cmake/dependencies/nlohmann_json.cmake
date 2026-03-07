# ============================================================================
# Fetch Nlohmann json
# ============================================================================
include_guard(GLOBAL)
message(STATUS "Fetching Nlohmann json...")
FetchContent_Declare(
    nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG ${nlohmann_json_version}
)

FetchContent_MakeAvailable(nlohmann_json)
set_target_properties(
    nlohmann_json 
    PROPERTIES FOLDER "Dependencies/Nlohmann_json"
)