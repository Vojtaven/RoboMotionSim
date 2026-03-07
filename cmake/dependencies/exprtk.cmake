# ============================================================================
# Integrate ExprTk (Header-Only)
# ============================================================================
include_guard(GLOBAL)
message(STATUS "Integrating ExprTk...")
set(EXPRTK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/3rdPartyLibs/exprtk") 
# Create an INTERFACE library to define the include path
add_library(exprtk INTERFACE IMPORTED)
target_include_directories(exprtk INTERFACE ${EXPRTK_INCLUDE_DIR})
set_target_properties(exprtk 
    PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${EXPRTK_INCLUDE_DIR}
)
add_library(ExprTk::ExprTk ALIAS exprtk)
set_target_properties(exprtk PROPERTIES FOLDER "Dependencies/ExprTk")
