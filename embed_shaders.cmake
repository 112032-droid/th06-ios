# ==========================================
# 防呆機制：自動清除外部傳入時可能夾帶的雙引號
# ==========================================
string(REPLACE "\"" "" INPUT_VERT "${INPUT_VERT}")
string(REPLACE "\"" "" INPUT_FRAG "${INPUT_FRAG}")
string(REPLACE "\"" "" OUTPUT "${OUTPUT}")

# 讀取 Vertex Shader 檔案
file(READ "${INPUT_VERT}" VERT_HEX HEX)
# 讀取 Fragment Shader 檔案
file(READ "${INPUT_FRAG}" FRAG_HEX HEX)

string(LENGTH "${VERT_HEX}" VERT_LEN)
string(LENGTH "${FRAG_HEX}" FRAG_LEN)

file(WRITE "${OUTPUT}" "#pragma once\n\n")

file(APPEND "${OUTPUT}" "static const unsigned char vertShaderBytes[] = {\n")
math(EXPR VERT_LAST "${VERT_LEN} - 2")
foreach(i RANGE 0 ${VERT_LAST} 2)
    string(SUBSTRING "${VERT_HEX}" ${i} 2 BYTE)
    file(APPEND "${OUTPUT}" "0x${BYTE}, ")
endforeach()
file(APPEND "${OUTPUT}" "0};\n\n")

file(APPEND "${OUTPUT}" "static const unsigned char fragShaderBytes[] = {\n")
math(EXPR FRAG_LAST "${FRAG_LEN} - 2")
foreach(i RANGE 0 ${FRAG_LAST} 2)
    string(SUBSTRING "${FRAG_HEX}" ${i} 2 BYTE)
    file(APPEND "${OUTPUT}" "0x${BYTE}, ")
endforeach()
file(APPEND "${OUTPUT}" "0};\n")
