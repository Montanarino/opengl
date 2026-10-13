cmake_minimum_required(VERSION 3.14)

project(GLFWApp C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --------------------------------------------------
# GLFW: desktop oppure implementazione Emscripten
# --------------------------------------------------

if(NOT EMSCRIPTEN)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
    )

    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(glfw)
endif()

# --------------------------------------------------
# GLAD 2
# --------------------------------------------------

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
)

FetchContent_MakeAvailable(glad)

add_subdirectory(
    "${glad_SOURCE_DIR}/cmake"
    glad_cmake
)

if(EMSCRIPTEN)
    glad_add_library(
        glad
        REPRODUCIBLE
        LOCATION "${CMAKE_CURRENT_BINARY_DIR}/glad"
        API gles2=3.0
    )
else()
    glad_add_library(
        glad
        REPRODUCIBLE
        LOCATION "${CMAKE_CURRENT_BINARY_DIR}/glad"
        API gl:core=3.3
    )
endif()

# --------------------------------------------------
# GLM
# --------------------------------------------------

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(glm)

# --------------------------------------------------
# stb
# --------------------------------------------------

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

FetchContent_MakeAvailable(stb)

# --------------------------------------------------
# Application
# --------------------------------------------------

add_executable(my_app
    main.cpp
)

# Prevent GLFW from including default GL headers that conflict with GLAD
target_compile_definitions(my_app PRIVATE GLFW_INCLUDE_NONE)

target_include_directories(my_app PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${stb_SOURCE_DIR}"
)

target_link_libraries(my_app PRIVATE
    glad
    glm::glm
)

if(EMSCRIPTEN)
    target_link_options(my_app PRIVATE
        "--shell-file"
        "${CMAKE_SOURCE_DIR}/shell.html"
        "--preload-file"
        "${CMAKE_SOURCE_DIR}/assets@/assets"
        "-sUSE_GLFW=3"
        "-sMIN_WEBGL_VERSION=2"
        "-sMAX_WEBGL_VERSION=2"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sASSERTIONS=1"
    )

    # Automatically rebuild my_app.html if you edit shell.html
    set_target_properties(my_app PROPERTIES
        SUFFIX ".html"
        LINK_DEPENDS "${CMAKE_SOURCE_DIR}/shell.html"
    )
else()
    target_link_libraries(my_app PRIVATE glfw)

    if(WIN32 OR MINGW)
        target_link_options(my_app PRIVATE
            -static-libgcc
            -static-libstdc++
            -static
        )
    endif()
endif()