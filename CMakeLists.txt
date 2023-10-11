cmake_minimum_required(VERSION 3.0)
project(ChessEngine)

set(
    includes
    main.cpp
    board.cpp
    hash.cpp
    attack.cpp
    notation.cpp
    move_generator.cpp
    move_maker.cpp
    validate.cpp
    test.cpp
    search.cpp search.hpp pvtable.cpp evaluate.cpp)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include_directories(${PROJECT_SOURCE_DIR})
add_executable(chess ${includes})