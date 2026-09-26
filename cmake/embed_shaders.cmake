file(READ "${INPUT_VERT}" VERT_HEX HEX)
file(READ "${INPUT_FRAG}" FRAG_HEX HEX)

string(LENGTH "${VERT_HEX}" VERT_LEN)
string(LENGTH "${FRAG_HEX}" FRAG_LEN)

file(WRITE "${OUTPUT}" "#pragma once\n\n")

# Vertex shader
file(APPEND "${OUTPUT}" "static const unsigned char vertShaderBytes[] = {\n")

if(VERT_LEN GREATER 0)
    math(EXPR VERT_LAST "${VERT_LEN} - 2")

    foreach(i RANGE 0 ${VERT_LAST} 2)
        string(SUBSTRING "${VERT_HEX}" ${i} 2 BYTE)
        file(APPEND "${OUTPUT}" "0x${BYTE}, ")
    endforeach()
endif()

file(APPEND "${OUTPUT}" "0};\n\n")

# Fragment shader
file(APPEND "${OUTPUT}" "static const unsigned char fragShaderBytes[] = {\n")

if(FRAG_LEN GREATER 0)
    math(EXPR FRAG_LAST "${FRAG_LEN} - 2")

    foreach(i RANGE 0 ${FRAG_LAST} 2)
        string(SUBSTRING "${FRAG_HEX}" ${i} 2 BYTE)
        file(APPEND "${OUTPUT}" "0x${BYTE}, ")
    endforeach()
endif()

file(APPEND "${OUTPUT}" "0};\n")
