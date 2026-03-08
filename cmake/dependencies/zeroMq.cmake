# ============================================================================
# Fetch ZeroMQ (libzmq and cppzmq) 
# ============================================================================
include_guard(GLOBAL)
message(STATUS "Fetching ZeroMQ...")

FetchContent_Declare(
    ZeroMQ
    GIT_REPOSITORY https://github.com/zeromq/libzmq.git
    GIT_TAG ${libzmq_tag_version}
)

set(BUILD_SHARED_LIBS    OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS          OFF CACHE BOOL "" FORCE)
set(WITH_DOCS            OFF CACHE BOOL "" FORCE)
set(ENABLE_CLANG         OFF CACHE BOOL "" FORCE)

# Disable optional transports/features to slim down the build
set(WITH_TLS             OFF CACHE BOOL "" FORCE)   # kills the GnuTLS warning
set(ENABLE_WS            OFF CACHE BOOL "" FORCE)   # disables WebSocket transport (needs TLS)
set(ENABLE_DRAFTS        OFF CACHE BOOL "" FORCE)   # disables draft APIs
set(WITH_LIBSODIUM       OFF CACHE BOOL "" FORCE)   # disables CURVE security (already was off)
set(ENABLE_CURVE         OFF CACHE BOOL "" FORCE)   # explicitly off
set(BUILD_SHARED         OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(ZeroMQ)

if(TARGET libzmq-static)
    set(_zmq_target libzmq-static)
elseif(TARGET libzmq)
    set(_zmq_target libzmq)
endif()

set_target_properties(${_zmq_target} PROPERTIES FOLDER "Dependencies/ZeroMQ")

# -------------------------
# Fetch cppzmq (header-only C++ wrapper)
# -------------------------
FetchContent_Declare(
    cppzmq
    GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
    GIT_TAG ${cppzmq_tag_version}
)

# Prevent cppzmq from building its own tests/examples (which trigger find_package issues)
set(CPPZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(cppzmq)

# cppzmq already defines its own "cppzmq" INTERFACE target — just alias it
if(NOT TARGET ZeroMQ::cppzmq)
    add_library(ZeroMQ::cppzmq ALIAS cppzmq)
endif()

set_target_properties(cppzmq PROPERTIES FOLDER "Dependencies/ZeroMQ")