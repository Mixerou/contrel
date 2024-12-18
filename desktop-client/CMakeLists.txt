cmake_minimum_required(VERSION 3.30 FATAL_ERROR)
project(contrel_desktop_client VERSION 0.0.1 LANGUAGES CXX)

# C
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-constant-logical-operand") # I need this, because some dependencies ruin the compilation

# C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(USE_ZLIB "Enable ZLIB support for the IXWebSocket" OFF)
option(USE_TLS "Enable TLS support for the IXWebSocket" ON)
option(MBEDTLS_FOUND "Mark MbedTLS as found for the IXWebSocket" ON)
option(USE_MBED_TLS "Use MbedTLS for the TLS support for the IXWebSocket" ON)
option(USE_OPEN_SSL "Use OpenSSL for the TLS support for the IXWebSocket" OFF)
option(ENABLE_TESTING "Build tests for the MbedTLS" OFF)
option(ENABLE_PROGRAMS "Build programs for the MbedTLS" OFF)
option(MSGPACK_USE_BOOST "Use Boost for the msgpack" OFF)

# General
include_directories(src src/backend src/ui/layouts src/ui/screens src/ui/widgets src/utils src/workers)
include_directories(libs/glfw/include/GLFW)
include_directories(libs/imgui libs/imgui/misc/freetype libs/imgui/backends)
include_directories(libs/mbedtls/include)
include_directories(libs/stb)

add_subdirectory(libs/freetype)
add_subdirectory(libs/glad)
add_subdirectory(libs/glfw)
add_subdirectory(libs/ix_websocket)
add_subdirectory(libs/keychain)
add_subdirectory(libs/lunasvg)
add_subdirectory(libs/mbedtls)
add_subdirectory(libs/msgpack)

set(IMGUI_SOURCES
        # Base
        libs/imgui/imgui.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp

        # Miscs
        libs/imgui/misc/freetype/imgui_freetype.cpp

        # Backends
        libs/imgui/backends/imgui_impl_glfw.cpp
        libs/imgui/backends/imgui_impl_opengl3.cpp
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_BUILD)
    set(IMGUI_SOURCES
            ${IMGUI_SOURCES}
            libs/imgui/imgui_demo.cpp
    )
endif ()

set(SOURCES
        src/main.cc
        src/app.cc

        # Backend
        src/backend/api.cc

        # UI Layouts
        src/ui/layouts/app.cc
        src/ui/layouts/auth.cc

        # UI Screens
        src/ui/screens/auth.cc
        src/ui/screens/dashboard.cc
        src/ui/screens/hotels.cc

        # UI Widgets
        src/ui/widgets/app_badges.cc
        src/ui/widgets/buttons.cc
        src/ui/widgets/inputs.cc
        src/ui/widgets/layout.cc
        src/ui/widgets/tables.cc
        src/ui/widgets/texts.cc

        # Utils
        src/utils/string.cc
        src/utils/texture.cc
        src/utils/time.cc

        # Workers
        src/workers/api.cc
)

# Definitions
add_compile_definitions(IMGUI_USER_CONFIG="imgui_config.h")

# Target
add_executable(${PROJECT_NAME} ${IMGUI_SOURCES} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX _debug)

# Deps
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} freetype OpenGL::GL glad glfw ixwebsocket::ixwebsocket keychain lunasvg::lunasvg mbedtls mbedcrypto mbedx509 msgpack-cxx)
