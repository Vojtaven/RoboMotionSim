file(READ "${INPUT_FILE}" content HEX)

string(LENGTH "${content}" content_length)
math(EXPR array_size "${content_length} / 2")

# Convert hex string to C array format
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " array_content "${content}")
string(REGEX REPLACE ", $" "" array_content "${array_content}")

# Write header file
file(WRITE "${OUTPUT_FILE}" 
"#pragma once
#include <cstddef>

inline constexpr size_t ${ASSET_NAME}_DATA_SIZE = ${array_size};

inline constexpr unsigned char ${ASSET_NAME}_DATA[${ASSET_NAME}_DATA_SIZE] = {
    ${array_content}
};
")