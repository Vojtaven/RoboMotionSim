include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/module_helpers.cmake)

include(FetchContent)

# Libraries versions
set(sfml_tag_version 3.0.2)
set(imgui_tag_version v1.91.9)
set(imgui_sfml_tag_version v3.0)
set(asio_tag_version asio-1-36-0)
set(nlohmann_json_version v3.12.0)
# Commit hash instead of official release for bypassing CMake minimum version issue
set(libzmq_tag_version dbbd240) 
set(cppzmq_tag_version v4.11.0) 
# Individual dependencies
include(${CMAKE_CURRENT_LIST_DIR}/dependencies/sfml.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dependencies/imgui.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dependencies/imgui_sfml.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dependencies/asio.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dependencies/nlohmann_json.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dependencies/exprtk.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dependencies/zeroMq.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dependencies/nativeFileDialog.cmake)