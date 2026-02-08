
add_library(core STATIC
    RobotConfig.hpp
    RobotConfig.cpp
    RobotState.hpp
    MathUtils.hpp
    AppConfig.hpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(core PROPERTIES FOLDER "Project Modules")