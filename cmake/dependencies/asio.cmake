# ============================================================================
# Fetch Boost (Asio header-only)
# ============================================================================
include_guard(GLOBAL)
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

set_target_properties(
    asio 
    PROPERTIES FOLDER "Dependencies/Asio"
)