message(STATUS "Making: ${CMAKE_CURRENT_SOURCE_DIR}")

set (CONFIG_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/ConfigFiles")
set(APP_DEFAULT_CONFIG_OUT ${CMAKE_CURRENT_BINARY_DIR}/AppDefaultConfig.h)
set(ROBOT_DEFAULT_CONFIG_OUT  ${CMAKE_CURRENT_BINARY_DIR}/RobotDefaultConfig.h)

file(GLOB LIBRARY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

set(CONFIG_FILES
    "${CONFIG_FOLDER}/outTestConfig.ini"
    "${CONFIG_FOLDER}/differential_config.ini"
    "${CONFIG_FOLDER}/mecanum_config.ini"
)

set(DEFAULT_CONFIG_FILES
    "${CONFIG_FOLDER}/app_config.json"
    "${CONFIG_FOLDER}/omni_config.ini"
)


add_custom_command(
    OUTPUT ${APP_DEFAULT_CONFIG_OUT}
    COMMAND ${CMAKE_COMMAND}
        -DINPUT_FILE=${CONFIG_FOLDER}/app_config.json
        -DCONFIG_NAME="DEFAULT_APP_CONFIG"
        -DOUTPUT_FILE=${APP_DEFAULT_CONFIG_OUT}
        -P ${CONFIG_FOLDER}/embedConfig.cmake
    DEPENDS ${CONFIG_FOLDER}/app_config.json ${CONFIG_FOLDER}/embedConfig.cmake
    COMMENT "Embedding default app config"
)

add_custom_command(
    OUTPUT ${ROBOT_DEFAULT_CONFIG_OUT}
    COMMAND ${CMAKE_COMMAND}
        -DINPUT_FILE=${CONFIG_FOLDER}/omni_config.ini
        -DCONFIG_NAME="DEFAULT_ROBOT_CONFIG"
        -DOUTPUT_FILE=${ROBOT_DEFAULT_CONFIG_OUT}
        -P ${CONFIG_FOLDER}/embedConfig.cmake
    DEPENDS ${CONFIG_FOLDER}/omni_config.ini ${CONFIG_FOLDER}/embedConfig.cmake
    COMMENT "Embedding default robot config"
)

set(MODULE_NAME libraryIntegration)

add_library(${MODULE_NAME} STATIC 
    ${LIBRARY_SOURCES} 
    ${CONFIG_FILES}
    ${DEFAULT_CONFIG_FILES}
    ${ROBOT_DEFAULT_CONFIG_OUT}
    ${APP_DEFAULT_CONFIG_OUT}
)

target_include_directories(${MODULE_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}     
    ${CMAKE_CURRENT_BINARY_DIR} 
)
target_link_libraries(${MODULE_NAME} PUBLIC exprtk)
target_link_libraries(${MODULE_NAME} PUBLIC core)
target_link_libraries(${MODULE_NAME} PUBLIC nlohmann_json)
set_target_properties(${MODULE_NAME} PROPERTIES FOLDER "Project Modules")

source_group("Config Files" FILES ${CONFIG_FILES})
source_group("Default Config Files" FILES ${DEFAULT_CONFIG_FILES})
source_group("Generated Config Headers" FILES 
    ${ROBOT_DEFAULT_CONFIG_OUT}
    ${APP_DEFAULT_CONFIG_OUT}
)