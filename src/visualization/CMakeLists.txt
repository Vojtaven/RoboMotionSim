message(STATUS "Making: ${CMAKE_CURRENT_SOURCE_DIR}")
set(ASSET_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../assets")
set(FONT_OUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/embeddedFont.h)
set(ICON_OUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/embeddedIcon.h)

file(GLOB_RECURSE LIBRARY_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_custom_command(
    OUTPUT ${FONT_OUT_FILE}
    COMMAND ${CMAKE_COMMAND}
        -DINPUT_FILE=${ASSET_FOLDER}/font/ProggyClean.ttf
        -DASSET_NAME="DEFAULT_FONT"
        -DOUTPUT_FILE=${FONT_OUT_FILE}
        -P ${ASSET_FOLDER}/embedAsset.cmake
    DEPENDS ${ASSET_FOLDER}/font/ProggyClean.ttf ${ASSET_FOLDER}/embedAsset.cmake
    COMMENT "Embedding default font"
)

add_custom_command(
    OUTPUT ${ICON_OUT_FILE}
    COMMAND ${CMAKE_COMMAND}
        -DINPUT_FILE=${ASSET_FOLDER}/robotIcon.png
        -DASSET_NAME="APP_ICON"
        -DOUTPUT_FILE=${ICON_OUT_FILE}
        -P ${ASSET_FOLDER}/embedAsset.cmake
    DEPENDS ${ASSET_FOLDER}/robotIcon.png ${ASSET_FOLDER}/embedAsset.cmake
    COMMENT "Embedding app icon"
)


set(MODULE_NAME visualization)
add_library(${MODULE_NAME} STATIC
    ${LIBRARY_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/embeddedFont.h
    ${CMAKE_CURRENT_BINARY_DIR}/embeddedIcon.h
)
# Link required libraries
target_link_libraries(${MODULE_NAME} PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    imgui
    ImGui-SFML
    configManager
    nfd
)

target_link_libraries(${MODULE_NAME} PUBLIC
    core
)
 
target_include_directories(${MODULE_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIBRARY_SOURCES})
source_group("Generated headers" FILES ${FONT_OUT_FILE} ${ICON_OUT_FILE})
set_target_properties(${MODULE_NAME} PROPERTIES FOLDER "Project Modules")