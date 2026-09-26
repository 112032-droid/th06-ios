cmake_minimum_required(VERSION 3.24)
project(th06 LANGUAGES C CXX OBJC)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# ============================================================
# iOS / Xcode code signing
# ============================================================
if(CMAKE_GENERATOR STREQUAL "Xcode")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
endif()
message(STATUS "========================================")
message(STATUS "Building Touhou th06 for iOS")
message(STATUS "Architecture: ${CMAKE_OSX_ARCHITECTURES}")
message(STATUS "SDK: ${CMAKE_OSX_SYSROOT}")
message(STATUS "========================================")
# ============================================================
# SDL2
# ============================================================
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL2_SHARED OFF CACHE BOOL "" FORCE)
set(SDL2_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(
    external/SDL
    ${CMAKE_BINARY_DIR}/SDL
)
# ============================================================
# SDL2_image
# ============================================================
set(SDL2IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_TESTS OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_VENDORED ON CACHE BOOL "" FORCE)
set(SDL2IMAGE_SHARED OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_JXL OFF CACHE BOOL "" FORCE)
add_subdirectory(
    external/SDL_image
    ${CMAKE_BINARY_DIR}/SDL_image
)
# ============================================================
# SDL2_ttf
# ============================================================
set(SDL2TTF_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2TTF_TESTS OFF CACHE BOOL "" FORCE)
set(SDL2TTF_VENDORED ON CACHE BOOL "" FORCE)
add_subdirectory(
    external/SDL_ttf
    ${CMAKE_BINARY_DIR}/SDL_ttf
)
# ============================================================
# th06 source files
# ============================================================
file(GLOB_RECURSE TH06_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
# ============================================================
# iOS source filtering
#
# Windows:
#   MidiWin32.cpp
#   使用 Windows 專用 mmsystem.h
#
# Linux:
#   MidiAlsa.cpp
#   使用 Linux ALSA:
#   alsa/asoundlib.h
#
# iOS:
#   使用 MidiCoreAudio.cpp
#   使用 Apple CoreAudio
# ============================================================
if(IOS)
    message(STATUS "========================================")
    message(STATUS "iOS source filtering")
    message(STATUS "========================================")
    # --------------------------------------------------------
    # 排除 Windows MIDI backend
    # --------------------------------------------------------
    list(FILTER TH06_SOURCES EXCLUDE REGEX
        ".*/MidiWin32\\.cpp$"
    )
    message(STATUS "Excluded Windows MIDI backend:")
    message(STATUS "  src/midi/MidiWin32.cpp")
    # --------------------------------------------------------
    # 排除 Linux ALSA MIDI backend
    # --------------------------------------------------------
    list(FILTER TH06_SOURCES EXCLUDE REGEX
        ".*/MidiAlsa\\.cpp$"
    )
    message(STATUS "Excluded Linux ALSA MIDI backend:")
    message(STATUS "  src/midi/MidiAlsa.cpp")
    # --------------------------------------------------------
    # 保留 Apple CoreAudio MIDI backend
    # --------------------------------------------------------
    message(STATUS "Using Apple CoreAudio MIDI backend:")
    message(STATUS "  src/midi/MidiCoreAudio.cpp")
endif()
# ============================================================
# Display source files
# ============================================================
message(STATUS "========================================")
message(STATUS "Found th06 source files:")
message(STATUS "========================================")
foreach(SOURCE_FILE ${TH06_SOURCES})
    message(STATUS "  ${SOURCE_FILE}")
endforeach()
list(LENGTH TH06_SOURCES TH06_SOURCE_COUNT)
message(STATUS "========================================")
message(STATUS "Total source files: ${TH06_SOURCE_COUNT}")
message(STATUS "========================================")
# ============================================================
# Executable
# ============================================================
add_executable(
    th06
    ${TH06_SOURCES}
)
# ============================================================
# Include directories
#
# include/
# └── SDL2/
#     ├── SDL.h
#     ├── SDL_image.h
#     └── SDL_ttf.h
#
# SDL_ttf.h 實際來源：
# external/SDL_ttf/SDL_ttf.h
# ============================================================
target_include_directories(
    th06
    PRIVATE
    # Unified SDL2 headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # Project source
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    # SDL2
    ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include
    # SDL2_image
    ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_image/include
    # SDL2_ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_ttf
)
# ============================================================
# Compile definitions
# ============================================================
target_compile_definitions(
    th06
    PRIVATE
    NDEBUG
)
# ============================================================
# Compile options
# ============================================================
target_compile_options(
    th06
    PRIVATE
    -Wno-deprecated-declarations
)
# ============================================================
# Libraries
# ============================================================
target_link_libraries(
    th06
    PRIVATE
    SDL2::SDL2-static
    SDL2_image
    SDL2_ttf
)
# ============================================================
# iOS frameworks
# ============================================================
if(IOS)
    target_link_libraries(
        th06
        PRIVATE
        "-framework Foundation"
        "-framework UIKit"
        "-framework AudioToolbox"
        "-framework AVFoundation"
        "-framework CoreAudio"
        "-framework CoreGraphics"
        "-framework CoreMotion"
        "-framework GameController"
        "-framework Metal"
        "-framework OpenGLES"
        "-framework QuartzCore"
    )
endif()
# ============================================================
# Shader embedding
# ============================================================
file(MAKE_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/src/autogenerated"
)
set(EMBED_HEADER
    "${CMAKE_CURRENT_SOURCE_DIR}/src/autogenerated/Embeds.hpp"
)
if(
    EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/ff.vert"
    AND
    EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/ff.frag"
)
    add_custom_command(
        OUTPUT
            "${EMBED_HEADER}"
        COMMAND
            ${CMAKE_COMMAND}
            -DINPUT_VERT="${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/ff.vert"
            -DINPUT_FRAG="${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/ff.frag"
            -DOUTPUT="${EMBED_HEADER}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/embed_shaders.cmake"
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/ff.vert"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/ff.frag"
            "${CMAKE_CURRENT_SOURCE_DIR}/embed_shaders.cmake"
        VERBATIM
    )
    add_custom_target(
        generate_embeds
        DEPENDS
            "${EMBED_HEADER}"
    )
    add_dependencies(
        th06
        generate_embeds
    )
endif()
# ============================================================
# iOS application bundle
# ============================================================
set_target_properties(
    th06
    PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER
        "com.example.th06"
    MACOSX_BUNDLE_BUNDLE_NAME
        "Touhou Scarlet Devil"
    MACOSX_BUNDLE_BUNDLE_VERSION
        "1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING
        "1.0"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER
        "com.example.th06"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY
        "1"
    XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED
        "NO"
    XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED
        "NO"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY
        ""
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleDisplayName
        "東方紅魔郷"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_LSRequiresIPhoneOS
        "YES"
)
# ============================================================
# Final information
# ============================================================
message(STATUS "========================================")
message(STATUS "th06 CMake configuration complete")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Architecture: ${CMAKE_OSX_ARCHITECTURES}")
message(STATUS "SDK: ${CMAKE_OSX_SYSROOT}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source count: ${TH06_SOURCE_COUNT}")
message(STATUS "========================================")
