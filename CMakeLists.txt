cmake_minimum_required(VERSION 4.0)

project(dblu C)

set(CMAKE_C_STANDARD 11)


if (NOT IVS_ROOT)
    message(FATAL_ERROR "Variable IVS_ROOT is not set. Please specify it using -DIVS_ROOT=path_to_libivs")
endif ()


set(DBLU_INCLUDE_LOCAL ./include)

add_subdirectory(${IVS_ROOT} ./ivs)

add_library(dblu STATIC)

file(GLOB_RECURSE DBLU_SRCS ./src/*.c)
target_sources(dblu PRIVATE ${DBLU_SRCS})
target_include_directories(dblu PRIVATE ${DBLU_INCLUDE_LOCAL})
target_link_libraries(dblu PRIVATE ivs)
target_include_directories(dblu PRIVATE ${IVS_ROOT}/include)
target_compile_definitions(dblu PRIVATE $<$<CONFIG:Release,MinSizeRel>:IVS_NDEBUG>)
