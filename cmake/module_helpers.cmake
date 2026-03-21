include_guard(GLOBAL)

# Macro to gather module sources (.cpp and .hpp) via GLOB_RECURSE.
# Sets LIBRARY_SOURCES in the calling scope.
macro(gather_module_sources)
    message(STATUS "Making: ${CMAKE_CURRENT_SOURCE_DIR}")
    file(GLOB_RECURSE LIBRARY_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    )
endmacro()

# Macro to finalize a module target: set up source_group and folder property.
# Call this after add_library().
macro(finalize_module TARGET_NAME)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIBRARY_SOURCES})
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Project Modules")
endmacro()
