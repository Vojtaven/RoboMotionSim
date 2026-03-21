gather_module_sources()

set (CONFIG_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles")
set (ASSET_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../assets")
set(ROBOT_DEFAULT_CONFIG_OUT ${CMAKE_CURRENT_BINARY_DIR}/RobotDefaultConfig.h)

set(CONFIG_FILES
    "${CONFIG_FOLDER}/outTestConfig.ini"
    "${CONFIG_FOLDER}/differential_config.ini"
    "${CONFIG_FOLDER}/mecanum_config.ini"
    "${CONFIG_FOLDER}/omni_config.ini"
)

add_custom_command(
    OUTPUT ${ROBOT_DEFAULT_CONFIG_OUT}
    COMMAND ${CMAKE_COMMAND}
        -DINPUT_FILE=${CONFIG_FOLDER}/omni_config.ini
        -DASSET_NAME="DEFAULT_ROBOT_CONFIG"
        -DOUTPUT_FILE=${ROBOT_DEFAULT_CONFIG_OUT}
        -P ${ASSET_FOLDER}/embedAsset.cmake
    DEPENDS ${CONFIG_FOLDER}/omni_config.ini ${ASSET_FOLDER}/embedAsset.cmake
    COMMENT "Embedding default robot config"
)

set(MODULE_NAME configManager)

add_library(${MODULE_NAME} STATIC
    ${LIBRARY_SOURCES}
    ${CONFIG_FILES}
    ${ROBOT_DEFAULT_CONFIG_OUT}
)

target_include_directories(${MODULE_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(${MODULE_NAME} PUBLIC exprtk)
target_link_libraries(${MODULE_NAME} PUBLIC core)
target_link_libraries(${MODULE_NAME} PRIVATE nlohmann_json)
finalize_module(${MODULE_NAME})
source_group("Config Files" FILES ${CONFIG_FILES})
source_group("Generated Config Headers" FILES
    ${ROBOT_DEFAULT_CONFIG_OUT}
)
