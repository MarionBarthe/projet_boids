# ---Choose CMake version---
cmake_minimum_required(VERSION 3.20)

# ---Choose project name---
project(BoidsCube)

# ---Declare source files---
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE src)

# ---Choose C++ version---
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# ---Choose warning level---
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -pedantic-errors -Wimplicit-fallthrough)
endif()

# ---Maybe enable warnings as errors---
set(WARNINGS_AS_ERRORS OFF CACHE BOOL "ON iff you want to treat warnings as errors")

if(WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /WX)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
    endif()
endif()

# ---Setup Testing---
include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG ae7a13539fb71f270b87eb2e874fbac80bc8dda2
)
FetchContent_MakeAvailable(doctest)
target_link_libraries(${PROJECT_NAME} PRIVATE doctest::doctest)

# ---Add p6 library---
set(P6_RAW_OPENGL_MODE ON CACHE BOOL "")
FetchContent_Declare(
    p6
    GIT_REPOSITORY https://github.com/julesfouchy/p6
    GIT_TAG c14c6fc641aac670becb79a80f2a56b047376064
)
FetchContent_MakeAvailable(p6)
target_link_libraries(${PROJECT_NAME} PRIVATE p6::p6)


# ---Copy the assets and the shaders to the output folder (where the executable is created)---
Cool__target_copy_folder(${PROJECT_NAME} assets)
# Cool__target_copy_folder(${PROJECT_NAME} src/shaders shaders)