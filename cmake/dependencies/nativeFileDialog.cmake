# ============================================================================
# Fetch Native File Dialog
# ============================================================================
include_guard(GLOBAL)
FetchContent_Declare(
  nfd
  GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
  GIT_TAG        v1.3.0 
)

set(NFD_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(nfd)
set_target_properties(
    nfd
    PROPERTIES FOLDER "Dependencies/NativeFileDialog" 
)