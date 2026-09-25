cmake_minimum_required(VERSION 3.24)

project(th06 LANGUAGES C CXX)

# ============================================================
# Basic settings
# ============================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "")

# ============================================================
# iOS / ARM64
# ============================================================

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    message(STATUS "========================================")
    message(STATUS "Building Touhou th06 for iOS")
    message(STATUS "Architecture: ${CMAKE_OSX_ARCHITECTURES}")
    message(STATUS "SDK: ${CMAKE_OSX_SYSROOT}")
    message(STATUS "========================================")
endif()

# ============================================================
# SDL2
# ============================================================

set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

# Some SDL2-dependent projects use SDL2_SHARED.
# Keep it explicitly disabled so SDL2_image agrees with SDL2.
set(SDL2_SHARED OFF CACHE BOOL "" FORCE)

add_subdirectory(
    external/SDL
)

# ============================================================
# SDL2_image
# ============================================================

set(SDL2IMAGE_VENDORED ON CACHE BOOL "" FORCE)
set(SDL2IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_TESTS OFF CACHE BOOL "" FORCE)

set(SDL2IMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_JXL OFF CACHE BOOL "" FORCE)

add_subdirectory(
    external/SDL_image
)

# ============================================================
# SDL2_ttf
# ============================================================

set(SDL2TTF_VENDORED ON CACHE BOOL "" FORCE)
set(SDL2TTF_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2TTF_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(
    external/SDL_ttf
)

# ============================================================
# Source files
# ============================================================

set(TH06_SOURCES

    # Main
    src/main.cpp

    # Graphics
    src/graphics/DirectX.cpp
    src/graphics/Graphics.cpp
    src/graphics/Shader.cpp
    src/graphics/Texture.cpp

    # PBG3
    src/pbg3/Pbg3.cpp

    # SJIS
    src/sjis/Sjis.cpp

    # MIDI
    src/midi/MidiDefault.cpp
)

# ============================================================
# Add extra source files if they exist
# ============================================================

foreach(SOURCE_FILE
    src/Game.cpp
    src/Config.cpp
    src/Logger.cpp
    src/Replay.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Bullet.cpp
    src/Stage.cpp
)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
        list(APPEND TH06_SOURCES "${SOURCE_FILE}")
    endif()
endforeach()

# ============================================================
# Main executable
# ============================================================

add_executable(
    th06
    ${TH06_SOURCES}
)

# ============================================================
# Include directories
# ============================================================

target_include_directories(
    th06
    PRIVATE

    ${CMAKE_CURRENT_SOURCE_DIR}/src
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
# Compiler options
# ============================================================

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(
        th06
        PRIVATE

        -Wno-deprecated-declarations
    )
endif()

# ============================================================
# SDL2 libraries
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

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")

    find_library(UIKIT_FRAMEWORK UIKit)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(AVFOUNDATION_FRAMEWORK AVFoundation)
    find_library(AUDIO_TOOLBOX_FRAMEWORK AudioToolbox)
    find_library(CORE_AUDIO_FRAMEWORK CoreAudio)
    find_library(METAL_FRAMEWORK Metal)
    find_library(QUARTZ_CORE_FRAMEWORK QuartzCore)
    find_library(GAME_CONTROLLER_FRAMEWORK GameController)

    target_link_libraries(
        th06
        PRIVATE

        ${UIKIT_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
        ${AVFOUNDATION_FRAMEWORK}
        ${AUDIO_TOOLBOX_FRAMEWORK}
        ${CORE_AUDIO_FRAMEWORK}
        ${METAL_FRAMEWORK}
        ${QUARTZ_CORE_FRAMEWORK}
        ${GAME_CONTROLLER_FRAMEWORK}
    )

endif()

# ============================================================
# Generate embedded shaders
# ============================================================

file(
    MAKE_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/src/autogenerated
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/src/autogenerated/Embeds.hpp

    COMMAND
        ${CMAKE_COMMAND}

        -DINPUT_VERT=${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/ff.vert

        -DINPUT_FRAG=${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/ff.frag

        -DOUTPUT=${CMAKE_CURRENT_SOURCE_DIR}/src/autogenerated/Embeds.hpp

        -P
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/embed_shaders.cmake

    DEPENDS

        src/graphics/ff.vert
        src/graphics/ff.frag
        cmake/embed_shaders.cmake
)

add_custom_target(
    generate_embeds
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/autogenerated/Embeds.hpp
)

add_dependencies(
    th06
    generate_embeds
)

# ============================================================
# Application bundle
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

    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleDisplayName
        "東方紅魔郷"

    XCODE_ATTRIBUTE_INFOPLIST_KEY_LSRequiresIPhoneOS
        "YES"
)

# ============================================================
# iOS deployment target
# ============================================================

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")

    set_target_properties(
        th06
        PROPERTIES

        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET
            "13.0"

        XCODE_ATTRIBUTE_ARCHS
            "arm64"

        XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH
            "NO"

        XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED
            "NO"

        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED
            "NO"
    )

endif()

# ============================================================
# Output information
# ============================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "th06 CMake configuration complete")
message(STATUS "========================================")
message(STATUS "System:       ${CMAKE_SYSTEM_NAME}")
message(STATUS "Architecture: ${CMAKE_OSX_ARCHITECTURES}")
message(STATUS "SDK:          ${CMAKE_OSX_SYSROOT}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "========================================")
